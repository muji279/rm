#ifndef SYSTEM_H
#define SYSTEM_H

/* 默认任务（强实现，覆盖 freertos.c 中的 __weak 版本）：
 * 开机后按板级宏裁剪不必要的任务，最后删除自己 */
void StartDefaultTask(void *argument);

#endif /* SYSTEM_H */