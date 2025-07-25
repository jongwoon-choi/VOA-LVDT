/*
 * ads1115.h
 *
 *  Created on: Jul 23, 2025
 *      Author: hl3xs
 */

#ifndef __ADS1115_H__
#define __ADS1115_H__

// ADS1115 I2C 기본 주소 (ADDR 핀이 GND일 경우)
#define ADS1115_ADDRESS         (0x48 << 1)  // 7-bit 주소 + R/W bit

// ADS1115 레지스터 주소
#define ADS1115_REG_CONVERSION  0x00
#define ADS1115_REG_CONFIG      0x01
#define ADS1115_REG_LO_THRESH   0x02
#define ADS1115_REG_HI_THRESH   0x03


#include "stdint.h"


int16_t ADS1115_Read(uint8_t channel);  // 선언 추가
float ADS1115_ReadVoltage(uint8_t channel);  // 이미 있을 것

#endif
