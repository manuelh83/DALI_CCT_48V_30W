#include "hal_eeprom_i2c.h"

#include <string.h>

#include "config.h"

static uint8_t g_eeprom[FW_EEPROM_SIZE];

void HalEeprom_Init(void)
{
    memset(g_eeprom, 0xFF, sizeof(g_eeprom));
}

bool HalEeprom_Read(uint16_t address, uint8_t *buffer, size_t length)
{
    if ((buffer == 0) || ((size_t)address + length > sizeof(g_eeprom))) {
        return false;
    }
    memcpy(buffer, &g_eeprom[address], length);
    return true;
}

bool HalEeprom_Write(uint16_t address, const uint8_t *buffer, size_t length)
{
    if ((buffer == 0) || ((size_t)address + length > sizeof(g_eeprom))) {
        return false;
    }
    memcpy(&g_eeprom[address], buffer, length);
    return true;
}
