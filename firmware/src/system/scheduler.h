#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

void Scheduler_Init(void);
void WaitForTick(void);
uint32_t Scheduler_NowMs(void);
bool Scheduler_TimerExpired(uint32_t last_ms, uint32_t period_ms);

#endif
