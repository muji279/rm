#include "joystick.h"
#include "bsp_adc.h"
#include "can_send.h"
#include "cmsis_os.h"

/* 摇杆原始值结构体，供显示任务等其他模块读取 */
Joystick_t joystick = {0};

/**
 * @brief 摇杆解码任务：循环读取 ADC 原始值存入结构体
 * @param argument 未使用
 */
void joystick_decode_Task(void *argument)
{
  (void)argument;

  /* 启动 ADC1 + DMA 循环采集（PA1/PA2） */
  BSP_ADC_Init();

  for (;;)
  {
    BSP_ADC_GetJoystickRaw(&joystick.x, &joystick.y);
    joystick.updated = 1;

    /* 摇杆在云台板，原始值通过 CAN（ID 1）发给底盘板 */
    Send_Joystick_Data(joystick.x, joystick.y);
    /* sw 按键：将 HW-504 的 SW 引脚接到任意 GPIO 后，用 HAL_GPIO_ReadPin 读取即可 */

    osDelay(20); /* 50Hz 采样，可按需调整 */
  }
}