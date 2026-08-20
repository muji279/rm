#ifndef BSP_CAN_H
#define BSP_CAN_H

#include "can.h"

/* CAN 总线初始化：配置过滤器、启动 CAN1、使能接收中断通知 */
void CAN_init(void);

/* CAN1 发送失败/总线离线时调用：中止发送并重启 CAN1 */
void CAN1_Error_Recover(void);

#endif /* BSP_CAN_H */