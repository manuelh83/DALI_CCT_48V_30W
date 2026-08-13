#include "hal/hal_timer.h"

void TIM14_IRQHandler(void)
{
    HalTimer_TickIsr();
}

void Default_Handler(void)
{
    while (1) {
    }
}
