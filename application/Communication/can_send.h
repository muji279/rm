#ifndef CAN_SEND_H
#define CAN_SEND_H

#include "main.h"

/* ============ 自定义板间通信协议 ============
 * 标准帧、数据帧、DLC=8，两板使用同一份代码，字节序约定一致（小端）。
 *   ID 1（0x001）：摇杆数据，载荷 = X/Y 两路 12 位 ADC 原始值
 *   ID 2（0x002）：舵机数据，载荷 = 在线标志 + 目标转速
 */

/* ID 2 载荷：舵机数据结构体（逻辑上的“该舵机的所有数据”）
 * 字节布局：
 *   target_speed [0..1]  int16，目标转速：0=停止，±1000=最大，正=逆时针，负=顺时针
 *   online       [2]     uint8，1=在线，0=离线
 *   reserved     [3..7]  预留（扩展用）
 */
typedef struct
{
  int16_t target_speed;
  uint8_t online;
  uint8_t reserved[5];
} Servo_Data_t;

/* ID 1：发送摇杆数据（X/Y 为 ADC 原始值，0~4095） */
void Send_Joystick_Data(uint16_t x, uint16_t y);

/* ID 2：发送舵机数据（在线标志 + 目标转速） */
void Send_Servo_Data(const Servo_Data_t *servo);

#endif /* CAN_SEND_H */