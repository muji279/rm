#include "UI.h"
#include "bsp_oled.h"
#include "joystick.h"
#include "Chassis.h"
#include "can_receive.h"
#include "Detection.h"
#include "cmsis_os.h"

/* 将 0~4095 转成 4 位数字字符串（零填充），buf 至少 5 字节 */
static void uint16_to_str(uint16_t val, char *buf)
{
  buf[4] = '\0';
  for (int i = 3; i >= 0; i--)
  {
    buf[i] = (char)('0' + (val % 10));
    val /= 10;
  }
}

/* 带符号整数 -> 字符串（支持负数），buf 至少 8 字节 */
static void int16_to_str(int16_t val, char *buf)
{
  char tmp[6];
  int  i = 0;
  int  j = 0;
  int  neg = (val < 0) ? 1 : 0;

  if (neg)
  {
    val = (int16_t)(-val);
  }

  do
  {
    tmp[i++] = (char)('0' + (val % 10));
    val /= 10;
  } while (val > 0);

  if (neg)
  {
    tmp[i++] = '-';
  }
  while (i > 0)
  {
    buf[j++] = tmp[--i];
  }
  buf[j] = '\0';
}

/* 浮点 RPM -> 四舍五入后的整数字符串 */
static void rpm_to_str(float rpm, char *buf)
{
  int16_t v = (rpm >= 0.0f) ? (int16_t)(rpm + 0.5f) : (int16_t)(rpm - 0.5f);
  int16_to_str(v, buf);
}

/* RPM×10（如 556 表示 55.6 RPM）-> 一位小数字符串，buf 至少 8 字节 */
static void rpm_x10_to_str(int16_t v10, char *buf)
{
  int16_t whole = v10 / 10;
  int16_t frac  = (int16_t)(v10 % 10);
  char tmp[8];
  int  i = 0;
  int  j = 0;
  int  neg = (whole < 0) ? 1 : 0;

  if (neg)
  {
    whole = (int16_t)(-whole);
  }
  if (frac < 0)
  {
    frac = (int16_t)(-frac);
  }

  do
  {
    tmp[i++] = (char)('0' + (whole % 10));
    whole /= 10;
  } while (whole > 0);

  if (neg)
  {
    tmp[i++] = '-';
  }
  while (i > 0)
  {
    buf[j++] = tmp[--i];
  }
  buf[j++] = '.';
  buf[j++] = (char)('0' + frac);
  buf[j] = '\0';
}

/**
 * @brief OLED 显示任务（底盘板）：摇杆 X/Y + 电机转速 + 舵机数据 + CAN 通信状态
 *
 * 布局（128x64）：
 *   第 1 行 16px: X: xxxx（CAN 收到的云台板摇杆原始值）
 *   第 2 行 16px: Y: yyyy
 *   第 3 行  8px: MT: 电机目标转速（RPM）
 *   第 4 行  8px: MA: 电机实际转速（RPM）
 *   第 5 行  8px: SV: 舵机目标转速（RPM，0.1 精度）+ On/Off
 *   第 6 行  8px: CAN: 板间通信 On/Off
 */
void Oled_DisplayTask(void *argument)
{
  (void)argument;
  char num_buf[8];

  OLED_Init();

  for (;;)
  {
    OLED_NewFrame();

    /* 第 1/2 行：CAN 收到的摇杆 X/Y（16px 大字体） */
    OLED_PrintASCIIString(0, 0, "X:", &afont16x8, OLED_COLOR_NORMAL);
    uint16_to_str(joystick.x, num_buf);
    OLED_PrintASCIIString(24, 0, num_buf, &afont16x8, OLED_COLOR_NORMAL);

    OLED_PrintASCIIString(0, 16, "Y:", &afont16x8, OLED_COLOR_NORMAL);
    uint16_to_str(joystick.y, num_buf);
    OLED_PrintASCIIString(24, 16, num_buf, &afont16x8, OLED_COLOR_NORMAL);

    /* 第 3 行：电机目标转速（输出轴 RPM） */
    OLED_PrintASCIIString(0, 32, "MT:", &afont8x6, OLED_COLOR_NORMAL);
    rpm_to_str(motor_target_rpm, num_buf);
    OLED_PrintASCIIString(18, 32, num_buf, &afont8x6, OLED_COLOR_NORMAL);

    /* 第 4 行：电机实际转速（输出轴 RPM） */
    OLED_PrintASCIIString(0, 40, "MA:", &afont8x6, OLED_COLOR_NORMAL);
    rpm_to_str(motor_actual_rpm, num_buf);
    OLED_PrintASCIIString(18, 40, num_buf, &afont8x6, OLED_COLOR_NORMAL);

    /* 第 5 行：舵机目标转速（RPM，0.1 精度）+ 在线状态 */
    OLED_PrintASCIIString(0, 48, "SV:", &afont8x6, OLED_COLOR_NORMAL);
    rpm_x10_to_str(can_servo_data.target_rpm_x10, num_buf);
    OLED_PrintASCIIString(18, 48, num_buf, &afont8x6, OLED_COLOR_NORMAL);
    OLED_PrintASCIIString(48, 48, (can_servo_data.online ? "On" : "Off"), &afont8x6, OLED_COLOR_NORMAL);

    /* 第 6 行：板间 CAN 通信状态 */
    OLED_PrintASCIIString(0, 56, "CAN:", &afont8x6, OLED_COLOR_NORMAL);
    OLED_PrintASCIIString(30, 56, (detect_list[DETECT_CAN_COMM].status == ONLINE ? "On" : "Off"), &afont8x6, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
    osDelay(10);
  }
}