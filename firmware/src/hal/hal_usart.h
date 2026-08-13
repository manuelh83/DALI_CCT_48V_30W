#ifndef HAL_USART_H
#define HAL_USART_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void HalUsart_Init(void);
void HalUsart_PushRx(uint8_t value);
bool HalUsart_PopRx(uint8_t *value);
size_t HalUsart_Write(const uint8_t *data, size_t length);
size_t HalUsart_ReadTx(uint8_t *data, size_t capacity);

#endif
