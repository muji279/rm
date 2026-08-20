#ifndef BSP_MOTOR_H
#define BSP_MOTOR_H

#include "main.h"

/* TB6612 方向引脚：IN1/IN2（CubeMX 已配置为 PA4/PA5 推挽输出） */
#define MOTOR_IN1_GPIO_PORT   GPIOA
#define MOTOR_IN1_PIN         GPIO_PIN_4
#define MOTOR_IN2_GPIO_PORT   GPIOA
#define MOTOR_IN2_PIN         GPIO_PIN_5

/* 电机转速范围：±100（-100 全速反转，0 停止，+100 全速正转） */
#define MOTOR_SPEED_MIN       (-100)
#define MOTOR_SPEED_MAX       (100)

/* 初始化方向引脚并启动 TIM2_CH1(PA0) PWM 输出 */
void Motor_Init(void);

/* 设置电机转速：-100~+100
 * 正转：IN1=1, IN2=0；反转：IN1=0, IN2=1；PWM 占空比 = |speed|/100 */
void Motor_SetSpeed(int16_t speed);

/* 刹车停止（IN1=IN2=1，PWM=0） */
void Motor_Stop(void);

#endif /* BSP_MOTOR_H */