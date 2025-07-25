/*
 * ads1115.c
 *
 *  Created on: Jul 22, 2025
 *      Author: hl3xs
 */

#include "ads1115.h"
#include "stm32g4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

#define ADS1115_ADDR     (0x48 << 1)  // 7bit 주소 << 1
#define ADS1115_CONFIG_REG 0x01
#define ADS1115_CONV_REG   0x00

extern I2C_HandleTypeDef hi2c1;  // 사용중인 I2C 핸들


void ADS1115_Config_SingleEnded(uint8_t channel) {
    uint16_t config = 0x4000;  // OS=1 (start conversion), MODE=single-shot
    config |= (channel & 0x03) << 12;  // MUX: AINx-GND
    config |= (0x02 << 9);  // PGA ±4.096V
    config |= (0x04 << 5);  // DR=250SPS
    config |= 0x0003;       // COMP_DISABLE

    uint8_t config_data[3];
    config_data[0] = ADS1115_CONFIG_REG;
    config_data[1] = (config >> 8) & 0xFF;
    config_data[2] = config & 0xFF;

    HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADDR, config_data, 3, HAL_MAX_DELAY);
}

int16_t ADS1115_ReadConversion(void) {
    uint8_t reg = ADS1115_CONV_REG;
    uint8_t buf[2];

    HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADDR, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, ADS1115_ADDR, buf, 2, HAL_MAX_DELAY);

    return (int16_t)((buf[0] << 8) | buf[1]);
}

float ADS1115_ReadVoltage(uint8_t channel) {
    ADS1115_Config_SingleEnded(channel);
    HAL_Delay(5);  // Conversion time (4ms @ 250SPS)
    int16_t raw = ADS1115_ReadConversion();
    return (float)raw * 4.096f / 32768.0f;
}



int16_t ADS1115_Read(uint8_t channel)
{
    uint8_t config[2];
    uint8_t data[2];
    uint16_t config_value = 0;

    // 입력 채널 설정 (MUX[14:12])
    switch(channel) {
        case 0: config_value = 0x4000; break;  // AIN0
        case 1: config_value = 0x5000; break;  // AIN1
        case 2: config_value = 0x6000; break;  // AIN2
        case 3: config_value = 0x7000; break;  // AIN3
        default: return 0; // invalid channel
    }

    config_value |= 0x8000;  // [15] OS = 1 (start single conversion)
    config_value |= 0x0200;  // [11:9] PGA = ±4.096V
    config_value |= 0x0100;  // [8] MODE = single-shot
    config_value |= 0x0080;  // [7:5] DR = 128 SPS
    config_value |= 0x0003;  // [1:0] COMP_QUE = disable comparator

    config[0] = (config_value >> 8) & 0xFF;
    config[1] = config_value & 0xFF;

    // Write config to start conversion
    uint8_t cmd = ADS1115_REG_CONFIG;
    if (HAL_I2C_Mem_Write(&hi2c1, ADS1115_ADDRESS, cmd, I2C_MEMADD_SIZE_8BIT, config, 2, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    // Wait for conversion to complete (최대 8ms, 128SPS 기준)
    HAL_Delay(9);

    // Read conversion result
    cmd = ADS1115_REG_CONVERSION;
    if (HAL_I2C_Mem_Read(&hi2c1, ADS1115_ADDRESS, cmd, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    // Combine MSB and LSB
    return (int16_t)((data[0] << 8) | data[1]);
}

