/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for servoTask */
osThreadId_t servoTaskHandle;
const osThreadAttr_t servoTask_attributes = {
  .name = "servoTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for motorTask */
osThreadId_t motorTaskHandle;
const osThreadAttr_t motorTask_attributes = {
  .name = "motorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for oledDisplayTask */
osThreadId_t oledDisplayTaskHandle;
const osThreadAttr_t oledDisplayTask_attributes = {
  .name = "oledDisplayTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for joystick_decode */
osThreadId_t joystick_decodeHandle;
const osThreadAttr_t joystick_decode_attributes = {
  .name = "joystick_decode",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for detecttask */
osThreadId_t detecttaskHandle;
const osThreadAttr_t detecttask_attributes = {
  .name = "detecttask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void Servo_task(void *argument);
void Motor_task(void *argument);
void Oled_DisplayTask(void *argument);
void joystick_decode_Task(void *argument);
void Detect_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of servoTask */
  servoTaskHandle = osThreadNew(Servo_task, NULL, &servoTask_attributes);

  /* creation of motorTask */
  motorTaskHandle = osThreadNew(Motor_task, NULL, &motorTask_attributes);

  /* creation of oledDisplayTask */
  oledDisplayTaskHandle = osThreadNew(Oled_DisplayTask, NULL, &oledDisplayTask_attributes);

  /* creation of joystick_decode */
  joystick_decodeHandle = osThreadNew(joystick_decode_Task, NULL, &joystick_decode_attributes);

  /* creation of detecttask */
  detecttaskHandle = osThreadNew(Detect_task, NULL, &detecttask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Servo_task */
/**
* @brief Function implementing the servoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Servo_task */
__weak void Servo_task(void *argument)
{
  /* USER CODE BEGIN Servo_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Servo_task */
}

/* USER CODE BEGIN Header_Motor_task */
/**
* @brief Function implementing the motorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Motor_task */
__weak void Motor_task(void *argument)
{
  /* USER CODE BEGIN Motor_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Motor_task */
}

/* USER CODE BEGIN Header_Oled_DisplayTask */
/**
* @brief Function implementing the oledDisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Oled_DisplayTask */
__weak void Oled_DisplayTask(void *argument)
{
  /* USER CODE BEGIN Oled_DisplayTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Oled_DisplayTask */
}

/* USER CODE BEGIN Header_joystick_decode_Task */
/**
* @brief Function implementing the joystick_decode thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_joystick_decode_Task */
__weak void joystick_decode_Task(void *argument)
{
  /* USER CODE BEGIN joystick_decode_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END joystick_decode_Task */
}

/* USER CODE BEGIN Header_Detect_task */
/**
* @brief Function implementing the detecttask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Detect_task */
__weak void Detect_task(void *argument)
{
  /* USER CODE BEGIN Detect_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Detect_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

