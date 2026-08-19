#include "UI.h"
#include "bsp_oled.h"
#include "joystick.h"
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

/**
 * @brief OLED 显示任务：显示摇杆 X/Y 原始值
 * @param argument 未使用
 */
void Oled_DisplayTask(void *argument)
{
  (void)argument;
  char num_buf[5];

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

    /* 角落自检标记：能看到说明屏幕和驱动正常 */
    OLED_PrintASCIIString(0, 56, "hello", &afont8x6, OLED_COLOR_NORMAL);

    OLED_ShowFrame();
    osDelay(100);
  }
}