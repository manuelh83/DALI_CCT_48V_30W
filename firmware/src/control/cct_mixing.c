#include "cct_mixing.h"

#include "power_limiter.h"

static float clampf(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

ChannelCurrents CctMixing_Compute(uint16_t tc_mirek, uint16_t tc_cool, uint16_t tc_warm, float total_current_ma, float bus_voltage, float max_power_w)
{
    ChannelCurrents currents = {0.0f, 0.0f};
    if (tc_warm <= tc_cool) {
        currents.cw_ma = total_current_ma;
        return currents;
    }

    const float alpha = clampf((float)(tc_mirek - tc_cool) / (float)(tc_warm - tc_cool), 0.0f, 1.0f);
    currents.ww_ma = total_current_ma * alpha;
    currents.cw_ma = total_current_ma * (1.0f - alpha);
    PowerLimiter_Apply(bus_voltage, max_power_w, &currents);
    return currents;
}
