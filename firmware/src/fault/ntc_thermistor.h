#ifndef NTC_THERMISTOR_H
#define NTC_THERMISTOR_H

#include <stdint.h>

float NtcThermistor_AdcToCelsius(uint16_t adc_code);

#endif
