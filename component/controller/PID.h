#ifndef PID_H
#define PID_H

#include <stdint.h>

enum
{
    LAST = 0,
    NOW  = 1,
};

/**
  * @brief PID 结构体
  */
typedef struct
{
    /* p、i、d 参数 */
    float p;
    float i;
    float d;

    /* 目标值、反馈值、误差值 */
    float set;
    float get;
    float err[2];
    float sum_err;

    /* p、i、d 各项计算出的输出 */
    float pout;
    float iout;
    float dout;

    /* PID 公式计算出的总输出 */
    float out;

    /* PID 最大输出限制 */
    uint32_t max_output;

    /* PID 积分输出项限幅 */
    uint32_t integral_limit;

} pid_type_def;

/* 初始化 PID（最大输出、积分限幅、kp/ki/kd） */
void pid_init(pid_type_def *pid, uint32_t max_out, uint32_t intergral_limit,
              float kp, float ki, float kd);

/* 复位 PID 参数与内部状态 */
void pid_reset(pid_type_def *pid, float kp, float ki, float kd);

/* 位置式 PID 计算：输入反馈值 get 与目标值 set，返回输出 */
float pid_calc(pid_type_def *pid, float get, float set);

/* 带积分分离的 PID 计算：误差超过阈值时清积分 */
float pid_calc_KI_Separation(pid_type_def *pid, float get, float set, float err_threshold);

/* 平衡车专用：微分项直接使用陀螺仪角速度 */
float pid_calc_balance(pid_type_def *pid, float get, float set, float gyro_y);

/* 环状量 PID（处理角度跨越 ±180° 等场景） */
float pid_loop_calc(pid_type_def *pid, float get, float set, float max_value, float min_value);

#endif /* PID_H */