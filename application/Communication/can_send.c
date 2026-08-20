#include "can_send.h"
#include "bsp_can.h"

/* ==================== 字节拆分联合体 ====================
 * 利用联合体让 int16/float 的各个字节可以直接取出来填进 CAN 数据区。
 * 例如 union ctrl_ch：value 是 int16_t，data[0]/data[1] 就是它的低/高字节。
 * 注意：小端模式下 data[0] 是低字节，收发两端都用同一套代码，约定一致即可。
 */
union ctrl_ch
{
  uint8_t data[2];
  int16_t value;
};

union angle
{
  uint8_t data[4];
  float value;
};

union mode
{
  uint8_t data[2];
  int16_t mode;
};

union referee
{
  uint8_t data[4];
  float referee;
};

union chassis_referee
{
  uint8_t data[2];
  uint16_t referee;
};

union vel
{
  uint8_t data[4];
  float value;
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

/* ==================== 云台 -> 底盘 ==================== */

/* 0x110：底盘速度（3 个 int16 通道 + 档位 + 开关量） */
void Send_Chassis_Speed(int16_t ch1, int16_t ch0, int16_t ch2,
                        int32_t shift, int32_t ctrl, int32_t c, int32_t v)
{
  uint8_t send_data[8] = {0};
  union ctrl_ch temp;

  temp.value = ch1;
  send_data[0] = temp.data[0];
  send_data[1] = temp.data[1];

  temp.value = ch0;
  send_data[2] = temp.data[0];
  send_data[3] = temp.data[1];

  temp.value = ch2;
  send_data[4] = temp.data[0];
  send_data[5] = temp.data[1];

  send_data[6] = (uint8_t)(shift & 0xFF);
  send_data[7] = (uint8_t)(((ctrl & 0x03) << 0) |
                           ((c    & 0x03) << 2) |
                           ((v    & 0x03) << 4));

  Can_Send_Frame(0x110, send_data);
}

/* 0x111：按键 W/A/S/D（各 1 字节）+ 相对角度（float 4 字节） */
void Send_Control(int32_t W, int32_t A, int32_t S, int32_t D, float relative_angle)
{
  uint8_t send_data[8] = {0};
  union angle temp;

  send_data[0] = (uint8_t)(W & 0xFF);
  send_data[1] = (uint8_t)(A & 0xFF);
  send_data[2] = (uint8_t)(S & 0xFF);
  send_data[3] = (uint8_t)(D & 0xFF);

  temp.value = relative_angle;
  send_data[4] = temp.data[0];
  send_data[5] = temp.data[1];
  send_data[6] = temp.data[2];
  send_data[7] = temp.data[3];

  Can_Send_Frame(0x111, send_data);
}

/* 0x117：各模块模式（每个 int16 占 2 字节） */
void Send_Mode(int16_t chassis_mode, int16_t gimbal_mode,
               int16_t launcher_mode, int16_t auto_aim_mode)
{
  uint8_t send_data[8] = {0};
  union mode chassis;
  union mode gimbal;
  union mode launcher;
  union mode auto_aim;

  chassis.mode = chassis_mode;
  launcher.mode = launcher_mode;
  gimbal.mode = gimbal_mode;
  auto_aim.mode = auto_aim_mode;

  send_data[0] = chassis.data[0];
  send_data[1] = chassis.data[1];
  send_data[2] = launcher.data[0];
  send_data[3] = launcher.data[1];
  send_data[4] = gimbal.data[0];
  send_data[5] = gimbal.data[1];
  send_data[6] = auto_aim.data[0];
  send_data[7] = auto_aim.data[1];

  Can_Send_Frame(0x117, send_data);
}

/* ==================== 底盘 -> 云台 ==================== */

/* 0x113：云台俯仰目标角度（float）+ 目标角速度（float） */
void Send_pitch_down(float absolute_angle, float vw)
{
  uint8_t send_data[8] = {0};
  union angle angle;
  union vel vel;

  angle.value = absolute_angle;
  send_data[0] = angle.data[0];
  send_data[1] = angle.data[1];
  send_data[2] = angle.data[2];
  send_data[3] = angle.data[3];

  vel.value = vw;
  send_data[4] = vel.data[0];
  send_data[5] = vel.data[1];
  send_data[6] = vel.data[2];
  send_data[7] = vel.data[3];

  Can_Send_Frame(0x113, send_data);
}

/* 0x114：裁判数据（能量 buffer、功率限制、机器人 ID、输出开关、比赛进度） */
void Send_referee(uint16_t buffer_energy, uint16_t power_limit, uint8_t robot_id,
                  uint8_t gimbal_output, uint8_t chassis_output, uint8_t game_progress)
{
  uint8_t send_data[8] = {0};
  union chassis_referee temp;
  union chassis_referee limit;

  temp.referee = buffer_energy;
  send_data[0] = temp.data[0];
  send_data[1] = temp.data[1];

  limit.referee = power_limit;
  send_data[2] = limit.data[0];
  send_data[3] = limit.data[1];

  send_data[4] = robot_id;
  send_data[5] = gimbal_output & 0x01;
  send_data[6] = chassis_output & 0x01;
  send_data[7] = game_progress & 0x0F;

  Can_Send_Frame(0x114, send_data);
}

/* 0x115：弹速（float）+ 枪口热量（float），用于摩擦轮转速补偿 */
void Send_bullet_speed(float bullet_speed, float shooter_heat0)
{
  uint8_t send_data[8] = {0};
  union referee temp1;
  union referee temp2;

  temp1.referee = bullet_speed;
  send_data[0] = temp1.data[0];
  send_data[1] = temp1.data[1];
  send_data[2] = temp1.data[2];
  send_data[3] = temp1.data[3];

  temp2.referee = shooter_heat0;
  send_data[4] = temp2.data[0];
  send_data[5] = temp2.data[1];
  send_data[6] = temp2.data[2];
  send_data[7] = temp2.data[3];

  Can_Send_Frame(0x115, send_data);
}

/* 0x116：发射限制（射速上限、冷却上限，各 uint16 占 2 字节） */
void Send_limit_speed(uint16_t shooter_limit, uint16_t shooter_cooling)
{
  uint8_t send_data[8] = {0};
  union chassis_referee temp1;
  union chassis_referee temp2;

  temp1.referee = shooter_limit;
  send_data[0] = temp1.data[0];
  send_data[1] = temp1.data[1];

  temp2.referee = shooter_cooling;
  send_data[2] = temp2.data[0];
  send_data[3] = temp2.data[1];

  Can_Send_Frame(0x116, send_data);
}