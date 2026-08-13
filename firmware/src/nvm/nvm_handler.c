#include "nvm_handler.h"

#include <string.h>

#include "eeprom_driver.h"
#include "nvm_layout.h"
#include "wear_leveling.h"

bool NvmHandler_Load(DaliState *state)
{
    bool ok = EepromDriver_Read(NVM_STATIC_PAGE_ADDRESS, (uint8_t *)&state->persistent, sizeof(state->persistent));
    if (!ok || (state->persistent.device_type != FW_DEVICE_TYPE_DT8)) {
        state->persistent.device_type = FW_DEVICE_TYPE_DT8;
    }
    if (WearLeveling_Load(&state->dynamic)) {
        state->actual_level = state->dynamic.last_arc_level;
        state->target_level = state->dynamic.last_arc_level;
        state->actual_tc_mirek = state->dynamic.last_tc_mirek;
        state->target_tc_mirek = state->dynamic.last_tc_mirek;
    }
    return ok;
}

bool NvmHandler_SavePersistent(const DaliState *state)
{
    return EepromDriver_Write(NVM_STATIC_PAGE_ADDRESS, (const uint8_t *)&state->persistent, sizeof(state->persistent));
}

bool NvmHandler_SaveDynamic(DaliState *state)
{
    state->dynamic.last_arc_level = state->actual_level;
    state->dynamic.last_tc_mirek = state->actual_tc_mirek;
    state->dynamic.valid_marker = 0xA5U;
    state->last_state_store_ms = state->last_valid_rx_ms;
    return WearLeveling_Store(&state->dynamic);
}
