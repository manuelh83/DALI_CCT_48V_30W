#include "ntc_thermistor.h"

#include <math.h>

float NtcThermistor_AdcToCelsius(uint16_t adc_code)
{
    if (adc_code == 0U) {
        return -40.0f;
    }
    if (adc_code >= 4095U) {
        return 125.0f;
    }

    const float pullup = 100000.0f;
    const float resistance = pullup * ((float)adc_code / (4095.0f - (float)adc_code));
    const float beta = 3950.0f;
    const float t0 = 298.15f;
    const float r0 = 100000.0f;
    const float temp_k = 1.0f / ((1.0f / t0) + (logf(resistance / r0) / beta));
    return temp_k - 273.15f;
}
