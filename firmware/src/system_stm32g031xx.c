#include <stdint.h>

uint32_t SystemCoreClock = 64000000U;

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void SystemCoreClockUpdate(void)
{
    SystemCoreClock = 64000000U;
}

void SystemInit(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }
    for (dst = &_sbss; dst < &_ebss; ++dst) {
        *dst = 0U;
    }
    SystemCoreClockUpdate();
}
