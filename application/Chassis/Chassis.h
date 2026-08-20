#ifndef CHASSIS_H
#define CHASSIS_H

/* 底盘应用层：提供 Motor_task 强实现，覆盖 freertos.c 中的 __weak 版本 */

/* 速度环目标/实际转速（RPM），供 OLED 等任务读取 */
extern float motor_target_rpm;
extern float motor_actual_rpm;

#endif /* CHASSIS_H */