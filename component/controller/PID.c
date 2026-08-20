#include <math.h>
#include "PID.h"

static void abs_limit(float *a, float ABS_MAX)
{
    if (*a > ABS_MAX)
        *a = ABS_MAX;
    if (*a < -ABS_MAX)
        *a = -ABS_MAX;
}

/**
  * @brief PID 初始化函数
  * @param[in] pid: PID 结构体
  * @param[in] max_out: 最大输出
  * @param[in] intergral_limit: 积分限幅
  * @param[in] kp/ki/kd: 具体 PID 参数
  */
void pid_init(pid_type_def *pid, uint32_t max_out, uint32_t intergral_limit,
              float kp, float ki, float kd)
{
    pid->integral_limit = intergral_limit;
    pid->max_output     = max_out;

    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

/**
  * @brief PID 计算函数，使用位置式 PID 计算
  * @param[in] pid: PID 结构体
  * @param[in] get: 反馈数据
  * @param[in] set: 目标数据
  * @retval    PID 计算输出
  */
float pid_calc(pid_type_def *pid, float get, float set)
{
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;

    pid->pout = pid->p * pid->err[NOW];
    pid->iout += pid->i * pid->err[NOW];
    pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST]);

    abs_limit(&(pid->iout), pid->integral_limit);
    pid->out = pid->pout + pid->iout + pid->dout;
    abs_limit(&(pid->out), pid->max_output);

    pid->err[LAST] = pid->err[NOW];

    return pid->out;
}

/**
  * @brief 环状量 PID（角度环跨零点等场景）
  * @param[in] pid: PID 结构体
  * @param[in] get: 反馈数据
  * @param[in] set: 目标数据
  * @param[in] max_value/min_value: 环状量上下限
  * @retval    PID 计算输出
  */
float pid_loop_calc(pid_type_def *pid, float get, float set, float max_value, float min_value)
{
    float gap, mid;
    mid = (max_value - min_value) / 2;
    gap = set - get;
    if (gap >= mid)
    {
        gap -= max_value - min_value;
        return pid_calc(pid, -gap, 0);
    }
    else if (gap <= -mid)
    {
        gap += max_value - min_value;
        return pid_calc(pid, -gap, 0);
    }
    else
    {
        return pid_calc(pid, get, set);
    }
}

/**
  * @brief PID 参数复位函数
  * @param[in] pid: PID 结构体
  * @param[in] kp/ki/kd: 具体 PID 参数
  */
void pid_reset(pid_type_def *pid, float kp, float ki, float kd)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;

    pid->pout = 0;
    pid->iout = 0;
    pid->dout = 0;
    pid->out  = 0;
}

/**
  * @brief 带积分分离的 PID 计算
  * @param[in] pid: PID 结构体
  * @param[in] get: 反馈数据
  * @param[in] set: 目标数据
  * @param[in] err_threshold: 积分分离阈值
  * @retval    PID 计算输出
  */
float pid_calc_KI_Separation(pid_type_def *pid, float get, float set, float err_threshold)
{
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;

    pid->pout = pid->p * pid->err[NOW];

    if (fabs(pid->err[NOW]) > err_threshold)
    {
        pid->iout = 0;
    }
    else
    {
        pid->iout += pid->i * pid->err[NOW];
    }

    pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST]);
    abs_limit(&(pid->iout), pid->integral_limit);
    pid->out = pid->pout + pid->iout + pid->dout;
    abs_limit(&(pid->out), pid->max_output);

    pid->err[LAST] = pid->err[NOW];

    return pid->out;
}

/**
  * @brief 平衡车专用 PID：微分项直接使用陀螺仪角速度
  */
float pid_calc_balance(pid_type_def *pid, float get, float set, float gyro_y)
{
    pid->get = get;
    pid->set = set;
    pid->err[NOW] = set - get;

    pid->pout = pid->p * pid->err[NOW];
    pid->iout += pid->i * pid->err[NOW];
    pid->dout = pid->d * gyro_y;

    abs_limit(&(pid->iout), pid->integral_limit);
    pid->out = pid->pout + pid->iout + pid->dout;
    abs_limit(&(pid->out), pid->max_output);

    pid->err[LAST] = pid->err[NOW];

    return pid->out;
}