#include "hal_gpio.h"

static bool g_en_ww;
static bool g_en_cw;
static bool g_buck_enable;
static bool g_status_led;
static bool g_fault_led;
static bool g_ocp_ww;
static bool g_ocp_cw;

void HalGpio_Init(void)
{
    g_en_ww = false;
    g_en_cw = false;
    g_buck_enable = false;
    g_status_led = false;
    g_fault_led = false;
    g_ocp_ww = false;
    g_ocp_cw = false;
}

void HalGpio_SetEnWw(bool enabled) { g_en_ww = enabled; }
void HalGpio_SetEnCw(bool enabled) { g_en_cw = enabled; }
void HalGpio_SetBuckEnable(bool enabled) { g_buck_enable = enabled; }
void HalGpio_WriteStatusLed(bool on) { g_status_led = on; }
void HalGpio_WriteFaultLed(bool on) { g_fault_led = on; }
bool HalGpio_ReadOcpWw(void) { return g_ocp_ww; }
bool HalGpio_ReadOcpCw(void) { return g_ocp_cw; }
void HalGpio_InjectOcp(bool ww, bool cw) { g_ocp_ww = ww; g_ocp_cw = cw; }
