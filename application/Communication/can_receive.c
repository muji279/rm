#include "can_receive.h"
#include "Detection.h"
#include "can.h"

Can_RawFrame_t can_rx_frame = {0};

/**
 * @brief FIFO1 接收回调：板间通信报文（0x110~0x11F）走这里
 *
 * 由 stm32f1xx_it.c 的 CAN1_RX1_IRQHandler -> HAL_CAN_IRQHandler 触发，
 * 不需要自己再写中断函数（否则会和 CubeMX 生成的中断重复定义）。
 *
 * TODO: 下一步写预编译（GIMBAL/CHASSIS）时，在这里按板子分发：
 *   #ifdef GIMBAL  -> Chassis_to_Gimbal_Can(std_id, data)
 *   #ifdef CHASSIS -> Gimbal_to_Chassis_Can(std_id, data)
 */
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan_ptr)
{
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];

  if (hcan_ptr->Instance != CAN1)
  {
    return;
  }

  HAL_CAN_GetRxMessage(hcan_ptr, CAN_RX_FIFO1, &rx_header, rx_data);

  can_rx_frame.std_id = rx_header.StdId;
  can_rx_frame.updated = 1;
  for (uint8_t i = 0; i < 8; i++)
  {
    can_rx_frame.data[i] = rx_data[i];
  }

  /* 收到板间通信报文 -> 上报 CAN 通信心跳 */
  detect_handle(DETECT_CAN_COMM);
}

/**
 * @brief FIFO0 接收回调：预留，后续接 CAN 电机/裁判系统时使用
 *
 * 当前 CubeMX 只使能了 CAN1_RX1_IRQn，FIFO0 的报文暂时收不到中断；
 * 以后用到 FIFO0 时，需要在 CubeMX 里勾选 CAN1_RX0_IRQn，并在
 * bsp_can.c 的 CAN_init() 中激活 CAN_IT_RX_FIFO0_MSG_PENDING。
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_ptr)
{
  CAN_RxHeaderTypeDef rx_header;
  uint8_t rx_data[8];

  if (hcan_ptr->Instance != CAN1)
  {
    return;
  }

  HAL_CAN_GetRxMessage(hcan_ptr, CAN_RX_FIFO0, &rx_header, rx_data);

  /* TODO: 电机反馈 / 裁判系统数据解析 */
  (void)rx_header;
  (void)rx_data;
}