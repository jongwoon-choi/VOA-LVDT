


#include "stm32_adc.h"
#include "main.h"  // HAL_ADC_HandleTypeDef

extern ADC_HandleTypeDef hadc1;

uint16_t ReadSTM32ADC(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return value;
}
