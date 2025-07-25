/*
 * telemetry.c
 *
 *  Created on: Jul 23, 2025
 *      Author: hl3xs
 */


#include "telemetry.h"
#include "main.h"
#include "cmsis_os.h"

extern UART_HandleTypeDef huart2;

// FreeRTOS 큐/Task 핸들
static osMessageQueueId_t telemetryQueueHandle;
static osThreadId_t telemetryTaskHandle;
static StaticQueue_t queueControlBlock;
static uint8_t queueStorage[10 * sizeof(TelemetryPacket_t)];  // 최대 10개 버퍼
static StaticTask_t taskControlBlock;
static StackType_t telemetryTaskStack[512];

static uint8_t calc_crc8(uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
    }
    return crc;
}

// 송신 전용 Task
static void TelemetryTask(void *argument) {
    TelemetryPacket_t pkt;
    for (;;) {
        if (osMessageQueueGet(telemetryQueueHandle, &pkt, NULL, osWaitForever) == osOK) {
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *)&pkt, sizeof(pkt));
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // 완료까지 대기
        }
    }
}

// 송신 요청 함수 (큐에 넣음)
void EnqueueTelemetryPacket(uint16_t pos_set_dac,
                            int16_t  pos_fb_adc,
                            uint16_t cur_set_dac,
                            uint16_t cur_fb_adc,
                            uint16_t ctrl_dac_val,
                            int16_t  pos_out_raw,
                            int16_t  cur_out_raw)
{
    TelemetryPacket_t pkt;
    pkt.header        = 0xA5;
    pkt.pos_set_dac   = pos_set_dac;
    pkt.pos_fb_adc    = pos_fb_adc;
    pkt.cur_set_dac   = cur_set_dac;
    pkt.cur_fb_adc    = cur_fb_adc;
    pkt.pos_err_dac   = (int16_t)(pos_set_dac - (uint16_t)pos_fb_adc);
    pkt.cur_err_adc   = (int16_t)(cur_set_dac - cur_fb_adc);
    pkt.ctrl_dac_val  = ctrl_dac_val;
    pkt.pos_out_raw   = pos_out_raw;
    pkt.cur_out_raw   = cur_out_raw;
    pkt.crc8          = calc_crc8((uint8_t *)&pkt, sizeof(pkt) - 2);
    pkt.footer        = 0x5A;

    osMessageQueuePut(telemetryQueueHandle, &pkt, 0, 0);  // 비차단 방식
}

// 초기화 함수
void InitTelemetrySystem(void) {
    telemetryQueueHandle = osMessageQueueNew(10, sizeof(TelemetryPacket_t), &queueControlBlock);
    telemetryTaskHandle  = osThreadNew(TelemetryTask, NULL, &(osThreadAttr_t){
        .name = "TelemetryTask",
        .stack_mem = telemetryTaskStack,
        .stack_size = sizeof(telemetryTaskStack),
        .cb_mem = &taskControlBlock,
        .cb_size = sizeof(taskControlBlock),
        .priority = osPriorityNormal
    });
}

// 송신 완료 콜백
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(telemetryTaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

