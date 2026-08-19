# F103_RM_Sim_Infantry

基于 STM32F103C8T6 + FreeRTOS 的 RoboMaster 步兵模拟工程（期末大作业）。

## 目录结构

- `Core/` — STM32CubeMX 生成代码（main、FreeRTOS、外设初始化）
- `Drivers/` — STM32 HAL 驱动库
- `Middlewares/` — FreeRTOS 中间件
- `bsp/` — 板级驱动（OLED、CAN、I2C 等）
- `component/` — 通用组件（PID、滤波器、字库等）
- `application/` — 应用层任务（底盘、云台、通信、UI 等）
- `cmake/` — CMake 构建配置

## 当前功能

- OLED（SSD1306）显示 "helloworld"，I2C1：PB6=SCL，PB7=SDA
- FreeRTOS 任务在 `freertos.c` 中为 `__weak` 声明，实际实现在 `application/` 对应模块（如 `application/UI/UI.c` 的 `Oled_DisplayTask`）

## 构建

需要 arm-none-eabi-gcc 与 Ninja，使用 CLion 打开后选择 Debug 预设即可编译，产物为 `build/Debug/F103_RM_Sim_Infantry.elf`。