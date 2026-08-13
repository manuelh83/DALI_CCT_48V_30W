#include "hal_usart.h"

#define USART_BUFFER_SIZE 32U

static uint8_t g_rx[USART_BUFFER_SIZE];
static uint8_t g_tx[USART_BUFFER_SIZE];
static uint8_t g_rx_head;
static uint8_t g_rx_tail;
static uint8_t g_tx_count;

void HalUsart_Init(void)
{
    g_rx_head = 0U;
    g_rx_tail = 0U;
    g_tx_count = 0U;
}

void HalUsart_PushRx(uint8_t value)
{
    g_rx[g_rx_head] = value;
    g_rx_head = (uint8_t)((g_rx_head + 1U) % USART_BUFFER_SIZE);
}

bool HalUsart_PopRx(uint8_t *value)
{
    if ((value == 0) || (g_rx_tail == g_rx_head)) {
        return false;
    }
    *value = g_rx[g_rx_tail];
    g_rx_tail = (uint8_t)((g_rx_tail + 1U) % USART_BUFFER_SIZE);
    return true;
}

size_t HalUsart_Write(const uint8_t *data, size_t length)
{
    size_t written = 0U;
    for (size_t i = 0U; (i < length) && (g_tx_count < USART_BUFFER_SIZE); ++i) {
        g_tx[g_tx_count++] = data[i];
        ++written;
    }
    return written;
}

size_t HalUsart_ReadTx(uint8_t *data, size_t capacity)
{
    size_t copied = (g_tx_count < capacity) ? g_tx_count : capacity;
    for (size_t i = 0U; i < copied; ++i) {
        data[i] = g_tx[i];
    }
    g_tx_count = 0U;
    return copied;
}
