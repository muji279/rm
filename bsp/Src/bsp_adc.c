#include "bsp_adc.h"
#include "adc.h"

/* DMA 会自动把 ADC1 两路转换结果循环搬运到这里 */
uint16_t joystick_adc_raw[JOYSTICK_ADC_CH_NUM] = {0};

void BSP_ADC_Init(void)
{
  /* circular 模式下 DMA 持续搬运，无需重复启动 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)joystick_adc_raw, JOYSTICK_ADC_CH_NUM);

  /* 关键修复：ADC 连续采集 + circular DMA 时，TC/HT 中断会以微秒级频率狂发，
     把 SysTick/PendSV（最低优先级）饿死，导致 RTOS 调度冻结、其他任务全部停摆。
     这里只需要 DMA 默默搬运数据，任务定时读缓冲区即可，因此关掉 DMA 中断。 */
  __HAL_DMA_DISABLE_IT(hadc1.DMA_Handle, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE);

  /* 清除残留的 DMA 标志，避免还挂着一次 pending 中断 */
  __HAL_DMA_CLEAR_FLAG(hadc1.DMA_Handle,
                       __HAL_DMA_GET_TC_FLAG_INDEX(hadc1.DMA_Handle) |
                       __HAL_DMA_GET_HT_FLAG_INDEX(hadc1.DMA_Handle) |
                       __HAL_DMA_GET_TE_FLAG_INDEX(hadc1.DMA_Handle));
}

void BSP_ADC_GetJoystickRaw(uint16_t *x, uint16_t *y)
{
  *x = joystick_adc_raw[0];
  *y = joystick_adc_raw[1];
}