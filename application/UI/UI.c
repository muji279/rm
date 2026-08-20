#include "UI.h"
#include "bsp_oled.h"
#include "joystick.h"
#include "Chassis.h"
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

/**
 * @brief OLED 显示任务：摇杆 X/Y + 目标/实际转速
 * @param argument 未使用
 */
void Oled_DisplayTask(void *argument)
{
  (void)argument;
  char num_buf[8];

  OLED_Init();

  for (;;)
  {
    OLED_NewFrame();

    OLED_PrintASCIIString(0, 0, "X:", &afont16x8, OLED_COLOR_NORMAL);
    uint16_to_str(joystick.x, num_buf);
    OLED_PrintASCIIString(24, 0, num_buf, &afont16x8, OLED_COLOR_NORMAL);

    OLED_PrintASCIIString(0, 16, "Y:", &afont16x8, OLED_COLOR_NORMAL);
    uint16_to_str(joystick.y, num_buf);
    OLED_PrintASCIIString(24, 16, num_buf, &afont16x8, OLED_COLOR_NORMAL);

    /* 目标转速（输出轴 RPM） */
    OLED_PrintASCIIString(0, 32, "T:", &afont8x6, OLED_COLOR_NORMAL);
    rpm_to_str(motor_target_rpm, num_buf);
    OLED_PrintASCIIString(12, 32, num_buf, &afont8x6, OLED_COLOR_NORMAL);

    /* 实际转速（输出轴 RPM） */
    OLED_PrintASCIIString(0, 40, "A:", &afont8x6, OLED_COLOR_NORMAL);
    rpm_to_str(motor_actual_rpm, num_buf);
    OLED_PrintASCIIString(12, 40, num_buf, &afont8x6, OLED_COLOR_NORMAL);

    /* 角落自检标记：能看到说明屏幕和驱动正常 */
    OLED_PrintASCIIString(0, 56, "hello", &afont8x6, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
    osDelay(10);
  }
}