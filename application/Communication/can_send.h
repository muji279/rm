#ifndef CAN_SEND_H
#define CAN_SEND_H

#include "main.h"

/* ============ 板间通信发送函数（仿步兵工程 can_send.c） ============
 * 报文 ID 划分（云台板 <-> 底盘板，F103 只有 CAN1）：
 *   云台 -> 底盘：0x110 底盘速度  0x111 控制量   0x117 模式
 *   底盘 -> 云台：0x113 云台角度  0x114 裁判数据 0x115 弹速热量 0x116 发射限制
 * 具体字节布局与步兵工程保持一致，方便两板互认。
 */

/* 云台 -> 底盘：底盘期望速度与开关量 */
void Send_Chassis_Speed(int16_t ch1, int16_t ch0, int16_t ch2,
                        int32_t shift, int32_t ctrl, int32_t c, int32_t v);

/* 云台 -> 底盘：按键 W/A/S/D 与云台相对角度 */
void Send_Control(int32_t W, int32_t A, int32_t S, int32_t D, float relative_angle);

/* 云台 -> 底盘：各模块工作模式 */
void Send_Mode(int16_t chassis_mode, int16_t gimbal_mode,
               int16_t launcher_mode, int16_t auto_aim_mode);

/* 底盘 -> 云台：云台俯仰目标角度与角速度 */
void Send_pitch_down(float absolute_angle, float vw);

/* 底盘 -> 云台：裁判系统数据（缓冲能量、功率限制、机器人 ID 等） */
void Send_referee(uint16_t buffer_energy, uint16_t power_limit, uint8_t robot_id,
                  uint8_t gimbal_output, uint8_t chassis_output, uint8_t game_progress);

/* 底盘 -> 云台：弹速与枪口热量（用于摩擦轮转速补偿） */
void Send_bullet_speed(float bullet_speed, float shooter_heat0);

/* 底盘 -> 云台：发射限制（射速上限、冷却上限） */
void Send_limit_speed(uint16_t shooter_limit, uint16_t shooter_cooling);

#endif /* CAN_SEND_H */