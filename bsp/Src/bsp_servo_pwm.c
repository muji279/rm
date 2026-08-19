#include "bsp_servo_pwm.h"
#include "tim.h"

void Servo_Init(void)
{
  /* TIM1 是高级定时器，HAL_TIM_PWM_Start 会同时打开主输出使能(MOE) */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

  /* 上电先给停止脉宽，防止舵机乱转 */
  Servo_SetPulse(SERVO_PULSE_STOP);
}

void Servo_SetPulse(uint16_t pulse_us)
{
  /* 限幅，匹配 S20F-360° 舵机规格 */
  if (pulse_us < SERVO_PULSE_MIN)
  {
    pulse_us = SERVO_PULSE_MIN;
  }
  if (pulse_us > SERVO_PULSE_MAX)
  {
    pulse_us = SERVO_PULSE_MAX;
  }

  /* PSC=71, ARR=19999：1 tick = 1us，CCR 直接填高电平微秒数 */
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pulse_us);
}