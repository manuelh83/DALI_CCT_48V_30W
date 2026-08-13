#include "hal_dac_i2c.h"

#include <string.h>

#include "hal_i2c.h"

static uint8_t g_dac_image[8];
static uint16_t g_ww_code;
static uint16_t g_cw_code;

void HalDac_Init(void)
{
    memset(g_dac_image, 0, sizeof(g_dac_image));
    HalI2c_RegisterDevice(0x60U, g_dac_image, sizeof(g_dac_image));
}

bool HalDac_WriteChannels(uint16_t ww_code, uint16_t cw_code)
{
    g_ww_code = ww_code;
    g_cw_code = cw_code;
    g_dac_image[0] = (uint8_t)(ww_code >> 8U);
    g_dac_image[1] = (uint8_t)(ww_code & 0xFFU);
    g_dac_image[2] = (uint8_t)(cw_code >> 8U);
    g_dac_image[3] = (uint8_t)(cw_code & 0xFFU);
    return HalI2c_Write(0x60U, g_dac_image, sizeof(g_dac_image));
}

uint16_t HalDac_GetWwCode(void) { return g_ww_code; }
uint16_t HalDac_GetCwCode(void) { return g_cw_code; }
