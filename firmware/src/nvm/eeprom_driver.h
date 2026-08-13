#ifndef EEPROM_DRIVER_H
#define EEPROM_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool EepromDriver_Read(uint16_t address, uint8_t *buffer, size_t length);
bool EepromDriver_Write(uint16_t address, const uint8_t *buffer, size_t length);

#endif
