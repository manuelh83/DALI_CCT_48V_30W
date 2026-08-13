#include "dali_device_type.h"

uint16_t DaliDeviceType_ClampTc(const DaliState *state, uint16_t tc_mirek)
{
    uint16_t tc = tc_mirek;
    if (tc < state->persistent.tc_cool_mirek) {
        tc = state->persistent.tc_cool_mirek;
    }
    if (tc > state->persistent.tc_warm_mirek) {
        tc = state->persistent.tc_warm_mirek;
    }
    return tc;
}

void DaliDeviceType_SetTc(DaliState *state, uint16_t tc_mirek)
{
    state->target_tc_mirek = DaliDeviceType_ClampTc(state, tc_mirek);
    state->state_changed = true;
}
