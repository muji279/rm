#include "Gimbal.h"
#include "bsp_servo_pwm.h"
#include "can_send.h"
#include "joystick.h"
#include "cmsis_os.h"

/* 摇杆 ADC 为 12 位：0~4095，理论中位 2048 */
#define JOYSTICK_X_MID       2900u
/* 最大转速：0.18s/60° -> 转一圈 1.08s -> 约 55.56 RPM（数据手册估算值） */
#define SERVO_RPM_MAX        55.56f
/* 中位附近死区（ADC 单位），摇杆松手回中后舵机保持静止。
 * 若发现松手后舵机仍缓慢转动，说明模块中位有偏差，可适当加大该值。 */
#define JOYSTICK_X_DEADZONE  200u

/**
 * @brief 摇杆 X 轴原始值 -> 舵机脉宽（us）
 *
 * 映射规则（速度式，舵机无角度定位）：
 *   X 在死区内        -> 1500us（静止死区中点，转轴保持停止）
 *   X < 中位-死区     -> 顺时针，越偏离中位越快：1400us -> 1000us
 *   X > 中位+死区     -> 逆时针，越偏离中位越快：1600us -> 2000us
 */
static uint16_t JoystickX_To_ServoPulse(uint16_t x)
{
  uint32_t pulse = SERVO_PULSE_STOP;

  if (x < (JOYSTICK_X_MID - JOYSTICK_X_DEADZONE))
  {
    /* 顺时针区：x=0 全速(1000us)，x=死区左边界 静止(1400us) */
    uint32_t range = JOYSTICK_X_MID - JOYSTICK_X_DEADZONE;
    pulse = SERVO_PULSE_MIN
          + (uint32_t)x * (SERVO_PULSE_DEAD_MIN - SERVO_PULSE_MIN) / range;
  }
  else if (x > (JOYSTICK_X_MID + JOYSTICK_X_DEADZONE))
  {
    /* 逆时针区：x=死区右边界 静止(1600us)，x=4095 全速(2000us) */
    uint32_t range = 4095u - (JOYSTICK_X_MID + JOYSTICK_X_DEADZONE);
    pulse = SERVO_PULSE_DEAD_MAX
          + (uint32_t)(x - (JOYSTICK_X_MID + JOYSTICK_X_DEADZONE))
            * (SERVO_PULSE_MAX - SERVO_PULSE_DEAD_MAX) / range;
  }

  return (uint16_t)pulse;
}

/**
 * @brief 舵机脉宽 -> 目标转速（RPM×10，int16）
 *
 * 按数据手册换算（0.18s/60° -> 55.56 RPM）：
 *   1000~1399us  正向（顺时针）：+RPM_MAX * (1400 - p) / 400
 *   1400~1600us  停止：0
 *   1601~2000us  反向（逆时针）：-RPM_MAX * (p - 1600) / 400
 * 用 ×10 定点保存（0.1 RPM 分辨率），最大 ±556。
 */
static int16_t ServoPulse_To_RpmX10(uint16_t pulse)
{
  if (pulse < SERVO_PULSE_DEAD_MIN)
  {
    return (int16_t)(SERVO_RPM_MAX * (float)(SERVO_PULSE_DEAD_MIN - pulse) / 400.0f * 10.0f + 0.5f);
  }
  if (pulse <= SERVO_PULSE_DEAD_MAX)
  {
    return 0;
  }
  return (int16_t)(-SERVO_RPM_MAX * (float)(pulse - SERVO_PULSE_DEAD_MAX) / 400.0f * 10.0f - 0.5f);
}

/**
 * @brief 舵机控制任务：读取摇杆 X 轴，按映射输出 PWM 并上报舵机数据
 * @param argument 未使用
 */
void Servo_task(void *argument)
{
  (void)argument;
  Servo_Data_t servo_data = {0};

  Servo_Init();

  for (;;)
  {
    uint16_t pulse = JoystickX_To_ServoPulse(joystick.x);
    Servo_SetPulse(pulse);

    /* 舵机数据通过 CAN（ID 2）发给底盘板：目标转速（RPM×10）+ 在线状态 */
    servo_data.target_rpm_x10 = ServoPulse_To_RpmX10(pulse);
    servo_data.online = 1; /* 无反馈：PWM 正常输出即视为在线 */
    Send_Servo_Data(&servo_data);

    osDelay(10); /* 100Hz 控制更新，远高于人手的操作频率 */
  }
}