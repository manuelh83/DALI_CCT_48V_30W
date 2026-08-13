#include "hal_i2c.h"

#include <string.h>

#define MAX_I2C_DEVICES 4U

typedef struct {
    uint8_t address;
    uint8_t *data;
    size_t length;
} I2cDevice;

static I2cDevice g_devices[MAX_I2C_DEVICES];

void HalI2c_Init(void)
{
    memset(g_devices, 0, sizeof(g_devices));
}

void HalI2c_RegisterDevice(uint8_t address, uint8_t *backing_store, size_t length)
{
    for (size_t i = 0U; i < MAX_I2C_DEVICES; ++i) {
        if (g_devices[i].data == 0) {
            g_devices[i].address = address;
            g_devices[i].data = backing_store;
            g_devices[i].length = length;
            break;
        }
    }
}

static I2cDevice *find_device(uint8_t address)
{
    for (size_t i = 0U; i < MAX_I2C_DEVICES; ++i) {
        if ((g_devices[i].data != 0) && (g_devices[i].address == address)) {
            return &g_devices[i];
        }
    }
    return 0;
}

bool HalI2c_Write(uint8_t address, const uint8_t *buffer, size_t length)
{
    I2cDevice *device = find_device(address);
    if ((device == 0) || (buffer == 0) || (length > device->length)) {
        return false;
    }
    memcpy(device->data, buffer, length);
    return true;
}

bool HalI2c_Read(uint8_t address, uint8_t *buffer, size_t length)
{
    I2cDevice *device = find_device(address);
    if ((device == 0) || (buffer == 0) || (length > device->length)) {
        return false;
    }
    memcpy(buffer, device->data, length);
    return true;
}
