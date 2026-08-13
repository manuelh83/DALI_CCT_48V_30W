#include "protection.h"

#include "fault_log.h"

void Protection_Init(void)
{
    FaultLog_Clear();
}

ProtectionDecision Protection_Monitor(DaliState *state, float vin_v, float vbus_v, float temperature_c, bool ocp_ww, bool ocp_cw)
{
    (void)vbus_v;
    ProtectionDecision decision = {FW_I_MAX_MA, true, true, true};

    if (ocp_ww) {
        FaultLog_Set(FAULT_OCP_WW);
        state->status_bits |= DALI_STATUS_LAMP_FAILURE;
        decision.ww_enable = false;
        if (state->ocp_retry_ww < 3U) {
            state->ocp_retry_ww++;
        } else {
            decision.buck_enable = false;
        }
    }
    if (ocp_cw) {
        FaultLog_Set(FAULT_OCP_CW);
        state->status_bits |= DALI_STATUS_LAMP_FAILURE;
        decision.cw_enable = false;
        if (state->ocp_retry_cw < 3U) {
            state->ocp_retry_cw++;
        } else {
            decision.buck_enable = false;
        }
    }

    if (temperature_c >= FW_TEMP_SHUTDOWN_C) {
        FaultLog_Set(FAULT_TEMP_SHUTDOWN);
        decision.current_limit_ma = 0.0f;
        decision.buck_enable = false;
        decision.ww_enable = false;
        decision.cw_enable = false;
        state->status_bits |= DALI_STATUS_BALLAST_FAILURE;
    } else if (temperature_c >= FW_TEMP_WARN_C) {
        FaultLog_Set(FAULT_TEMP_WARN);
        decision.current_limit_ma = FW_I_THERMAL_MA;
    } else if (temperature_c < FW_TEMP_WARN_CLEAR_C) {
        FaultLog_Reset(FAULT_TEMP_WARN);
    }

    if (vin_v < FW_INPUT_UV_V) {
        FaultLog_Set(FAULT_INPUT_UV);
        decision.buck_enable = false;
        decision.ww_enable = false;
        decision.cw_enable = false;
    } else if (vin_v > FW_INPUT_OV_V) {
        FaultLog_Set(FAULT_INPUT_OV);
        decision.buck_enable = false;
        decision.ww_enable = false;
        decision.cw_enable = false;
    } else {
        if (vin_v > FW_INPUT_UV_RECOVER_V) {
            FaultLog_Reset(FAULT_INPUT_UV);
        }
        if (vin_v < FW_INPUT_OV_RECOVER_V) {
            FaultLog_Reset(FAULT_INPUT_OV);
        }
    }

    return decision;
}
