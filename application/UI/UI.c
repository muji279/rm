#include "UI.h"
#include "bsp_oled.h"
#include "cmsis_os.h"

/**
 * @brief OLED 显示任务：初始化屏幕后循环显示 "helloworld"
 * @param argument 未使用
 */
void Oled_DisplayTask(void *argument)
{
  (void)argument;

  OLED_Init();

  for (;;)
  {
    OLED_NewFrame();
    OLED_PrintASCIIString(24, 24, "helloworld", &afont16x8, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    osDelay(100);
  }
}