#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>

void HalTimer_Init(void);
void HalTimer_TickIsr(void);
void HalTimer_WaitForTick(void);
uint32_t HalTimer_GetMs(void);

#endif
