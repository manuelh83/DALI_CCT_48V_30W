#include "power_limiter.h"

void PowerLimiter_Apply(float bus_voltage, float max_power_w, ChannelCurrents *currents)
{
    if ((currents == 0) || (bus_voltage <= 0.0f) || (max_power_w <= 0.0f)) {
        return;
    }

    const float total_a = (currents->ww_ma + currents->cw_ma) / 1000.0f;
    const float power = bus_voltage * total_a;
    if (power > max_power_w) {
        const float scale = max_power_w / power;
        currents->ww_ma *= scale;
        currents->cw_ma *= scale;
    }
}
