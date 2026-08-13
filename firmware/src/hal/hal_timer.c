#include "hal_timer.h"

#include "config.h"

static uint32_t g_ms;

void HalTimer_Init(void)
{
    g_ms = 0U;
}

void HalTimer_TickIsr(void)
{
    g_ms += FW_FADE_TICK_MS;
}

void HalTimer_WaitForTick(void)
{
#ifdef HOST_BUILD
    HalTimer_TickIsr();
#endif
}

uint32_t HalTimer_GetMs(void)
{
    return g_ms;
}
