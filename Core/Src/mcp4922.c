/*
 * mcp4922.c
 *
 *  Created on: Jul 22, 2025
 *      Author: hl3xs
 */

#include "stm32g4xx_hal.h"

extern SPI_HandleTypeDef hspi1;  // SPI1 사용

#define MCP4922_CS_GPIO_Port GPIOA
#define MCP4922_CS_Pin       GPIO_PIN_4

void MCP4922_Write(uint8_t channel, uint16_t value) {
    uint16_t command = 0;
    value &= 0x0FFF;  // 12bit 마스킹

    command |= (0x3000);           // 기본: BUF=1, GA=1 (x1), SHDN=1
    if (channel == 1) command |= (1 << 15); // 채널 B
    command |= value;

    uint8_t data[2];
    data[0] = (command >> 8) & 0xFF;
    data[1] = command & 0xFF;

    HAL_GPIO_WritePin(MCP4922_CS_GPIO_Port, MCP4922_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(MCP4922_CS_GPIO_Port, MCP4922_CS_Pin, GPIO_PIN_SET);
}

void MCP4922_WriteChannelA(uint16_t val) {
    MCP4922_Write(0, val);
}

void MCP4922_WriteChannelB(uint16_t val) {
    MCP4922_Write(1, val);
}

