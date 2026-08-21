#include "can_send.h"
#include "bsp_can.h"

/* ==================== 字节拆分联合体 ====================
 * 利用联合体把 uint16 拆成两个字节填进 CAN 数据区：
 * value 是 uint16_t，data[0]/data[1] 就是它的低/高字节（小端）。
 */
union uint16_bytes
{
  uint8_t  data[2];
  uint16_t value;
};

/**
 * @brief 通用发送：封装 CAN 标准帧头并发送
 * @param std_id 标准 ID
 * @param data   8 字节数据
 */
static void Can_Send_Frame(uint16_t std_id, uint8_t *data)
{
  CAN_TxHeaderTypeDef tx_message;
  uint32_t send_mail_box;

  tx_message.StdId = std_id;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;

  if (HAL_CAN_AddTxMessage(&hcan, &tx_message, data, &send_mail_box) != HAL_OK)
  {
    CAN1_Error_Recover();
  }
}

/* ==================== ID 1：摇杆数据 ====================
 * 两路 12 位 ADC 原始值（0~4095），每路占 2 字节（uint16 小端）：
 *   data[0..1] = X，data[2..3] = Y，data[4..7] 保留
 * 更省字节的做法是把两个 12 位拼进 3 字节（24 bit），
 * 但 2 字节/路可读性好、容错高，这里采用后者。
 */
void Send_Joystick_Data(uint16_t x, uint16_t y)
{
  uint8_t send_data[8] = {0};
  union uint16_bytes u;

  u.value = x;
  send_data[0] = u.data[0];
  send_data[1] = u.data[1];

  u.value = y;
  send_data[2] = u.data[0];
  send_data[3] = u.data[1];

  Can_Send_Frame(0x001, send_data);
}

/* ==================== ID 2：舵机数据 ====================
 * 字节布局与 can_send.h 中 Servo_Data_t 一致：
 *   data[0..1] = target_rpm_x10（int16，小端，×10 RPM）
 *   data[2]    = online
 *   data[3..7] = 保留
 */
void Send_Servo_Data(const Servo_Data_t *servo)
{
  uint8_t send_data[8] = {0};
  union uint16_bytes u;

  u.value = servo->target_rpm_x10;
  send_data[0] = u.data[0];
  send_data[1] = u.data[1];

  send_data[2] = servo->online;

  Can_Send_Frame(0x002, send_data);
}