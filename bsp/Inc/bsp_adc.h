#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "main.h"

/* 摇杆两路 ADC：X = ADC1_IN1(PA1)，Y = ADC1_IN2(PA2) */
#define JOYSTICK_ADC_CH_NUM 2

/* DMA 搬运缓冲区（circular 模式，半字） */
extern uint16_t joystick_adc_raw[JOYSTICK_ADC_CH_NUM];

/* 启动 ADC1 + DMA 循环采集 */
void BSP_ADC_Init(void);

/* 读取一次摇杆原始值（X, Y） */
void BSP_ADC_GetJoystickRaw(uint16_t *x, uint16_t *y);

#endif /* BSP_ADC_H */