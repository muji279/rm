#include "bsp_led.h"
#include "tim.h"

void LED_Init(void)
{
  /* TIM2_CH3 = PB10，CubeMX 已配置为 PWM 输出（PSC=71, ARR=999, 1kHz） */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  LED_Off();
}

void LED_Off(void)
{
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 0);
}

void LED_SetBrightness(uint16_t pwm)
{
  uint16_t max_pwm = (uint16_t)htim2.Init.Period; /* ARR=999 */

  if (pwm > max_pwm)
  {
    pwm = max_pwm;
  }
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pwm);
}

void LED_Breathing_Update(void)
{
  /* 三角波呼吸：前 1s 从 0 渐亮到最亮，后 1s 渐灭回 0。
   * 用 HAL_GetTick() 取时间，因此调用频率不影响呼吸快慢。
   * 如果 LED 是低电平点亮（共阳接法），把亮度取反：
   * pwm = max_pwm - pwm 即可。 */
  uint32_t max_pwm = htim2.Init.Period;
  uint32_t t    = HAL_GetTick() % LED_BREATH_PERIOD_MS;
  uint32_t half = LED_BREATH_PERIOD_MS / 2u;
  uint32_t pwm;

  if (t < half)
  {
    pwm = max_pwm * t / half;                              /* 渐亮 */
  }
  else
  {
    pwm = max_pwm * (LED_BREATH_PERIOD_MS - t) / half;     /* 渐灭 */
  }

  LED_SetBrightness((uint16_t)pwm);
}