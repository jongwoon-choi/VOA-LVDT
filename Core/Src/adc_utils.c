/*
 * adc_utils.c
 *
 *  Created on: Jul 23, 2025
 *      Author: hl3xs
 */


#include "adc_utils.h"
#include "stm32_adc.h"  // ReadSTM32ADC() 선언

#include "ads1115.h"      // ReadADS1115() 선언



// 현재 루프에서 사용할 ADC를 선택할 수 있는 enum 또는 define
#define USE_STM32_ADC    0
#define USE_ADS1115      1

// 이 매크로를 변경하여 사용할 ADC를 선택
#define CURRENT_ADC_SOURCE   USE_ADS1115

float Read_CurrentADC(void)
{
#if CURRENT_ADC_SOURCE == USE_STM32_ADC
    uint16_t raw = ReadSTM32ADC();  // 내부 ADC에서 읽기
    float voltage = (float)raw * 3.3f / 4096.0f;  // 12bit, 3.3V 기준
    return voltage;

#elif CURRENT_ADC_SOURCE == USE_ADS1115
    int16_t raw = ADS1115_Read(1);  // CH1 등 원하는 채널로 변경
    float voltage = (float)raw * 4.096f / 32768.0f;  // ±4.096V 범위, 16bit
    return voltage;

#else
    return 0.0f; // 기본 반환값
#endif
}
