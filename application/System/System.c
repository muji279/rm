#include "System.h"
#include "bsp_can.h"
#include "cmsis_os.h"

/* freertos.c 中创建的线程句柄 */
extern osThreadId_t defaultTaskHandle;
extern osThreadId_t servoTaskHandle;
extern osThreadId_t motorTaskHandle;
extern osThreadId_t oledDisplayTaskHandle;
extern osThreadId_t joystick_decodeHandle;

/**
 * @brief 默认任务：开机后按板级宏裁剪任务，最后删除自己
 *
 * defaultTask 优先级最高（CubeMX 里设为 High2），调度器启动后先于其他任务运行，
 * 因此在其他任务真正干活之前就能把本板用不到的任务删掉。
 *
 * 任务分配：
 *   Servo_task            -> 云台板
 *   joystick_decode_Task  -> 云台板（摇杆在云台板，原始值通过 CAN 发给底板）
 *   Motor_task            -> 底盘板
 *   Oled_DisplayTask      -> 底盘板（显示摇杆/转速）
 *   Detect_task           -> 两块板都要
 */
void StartDefaultTask(void *argument)
{
  (void)argument;

  /* 两块板都要用 CAN，开机最先初始化（默认任务优先级最高，先于其他任务执行） */
  CAN_init();

#ifdef GIMBAL
  /* 云台板：删除电机任务和 OLED 任务 */
  osThreadTerminate(motorTaskHandle);
  osThreadTerminate(oledDisplayTaskHandle);
#elif defined(CHASSIS)
  /* 底盘板：删除舵机任务和摇杆任务（摇杆数据从 CAN 接收） */
  osThreadTerminate(servoTaskHandle);
  osThreadTerminate(joystick_decodeHandle);
#else
#error "请先在 CMakeLists.txt 中定义 GIMBAL 或 CHASSIS 宏"
#endif

  /* 删除默认任务自身，把 CPU 让给剩下的任务 */
  osThreadTerminate(defaultTaskHandle);
}