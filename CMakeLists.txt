cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME F103_RM_Sim_Infantry)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    component/support/font.c
    component/controller/PID.c
    bsp/Src/bsp_oled.c
    bsp/Src/bsp_adc.c
    bsp/Src/bsp_servo_pwm.c
    bsp/Src/bsp_motor.c
    bsp/Src/bsp_can.c
    bsp/Src/bsp_led.c
    application/UI/UI.c
    application/Operate/joystick.c
    application/Gimbal/Gimbal.c
    application/Chassis/Chassis.c
    application/Communication/can_send.c
    application/Communication/can_receive.c
    application/Detection/Detection.c
    application/System/System.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    bsp/Inc
    component/support
    application/UI
    application/Operate
    application/Chassis
    component/controller
    application/Communication
    application/Detection
    application/System
)

# Add project symbols (macros)
# 板级宏：二选一（烧录哪块板就定义哪个）
#   烧录云台板 -> GIMBAL
#   烧录底盘板 -> CHASSIS
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    CHASSIS
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
