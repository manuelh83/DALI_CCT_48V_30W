#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

#include "config.h"

void HalAdc_Init(void);
uint16_t AdcRead(AdcChannel channel);
uint16_t HalAdc_ReadAverage(AdcChannel channel);
void HalAdc_SetRaw(AdcChannel channel, uint16_t raw_value);
float HalAdc_ToVoltage(AdcChannel channel, uint16_t raw_value);

#endif
