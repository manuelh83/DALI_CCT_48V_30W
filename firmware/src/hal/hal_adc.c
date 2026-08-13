#include "hal_adc.h"

#define ADC_WINDOW 10U

static uint16_t g_samples[ADC_CHANNEL_COUNT][ADC_WINDOW];
static uint8_t g_index[ADC_CHANNEL_COUNT];

void HalAdc_Init(void)
{
    for (uint8_t channel = 0U; channel < ADC_CHANNEL_COUNT; ++channel) {
        for (uint8_t i = 0U; i < ADC_WINDOW; ++i) {
            g_samples[channel][i] = 0U;
        }
        g_index[channel] = 0U;
    }
}

void HalAdc_SetRaw(AdcChannel channel, uint16_t raw_value)
{
    g_samples[channel][g_index[channel]] = raw_value;
    g_index[channel] = (uint8_t)((g_index[channel] + 1U) % ADC_WINDOW);
}

uint16_t HalAdc_ReadAverage(AdcChannel channel)
{
    uint32_t total = 0U;
    for (uint8_t i = 0U; i < ADC_WINDOW; ++i) {
        total += g_samples[channel][i];
    }
    return (uint16_t)(total / ADC_WINDOW);
}

uint16_t AdcRead(AdcChannel channel)
{
    return HalAdc_ReadAverage(channel);
}

float HalAdc_ToVoltage(AdcChannel channel, uint16_t raw_value)
{
    const float sense = ((float)raw_value / 4095.0f) * 3.3f;
    switch (channel) {
        case ADC_CHANNEL_VIN:
        case ADC_CHANNEL_VBUS:
            return sense / 0.048f;
        case ADC_CHANNEL_NTC:
        default:
            return sense;
    }
}
