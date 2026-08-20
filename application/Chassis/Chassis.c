#include "Chassis.h"
#include "bsp_motor.h"
#include "joystick.h"
#include "cmsis_os.h"

/* 摇杆 ADC 为 12 位：0~4095，理论中位 2048 */
#define JOYSTICK_Y_MID       2048
/* 中位附近死区（ADC 单位），摇杆松手回中后电机停止。
 * 若松手后电机仍缓慢转动，说明摇杆中位有偏差，可适当加大该值。 */
#define JOYSTICK_Y_DEADZONE  50

/**
 * @brief 摇杆 Y 轴原始值 -> 电机转速（-100 ~ +100）
 *
 * 开环直驱映射（先不做编码器闭环）：
 *   Y=0              -> -100（全速反转）
 *   Y=中位-死区      -> 0
 *   Y=中位+死区      -> 0
 *   Y=4095           -> +100（全速正转）
 * 死区内输出 0，避免中位抖动。
 */
static int16_t JoystickY_To_MotorSpeed(uint16_t y)
{
  int32_t speed = 0;

  if (y < (JOYSTICK_Y_MID - JOYSTICK_Y_DEADZONE))
  {
    /* 反转区：越往下推速度越快，y=0 时 -100 */
    speed = -(int32_t)((JOYSTICK_Y_MID - JOYSTICK_Y_DEADZONE - y) * 100L
                       / (JOYSTICK_Y_MID - JOYSTICK_Y_DEADZONE));
  }
  else if (y > (JOYSTICK_Y_MID + JOYSTICK_Y_DEADZONE))
  {
    /* 正转区：越往上推速度越快，y=4095 时 +100 */
    speed = (int32_t)((y - (JOYSTICK_Y_MID + JOYSTICK_Y_DEADZONE)) * 100L
                      / (4095u - (JOYSTICK_Y_MID + JOYSTICK_Y_DEADZONE)));
  }

  return (int16_t)speed;
}

/**
 * @brief 底盘电机控制任务：读取摇杆 Y 轴，开环输出 PWM
 * @param argument 未使用
 */
void Motor_task(void *argument)
{
  (void)argument;

  Motor_Init();

  for (;;)
  {
    Motor_SetSpeed(JoystickY_To_MotorSpeed(joystick.y));
    osDelay(10); /* 100Hz 控制更新 */
  }
}