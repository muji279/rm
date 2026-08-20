#ifndef CAN_RECEIVE_H
#define CAN_RECEIVE_H

#include "main.h"

/* 最近一次收到的 CAN 原始帧（FIFO1，板间通信） */
typedef struct
{
  uint32_t std_id;   /* 标准 ID */
  uint8_t  data[8];  /* 8 字节数据 */
  uint8_t  updated;  /* 是否有新数据：接收中断里置 1，应用层解析后清零 */
} Can_RawFrame_t;

extern Can_RawFrame_t can_rx_frame;

/* HAL 接收回调（由 CAN1_RX0/CAN1_RX1_IRQHandler -> HAL_CAN_IRQHandler 调用） */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif /* CAN_RECEIVE_H */