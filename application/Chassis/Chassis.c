#include "Chassis.h"
#include "bsp_motor.h"
#include "joystick.h"
#include "Detection.h"
#include "PID.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* ==================== 摇杆 Y 轴 -> 目标转速 ==================== */
/* 摇杆 ADC 为 12 位：0~4095，实测中位 2900（摇杆供电 5V 时中位偏高） */
#define JOYSTICK_Y_MID       2900u
/* 中位附近死区（ADC 单位），松手回中后目标转速为 0 */
#define JOYSTICK_Y_DEADZONE  50u
/* 输出轴最大目标转速：实测电机最高约 800 RPM，推到底时目标 ±500 RPM，可按实际电机调整 */
#define TARGET_RPM_MAX       500.0f

/* ==================== TIM3 编码器测速 ==================== */
/* MG310 霍尔编码器电机参数：
 *   编码器 13 线（13 PPR / 电机轴）
 *   减速比 1:20
 *   CubeMX 编码器模式 3（TI12：双通道双沿计数，四倍频）
 * 所以输出轴转一圈的计数 = 13 x 20 x 4 = 1040 */
#define MOTOR_REDUCTION      20.0f
#define ENCODER_LINES        13.0f
#define ENCODER_MULTIPLY     4.0f
#define PULSES_PER_REV       (MOTOR_REDUCTION * ENCODER_LINES * ENCODER_MULTIPLY) /* 1040 */

/* 速度环采样周期（ms）：任务用 vTaskDelayUntil 严格按此周期执行 */
#define SPEED_SAMPLE_MS      10u

/* 编码器方向：若推杆正转但 OLED 实际转速显示为负，将此宏改为 1
 * （或直接交换编码器 A/B 两根线） */
#define ENCODER_DIR_INVERT   1

/* 供 OLED 显示任务读取的目标/实际转速（输出轴 RPM） */
float motor_target_rpm = 0.0f;
float motor_actual_rpm = 0.0f;

static pid_type_def motor_speed_pid;

/**
 * @brief 摇杆 Y 原始值 -> 目标转速（RPM）
 *   Y=0           -> -TARGET_RPM_MAX
 *   Y=中位±死区   -> 0
 *   Y=4095        -> +TARGET_RPM_MAX
 */
static float JoystickY_To_TargetRpm(uint16_t y)
{
  float rpm = 0.0f;

  if (y < (JOYSTICK_Y_MID - JOYSTICK_Y_DEADZONE))
  {
    rpm = -TARGET_RPM_MAX
          * (float)(JOYSTICK_Y_MID - JOYSTICK_Y_DEADZONE - y)
          / (float)(JOYSTICK_Y_MID - JOYSTICK_Y_DEADZONE);
  }
  else if (y > (JOYSTICK_Y_MID + JOYSTICK_Y_DEADZONE))
  {
    rpm = TARGET_RPM_MAX
          * (float)(y - (JOYSTICK_Y_MID + JOYSTICK_Y_DEADZONE))
          / (float)(4095u - (JOYSTICK_Y_MID + JOYSTICK_Y_DEADZONE));
  }

  return rpm;
}

/**
 * @brief 读取 TIM3 编码器 CNT，按 10ms 采样周期计算实际转速（输出轴 RPM）
 *
 * delta 为两次采样间的计数差，用 int16_t 强转处理 16 位计数器的回绕。
 * 换算公式：
 *   delta 计数 / 10ms = delta x 100 计数/s
 *   RPM = (计数/s) / 1040 x 60 = delta x 6000 / 1040
 */
static void Encoder_UpdateSpeed(void)
{
  static uint16_t last_cnt = 0;
  static uint8_t  first    = 1;
  uint16_t cnt   = __HAL_TIM_GET_COUNTER(&htim3);
  int16_t  delta;

  if (first)
  {
    /* 第一次采样只记录基准值，避免算出无意义的大转速 */
    first = 0;
    last_cnt = cnt;
    motor_actual_rpm = 0.0f;
    return;
  }

  delta = (int16_t)(cnt - last_cnt);
  last_cnt = cnt;

  motor_actual_rpm = (float)delta * 60000.0f
                     / ((float)SPEED_SAMPLE_MS * PULSES_PER_REV);

  if (ENCODER_DIR_INVERT)
  {
    motor_actual_rpm = -motor_actual_rpm;
  }
}

/**
 * @brief 底盘电机控制任务：vTaskDelayUntil 严格 10ms 周期速度闭环
 *   1. 摇杆 Y -> 目标转速
 *   2. TIM3 编码器测速
 *   3. PD 速度环 -> PWM（±100）
 * @param argument 未使用
 */
void Motor_task(void *argument)
{
  (void)argument;

  Motor_Init();
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  __HAL_TIM_SET_COUNTER(&htim3, 0);

  /* PD 初始参数（误差单位 RPM，输出单位 ±100）：
   *   Kp = 0.8：100 RPM 误差 -> 80% 占空比
   *   Ki = 0：纯 PD，不加积分
   *   Kd = 0.1：抑制超调/震荡；震荡加大 Kd，响应慢减小 Kd
   * 推到底仍追不上目标 -> 增大 Kp 或调高 TARGET_RPM_MAX */
  pid_init(&motor_speed_pid, 100u, 0u, 0.8f, 0.1f, 0.1f);

  /* vTaskDelayUntil 固定节奏锚点：从第一次循环开始，每 10ms 唤醒一次，
   * 不会像 osDelay 那样因任务耗时产生累积漂移 */
  TickType_t last_wake_time = xTaskGetTickCount();
  const TickType_t period_ticks = pdMS_TO_TICKS(SPEED_SAMPLE_MS); /* 10ms = 10 tick */

  for (;;)
  {
    motor_target_rpm = JoystickY_To_TargetRpm(joystick.y);
    Encoder_UpdateSpeed();

    /* 电机心跳：有编码器反馈（实际转速≠0），或电机本就静止（目标=0）时视为在线。
     * 推杆有目标但编码器长时间无计数（堵转/反馈断线）-> 不上报心跳，
     * 检测任务在超时后判为电机异常，LED 呼吸 */
    if (motor_target_rpm == 0.0f || motor_actual_rpm != 0.0f)
    {
      detect_handle(DETECT_MOTOR);
    }

    float out = pid_calc(&motor_speed_pid, motor_actual_rpm, motor_target_rpm);
    Motor_SetSpeed((int16_t)out);

    /* 严格 10ms 周期；任务体耗时超过周期时不会死等，而是立即进入下一轮 */
    vTaskDelayUntil(&last_wake_time, period_ticks);
  }
}