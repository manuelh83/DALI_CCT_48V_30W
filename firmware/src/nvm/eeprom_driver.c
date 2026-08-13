#include "eeprom_driver.h"

#include "hal_eeprom_i2c.h"

bool EepromDriver_Read(uint16_t address, uint8_t *buffer, size_t length)
{
    return HalEeprom_Read(address, buffer, length);
}

bool EepromDriver_Write(uint16_t address, const uint8_t *buffer, size_t length)
{
    return HalEeprom_Write(address, buffer, length);
}
