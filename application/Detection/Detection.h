#ifndef DETECTION_H
#define DETECTION_H

#include "main.h"
#include "cmsis_os.h"
#include "bsp_led.h"

/* 设备在线状态 */
#define ONLINE   1
#define OFFLINE  0

/* 检测设备结构体（仿步兵工程 Detection.c） */
typedef struct
{
  uint8_t  enable;            /* 是否参与检测 */
  uint32_t last_online_time;  /* 最近一次“心跳”时间戳（ms） */
  uint8_t  status;            /* ONLINE / OFFLINE */
  uint32_t offline_threshold; /* 超过该时长没有心跳就判定离线（ms） */
} detect_device_t;

/* 设备索引：先统一列出，预编译（GIMBAL/CHASSIS）时再按板子裁剪 */
typedef enum
{
  DETECT_CAN_COMM = 0,        /* 板间 CAN 通信（收到任一 CAN 报文即刷新心跳） */
  DETECT_MOTOR,               /* 底盘电机反馈（编码器） */
  DETECT_DEVICE_LIST_LEN
} detect_device_index;

extern detect_device_t detect_list[DETECT_DEVICE_LIST_LEN];

/* 设备心跳上报：设备正常工作时周期性调用，刷新在线时间戳 */
void detect_handle(uint8_t index);

/* 检测任务（强实现，覆盖 freertos.c 中的 __weak 版本） */
void Detect_task(void *argument);

#endif /* DETECTION_H */