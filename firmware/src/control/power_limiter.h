#ifndef POWER_LIMITER_H
#define POWER_LIMITER_H

#include "config.h"

void PowerLimiter_Apply(float bus_voltage, float max_power_w, ChannelCurrents *currents);

#endif
