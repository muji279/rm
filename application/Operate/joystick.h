#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

typedef struct
{
  uint16_t x;   /* X 轴原始 ADC 值：ADC1_IN1 (PA1)，约 0~4095 */
  uint16_t y;   /* Y 轴原始 ADC 值：ADC1_IN2 (PA2)，约 0~4095 */
  uint8_t  sw;  /* 摇杆按键（HW-504 SW），未接 GPIO 时恒为 0 */
  uint8_t  updated; /* 新数据标志：云台板 ADC 采样后置 1；底盘板收到 CAN 帧后置 1 */
} Joystick_t;

extern Joystick_t joystick;

/* 摇杆解码任务（强实现，覆盖 freertos.c 中 __weak 版本） */
void joystick_decode_Task(void *argument);

#endif /* JOYSTICK_H */