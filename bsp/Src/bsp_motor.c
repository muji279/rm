#include "bsp_motor.h"
#include "tim.h"

void Motor_Init(void)
{
  GPIO_InitTypeDef gpio_init = {0};

  /* TB6612 方向引脚配置为推挽输出 */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  gpio_init.Pin   = MOTOR_IN1_PIN | MOTOR_IN2_PIN;
  gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &gpio_init);

  /* 初始状态：惰转停止 */
  HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

  /* 启动 TIM2_CH1 PWM 输出 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void Motor_SetSpeed(int16_t speed)
{
  uint32_t abs_speed;
  uint32_t ccr;

  /* 限幅到 ±100 */
  if (speed > MOTOR_SPEED_MAX)
  {
    speed = MOTOR_SPEED_MAX;
  }
  if (speed < MOTOR_SPEED_MIN)
  {
    speed = MOTOR_SPEED_MIN;
  }

  if (speed > 0)
  {
    /* 正转 */
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
  }
  else if (speed < 0)
  {
    /* 反转 */
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_PIN, GPIO_PIN_SET);
  }
  else
  {
    /* 停止 */
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_PIN, GPIO_PIN_RESET);
  }

  /* 占空比映射：CCR = |speed| * ARR / 100
   * TIM2 配置 PSC=71, ARR=999，因此 CCR 范围 0~999 */
  abs_speed = (speed < 0) ? (uint32_t)(-speed) : (uint32_t)speed;
  ccr = abs_speed * (uint32_t)(htim2.Init.Period) / 100u;
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr);
}

void Motor_Stop(void)
{
  /* TB6612 刹车：IN1=IN2=1 */
  HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_PIN, GPIO_PIN_SET);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
}