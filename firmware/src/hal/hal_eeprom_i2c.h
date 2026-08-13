#ifndef HAL_EEPROM_I2C_H
#define HAL_EEPROM_I2C_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void HalEeprom_Init(void);
bool HalEeprom_Read(uint16_t address, uint8_t *buffer, size_t length);
bool HalEeprom_Write(uint16_t address, const uint8_t *buffer, size_t length);

#endif
