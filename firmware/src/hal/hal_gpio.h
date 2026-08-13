#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdbool.h>

void HalGpio_Init(void);
void HalGpio_SetEnWw(bool enabled);
void HalGpio_SetEnCw(bool enabled);
void HalGpio_SetBuckEnable(bool enabled);
void HalGpio_WriteStatusLed(bool on);
void HalGpio_WriteFaultLed(bool on);
bool HalGpio_ReadOcpWw(void);
bool HalGpio_ReadOcpCw(void);
void HalGpio_InjectOcp(bool ww, bool cw);

#endif
