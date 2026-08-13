#include "current_sink.h"

#include "cct_mixing.h"
#include "dali_application.h"

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

void CurrentSink_Init(void)
{
}

uint16_t CurrentSink_CurrentToCode(float current_ma, float current_limit_ma)
{
    if (current_limit_ma <= 0.0f) {
        return 0U;
    }
    const float clamped = clampf(current_ma, 0.0f, current_limit_ma);
    return (uint16_t)((clamped / current_limit_ma) * 4095.0f + 0.5f);
}

CurrentSinkOutput CurrentSink_Update(const DaliState *state, float current_limit_ma)
{
    CurrentSinkOutput output;
    const float total_current_ma = DaliApplication_LevelToCurrentMa(state, state->actual_level);
    output.currents = CctMixing_Compute(
        state->actual_tc_mirek,
        state->persistent.tc_cool_mirek,
        state->persistent.tc_warm_mirek,
        total_current_ma,
        FW_FIXED_BUS_VOLTAGE,
        FW_POWER_LIMIT_W);

    output.currents.ww_ma = clampf(output.currents.ww_ma, 0.0f, current_limit_ma);
    output.currents.cw_ma = clampf(output.currents.cw_ma, 0.0f, current_limit_ma);
    output.dac_ww = CurrentSink_CurrentToCode(output.currents.ww_ma, current_limit_ma);
    output.dac_cw = CurrentSink_CurrentToCode(output.currents.cw_ma, current_limit_ma);
    output.enable_ww = output.currents.ww_ma >= 0.5f;
    output.enable_cw = output.currents.cw_ma >= 0.5f;
    return output;
}
