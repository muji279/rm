#ifndef BSP_LED_H
#define BSP_LED_H

#include "main.h"

/* 呼吸灯周期（ms），一个完整“渐亮->渐灭”循环 */
#define LED_BREATH_PERIOD_MS   2000u

/* 初始化 LED：启动 TIM2_CH3(PB10) PWM 输出并熄灭 */
void LED_Init(void);

/* 熄灭 LED */
void LED_Off(void);

/* 直接设置 LED 亮度（PWM 占空比，0~TIM2 ARR=999） */
void LED_SetBrightness(uint16_t pwm);

/* 呼吸灯刷新：按时间推进亮度（0 -> 最亮 -> 0），
 * 需要周期调用，调用频率越高呼吸越平滑 */
void LED_Breathing_Update(void);

#endif /* BSP_LED_H */