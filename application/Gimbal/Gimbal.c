#include "Gimbal.h"
#include "bsp_servo_pwm.h"
#include "joystick.h"
#include "cmsis_os.h"

/* 摇杆 ADC 为 12 位：0~4095，理论中位 2048 */
#define JOYSTICK_X_MID       2048u
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
 * @brief 舵机控制任务：读取摇杆 X 轴，按映射输出 PWM
 * @param argument 未使用
 */
void Servo_task(void *argument)
{
  (void)argument;

  Servo_Init();

  for (;;)
  {
    Servo_SetPulse(JoystickX_To_ServoPulse(joystick.x));
    osDelay(10); /* 100Hz 控制更新，远高于人手的操作频率 */
  }
}