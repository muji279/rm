#ifndef BSP_SERVO_PWM_H
#define BSP_SERVO_PWM_H

#include "main.h"

/* S20F-360° 连续旋转舵机 PWM 规格（50Hz，周期 20ms，1 tick = 1us） */
#define SERVO_PULSE_MIN        1000u   /* 全速顺时针 */
#define SERVO_PULSE_MAX        2000u   /* 全速逆时针 */
#define SERVO_PULSE_DEAD_MIN   1400u   /* 静止死区下限 */
#define SERVO_PULSE_DEAD_MAX   1600u   /* 静止死区上限 */
#define SERVO_PULSE_STOP       1500u   /* 死区中点，安全停止值 */

/* 启动 TIM1_CH1 PWM 输出，并让舵机停在安全位置 */
void Servo_Init(void);

/* 设置舵机脉宽（自动限幅 1000~2000us） */
void Servo_SetPulse(uint16_t pulse_us);

#endif /* BSP_SERVO_PWM_H */