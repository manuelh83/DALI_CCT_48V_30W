#ifndef HAL_I2C_H
#define HAL_I2C_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void HalI2c_Init(void);
bool HalI2c_Write(uint8_t address, const uint8_t *buffer, size_t length);
bool HalI2c_Read(uint8_t address, uint8_t *buffer, size_t length);
void HalI2c_RegisterDevice(uint8_t address, uint8_t *backing_store, size_t length);

#endif
