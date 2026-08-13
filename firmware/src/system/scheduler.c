#include "scheduler.h"

#include "hal_timer.h"

void Scheduler_Init(void)
{
    HalTimer_Init();
}

void WaitForTick(void)
{
    HalTimer_WaitForTick();
}

uint32_t Scheduler_NowMs(void)
{
    return HalTimer_GetMs();
}

bool Scheduler_TimerExpired(uint32_t last_ms, uint32_t period_ms)
{
    return (Scheduler_NowMs() - last_ms) >= period_ms;
}
