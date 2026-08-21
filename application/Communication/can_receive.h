#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H

#include "main.h"
#include "can_send.h"

/* 最近一次收到的 CAN 原始帧（FIFO1） */
typedef struct
{
  uint32_t std_id;   /* 标准 ID */
  uint8_t  data[8];  /* 8 字节数据 */
  uint8_t  updated;  /* 是否有新数据：接收中断里置 1，应用层解析后清零 */
} Can_RawFrame_t;

/* ID 2 解析结果：舵机数据 */
typedef struct
{
  uint8_t  online;       /* 1=在线，0=离线 */
  int16_t  target_speed; /* 目标转速（0=停止，±1000=最大） */
  uint8_t  updated;      /* 新数据标志 */
} Can_Servo_t;

extern Can_RawFrame_t can_rx_frame;
extern Can_Servo_t    can_servo_data;

/* HAL 接收回调（由 CAN1_RX0/CAN1_RX1_IRQHandler -> HAL_CAN_IRQHandler 调用） */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif /* CAN_RECEIVE_H */