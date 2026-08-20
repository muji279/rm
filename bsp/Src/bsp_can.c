#include "bsp_can.h"
#include "main.h"
#include "can.h"

/* 发送邮箱连续满多久触发恢复（ms） */
#define CAN1_TX_FULL_RECOVER_TIME  20U
/* 两次恢复之间的最小间隔（ms），防止频繁重启 CAN */
#define CAN1_RECOVER_INTERVAL      100U

static uint32_t can1_tx_full_start_time = 0;
static uint32_t can1_last_recover_time = 0;
volatile uint32_t can1_recover_count = 0;
volatile uint32_t can1_last_error = 0;

/**
 * @brief CAN 初始化：配置过滤器 + 启动 CAN1 + 使能接收中断
 *
 * 参考步兵工程的 CAN 过滤配置：
 * 板间通信报文使用 0x110~0x11F，统一放入 FIFO1。
 * 标准 ID 的 11 位放在过滤器寄存器的 bit[15:5]（即 FilterIdHigh 的 bit[15:5]），
 * 所以 ID 要左移 5 位；Mask 0xFF00 表示只匹配 ID 的高 8 位，
 * 即 0x110~0x11F 范围内的报文都能收到。
 */
void CAN_init(void)
{
  CAN_FilterTypeDef can_filter_st = {0};

  can_filter_st.FilterActivation = ENABLE;
  can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
  can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
  can_filter_st.FilterIdHigh = 0x0000;
  can_filter_st.FilterIdLow = 0x0000;
  can_filter_st.FilterMaskIdHigh = 0xFFFF;
  can_filter_st.FilterMaskIdLow = 0x0000;

  /* 板间通信：[0x110 ~ 0x11F] -> FIFO1 */
  can_filter_st.FilterBank = 0;
  can_filter_st.FilterIdHigh = (0x110 << 5);
  can_filter_st.FilterMaskIdHigh = 0xFF00;
  can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO1;
  HAL_CAN_ConfigFilter(&hcan, &can_filter_st);

  /* 启动 CAN1 并使能 FIFO1 接收中断（CubeMX 已开启 CAN1_RX1_IRQn） */
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}

/**
 * @brief CAN1 错误恢复：发送邮箱满或总线离线时重启 CAN1
 *
 * 逻辑参考步兵工程 bsp_can.c 的 CAN1_Error_Recover()：
 *   1. 邮箱连续满超过 20ms，或检测到 Bus Off，才需要恢复；
 *   2. 两次恢复至少间隔 100ms，避免反复重启；
 *   3. 恢复时中止所有发送请求、停止 CAN、清错误码、重新启动并使能接收中断。
 */
void CAN1_Error_Recover(void)
{
  uint32_t now = HAL_GetTick();
  uint8_t need_recover = 0;

  if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0U)
  {
    if (can1_tx_full_start_time == 0U)
    {
      can1_tx_full_start_time = now;
    }
    else if ((now - can1_tx_full_start_time) > CAN1_TX_FULL_RECOVER_TIME)
    {
      need_recover = 1;
    }
  }
  else
  {
    can1_tx_full_start_time = 0;
  }

  if ((hcan.Instance->ESR & CAN_ESR_BOFF) != 0U)
  {
    need_recover = 1;
  }

  if (!need_recover || ((now - can1_last_recover_time) < CAN1_RECOVER_INTERVAL))
  {
    return;
  }

  can1_last_recover_time = now;
  can1_last_error = HAL_CAN_GetError(&hcan);
  can1_recover_count++;

  HAL_CAN_AbortTxRequest(&hcan, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2);
  HAL_CAN_Stop(&hcan);
  hcan.ErrorCode = HAL_CAN_ERROR_NONE;
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);

  can1_tx_full_start_time = 0;
}