#include "Detection.h"
#include "bsp_led.h"

/* 上电后等待系统稳定再开始检测（ms） */
#define DETECT_TASK_INIT_TIME  800u
/* 巡检周期（ms） */
#define DETECT_LOOP_MS         40u

detect_device_t detect_list[DETECT_DEVICE_LIST_LEN];

/**
 * @brief 初始化一个检测设备
 * @param index          设备索引
 * @param threshold_time 离线判定阈值（ms）
 */
static void detect_init(uint8_t index, uint32_t threshold_time)
{
  detect_list[index].enable = 1;
  detect_list[index].status = OFFLINE;              /* 默认先离线，收到第一次心跳才在线 */
  detect_list[index].last_online_time = HAL_GetTick();
  detect_list[index].offline_threshold = threshold_time;
}

/**
 * @brief 设备心跳上报：数据源正常时调用（例如收到 CAN 报文、读到编码器计数）
 * @param index 设备索引
 */
void detect_handle(uint8_t index)
{
  if (index < DETECT_DEVICE_LIST_LEN)
  {
    detect_list[index].last_online_time = HAL_GetTick();
  }
}

/**
 * @brief 系统状态检测任务
 *
 * 周期巡检所有使能的设备，超过阈值没有心跳就判离线；
 * 有设备离线时让 LED 呈呼吸灯效果，全部在线则熄灭。
 *
 * TODO: 预编译（GIMBAL/CHASSIS）时按板子裁剪设备：
 *   云台板只使能 DETECT_CAN_COMM，底盘板只使能 DETECT_MOTOR
 */
void Detect_task(void *argument)
{
  (void)argument;

  LED_Init();
  osDelay(DETECT_TASK_INIT_TIME);

  /* 先清空设备表 */
  for (uint8_t i = 0; i < DETECT_DEVICE_LIST_LEN; i++)
  {
    detect_list[i].enable = 0;
    detect_list[i].status = ONLINE;
    detect_list[i].last_online_time = HAL_GetTick();
    detect_list[i].offline_threshold = 0;
  }

  /* 暂时两块板都使能：CAN 通信 500ms、电机反馈 500ms 判离线 */
  detect_init(DETECT_CAN_COMM, 500u);
  detect_init(DETECT_MOTOR, 500u);

  for (;;)
  {
    uint8_t offline_num = 0;

    for (uint8_t i = 0; i < DETECT_DEVICE_LIST_LEN; i++)
    {
      if (!detect_list[i].enable)
      {
        detect_list[i].status = ONLINE;
        continue;
      }

      if ((HAL_GetTick() - detect_list[i].last_online_time) > detect_list[i].offline_threshold)
      {
        detect_list[i].status = OFFLINE;
        offline_num++;
      }
      else
      {
        detect_list[i].status = ONLINE;
      }
    }

    if (offline_num > 0)
    {
      LED_Breathing_Update();   /* 有设备异常：LED 呼吸 */
    }
    else
    {
      LED_Off();                /* 全部正常：LED 熄灭 */
    }

    osDelay(DETECT_LOOP_MS);
  }
}