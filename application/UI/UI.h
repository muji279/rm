#ifndef UI_H
#define UI_H

/**
 * @brief OLED 显示任务（强实现，覆盖 freertos.c 中的 __weak 版本）
 * @param argument 未使用
 */
void Oled_DisplayTask(void *argument);

#endif /* UI_H */