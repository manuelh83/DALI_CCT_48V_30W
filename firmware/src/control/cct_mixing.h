#ifndef CCT_MIXING_H
#define CCT_MIXING_H

#include <stdint.h>

#include "config.h"

ChannelCurrents CctMixing_Compute(uint16_t tc_mirek, uint16_t tc_cool, uint16_t tc_warm, float total_current_ma, float bus_voltage, float max_power_w);

#endif
