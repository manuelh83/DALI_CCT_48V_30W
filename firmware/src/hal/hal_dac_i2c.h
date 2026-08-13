#ifndef HAL_DAC_I2C_H
#define HAL_DAC_I2C_H

#include <stdbool.h>
#include <stdint.h>

void HalDac_Init(void);
bool HalDac_WriteChannels(uint16_t ww_code, uint16_t cw_code);
uint16_t HalDac_GetWwCode(void);
uint16_t HalDac_GetCwCode(void);

#endif
