/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
// FreeRTOS 헤더
#include "cmsis_os.h"
#include "control_vars.h"  // 변수 정의는 control_vars.c 안에 있음
#include <stdio.h>  // snprintf
#include "telemetry.h"

float position_output = 0.0f;
float current_output = 0.0f;


// Task 핸들
osThreadId_t CurrentLoopTaskHandle;
osThreadId_t PositionLoopTaskHandle;

extern void MCP4922_WriteChannelA(uint16_t val);
extern float ADS1115_ReadVoltage(uint8_t channel);
extern uint16_t Read_CurrentADC(void);

extern void InitControlVars(void);
extern ControlParams_t controlParams;

extern UART_HandleTypeDef huart2;

float adc_to_current(uint16_t adc);  // 함수 선언 추가


// UART 송신용 버퍼
char uart_buffer[128];

float adc_to_current(uint16_t adc) {
    return ((float)adc * 3.3f / 4096.0f - 1.65f) / (10.0f * 0.25f);
}

// telemetry task
void UartTelemetryTask(void *argument) {
    for (;;) {
        float pos_set = controlParams.position_setpoint;
        float pos = ADS1115_ReadVoltage(1) * controlParams.position_scale;
        float cur_set = controlParams.current_setpoint;
        float cur_fb = adc_to_current(Read_CurrentADC());
        float ctrl_V = cur_set * 10.0f * 0.25f + 1.65f;

        SendTelemetryPacket(pos_set, pos, cur_set, cur_fb, ctrl_V, position_output, current_output);

        osDelay(20);  // 50Hz
    }
}


// PID 변수



// 1kHz 전류 루프
void CurrentLoopTask(void *argument) {
    for (;;) {
        uint16_t adc_val = Read_CurrentADC();
        float feedback_current = adc_to_current(adc_val);

        float error = controlParams.current_setpoint - feedback_current;
        controlParams.current_loop.integral += error * 0.001f;  // 1 ms 주기
        float control_voltage = controlParams.current_loop.Kp * error +
                                controlParams.current_loop.Ki * controlParams.current_loop.integral;

        current_output = control_voltage;  // 전류 루프 PI 출력 저장

        if (control_voltage < 0) control_voltage = 0;
        if (control_voltage > 3.3f) control_voltage = 3.3f;

        uint16_t dac_val = (uint16_t)((control_voltage / 3.3f) * 4095.0f);
        MCP4922_WriteChannelA(dac_val);

        osDelay(1);  // 1ms 주기
    }
}

// 200Hz 위치 루프
void PositionLoopTask(void *argument) {
    for (;;) {
        float voltage = ADS1115_ReadVoltage(1);  // AIN1
        float position = voltage * controlParams.position_scale;
        float error = controlParams.position_setpoint - position;
        controlParams.position_loop.integral += error * 0.005f;  // 5ms 주기
        controlParams.current_setpoint = controlParams.position_loop.Kp * error +
                                         controlParams.position_loop.Ki * controlParams.position_loop.integral;

        position_output = controlParams.current_setpoint;  // 위치 루프 PI 출력 저장

        osDelay(5);  // 5ms 주기
    }
}

// FreeRTOS task 생성
osThreadId_t TelemetryTaskHandle;

void StartControlTasks(void) {
    const osThreadAttr_t attr1 = {.name = "CurrentLoopTask", .priority = osPriorityHigh, .stack_size = 512};
    const osThreadAttr_t attr2 = {.name = "PositionLoopTask", .priority = osPriorityNormal, .stack_size = 512};
    const osThreadAttr_t attr3 = {.name = "UartTelemetryTask", .priority = osPriorityLow, .stack_size = 512};

    CurrentLoopTaskHandle = osThreadNew(CurrentLoopTask, NULL, &attr1);
    PositionLoopTaskHandle = osThreadNew(PositionLoopTask, NULL, &attr2);
    TelemetryTaskHandle = osThreadNew(UartTelemetryTask, NULL, &attr3);
}

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
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
//	InitControlVars();  // 초기 제어 파라미터 설정
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
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

