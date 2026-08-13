#include "dali_application.h"

#include <math.h>
#include <string.h>

#include "dali_device_type.h"
#include "dali_link.h"

static uint8_t clamp_level(const DaliState *state, uint8_t level)
{
    uint8_t clamped = level;
    if (level == 0U) {
        return 0U;
    }
    if (clamped < state->persistent.min_level) {
        clamped = state->persistent.min_level;
    }
    if (clamped > state->persistent.max_level) {
        clamped = state->persistent.max_level;
    }
    return clamped;
}

static void set_target_level(DaliState *state, uint8_t level)
{
    const uint8_t clamped = clamp_level(state, level);
    if (clamped != level) {
        state->status_bits |= DALI_STATUS_LIMIT_ERROR;
    }
    state->target_level = clamped;
    state->state_changed = true;
}

static void set_response(DaliState *state, uint8_t response)
{
    DaliLink_SetResponse(state, response);
}

void DaliState_Init(DaliState *state)
{
    memset(state, 0, sizeof(*state));
    state->persistent.short_address = FW_DALI_UNASSIGNED_ADDRESS;
    for (size_t i = 0; i < FW_DALI_MAX_SCENES; ++i) {
        state->persistent.scene_levels[i] = FW_DALI_LAST_ACTIVE;
        state->persistent.scene_tcs[i] = 0U;
    }
    state->persistent.max_level = FW_DALI_MAX_LEVEL;
    state->persistent.min_level = FW_DALI_MIN_LEVEL;
    state->persistent.power_on_level = FW_DALI_LAST_ACTIVE;
    state->persistent.system_failure_level = FW_DALI_LAST_ACTIVE;
    state->persistent.fade_time = 0U;
    state->persistent.fade_rate = 7U;
    state->persistent.physical_min_level = FW_DALI_MIN_LEVEL;
    state->persistent.device_type = FW_DEVICE_TYPE_DT8;
    state->persistent.tc_cool_mirek = FW_DALI_TC_COOL_DEFAULT;
    state->persistent.tc_warm_mirek = FW_DALI_TC_WARM_DEFAULT;
    state->dynamic.last_arc_level = FW_DALI_MAX_LEVEL;
    state->dynamic.last_tc_mirek = FW_DALI_RESET_TC_MIREK;
    state->dynamic.valid_marker = 0xA5U;
    state->actual_level = 0U;
    state->target_level = 0U;
    state->actual_tc_mirek = FW_DALI_RESET_TC_MIREK;
    state->target_tc_mirek = FW_DALI_RESET_TC_MIREK;
    state->status_bits = DALI_STATUS_RESET_STATE | DALI_STATUS_MISSING_SHORT_ADDRESS;
}

void DaliApplication_Init(DaliState *state)
{
    DaliState_Init(state);
    DaliLink_Init(state);
}

bool DaliApplication_AddressMatches(const DaliState *state, uint8_t address_byte)
{
    if (address_byte == 0xFFU) {
        return true;
    }
    if (state->persistent.short_address == FW_DALI_UNASSIGNED_ADDRESS) {
        return false;
    }
    return (uint8_t)(address_byte >> 1U) == state->persistent.short_address;
}

float DaliApplication_LevelToCurrentMa(const DaliState *state, uint8_t level)
{
    (void)state;
    if (level == 0U) {
        return 0.0f;
    }
    const float ratio = (float)level / (float)FW_DALI_MAX_LEVEL;
    const float min_ratio = 0.001f;
    const float scaled = powf(10.0f, ((ratio - 1.0f) * log10f(1.0f / min_ratio)));
    return FW_I_MAX_MA * scaled;
}

static void handle_query(const DaliState *state, DaliState *mutable_state, uint8_t command)
{
    switch (command) {
        case DALI_CMD_QUERY_STATUS:
            set_response(mutable_state, state->status_bits);
            break;
        case DALI_CMD_QUERY_CONTROL_GEAR_PRESENT:
            set_response(mutable_state, 0xFFU);
            break;
        case DALI_CMD_QUERY_ACTUAL_LEVEL:
            set_response(mutable_state, state->actual_level);
            break;
        case DALI_CMD_QUERY_MAX_LEVEL:
            set_response(mutable_state, state->persistent.max_level);
            break;
        case DALI_CMD_QUERY_MIN_LEVEL:
            set_response(mutable_state, state->persistent.min_level);
            break;
        case DALI_CMD_QUERY_POWER_ON_LEVEL:
            set_response(mutable_state, state->persistent.power_on_level);
            break;
        case DALI_CMD_QUERY_SYSTEM_FAILURE_LEVEL:
            set_response(mutable_state, state->persistent.system_failure_level);
            break;
        case DALI_CMD_QUERY_FADE_TIME:
            set_response(mutable_state, state->persistent.fade_time);
            break;
        case DALI_CMD_QUERY_FADE_RATE:
            set_response(mutable_state, state->persistent.fade_rate);
            break;
        case DALI_CMD_QUERY_SHORT_ADDRESS:
            set_response(mutable_state, state->persistent.short_address == FW_DALI_UNASSIGNED_ADDRESS ? 0xFFU : (uint8_t)(state->persistent.short_address << 1U));
            break;
        case DALI_CMD_QUERY_TC:
            set_response(mutable_state, (uint8_t)(state->target_tc_mirek & 0xFFU));
            break;
        case DALI_CMD_QUERY_TC_COOL:
            set_response(mutable_state, (uint8_t)(state->persistent.tc_cool_mirek & 0xFFU));
            break;
        case DALI_CMD_QUERY_TC_WARM:
            set_response(mutable_state, (uint8_t)(state->persistent.tc_warm_mirek & 0xFFU));
            break;
        default:
            if ((command >= DALI_CMD_QUERY_SCENE_LEVEL_0) && (command < (DALI_CMD_QUERY_SCENE_LEVEL_0 + FW_DALI_MAX_SCENES))) {
                const uint8_t scene = (uint8_t)(command - DALI_CMD_QUERY_SCENE_LEVEL_0);
                set_response(mutable_state, state->persistent.scene_levels[scene]);
            }
            break;
    }
}

void DaliApplication_Process(DaliState *state, uint16_t frame, uint32_t now_ms)
{
    const uint8_t address = (uint8_t)(frame >> 8U);
    const uint8_t data = (uint8_t)(frame & 0xFFU);
    const bool command_frame = (address & 0x01U) != 0U;
    const bool address_match = DaliApplication_AddressMatches(state, address);
    const bool special_broadcast = (address == 0xFFU);

    if (!address_match && !special_broadcast) {
        return;
    }

    state->last_valid_rx_ms = now_ms;
    state->status_bits &= (uint8_t)~DALI_STATUS_RESET_STATE;

    if (!command_frame) {
        set_target_level(state, data);
        return;
    }

    switch (data) {
        case DALI_CMD_OFF:
            set_target_level(state, 0U);
            break;
        case DALI_CMD_RECALL_MAX:
            set_target_level(state, state->persistent.max_level);
            break;
        case DALI_CMD_RECALL_MIN:
            set_target_level(state, state->persistent.min_level);
            break;
        case DALI_CMD_UP:
        case DALI_CMD_STEP_UP:
            set_target_level(state, (uint8_t)(state->target_level < FW_DALI_MAX_LEVEL ? state->target_level + 1U : FW_DALI_MAX_LEVEL));
            break;
        case DALI_CMD_DOWN:
        case DALI_CMD_STEP_DOWN:
            set_target_level(state, (uint8_t)(state->target_level > 0U ? state->target_level - 1U : 0U));
            break;
        case DALI_CMD_SET_MAX_LEVEL:
            state->persistent.max_level = clamp_level(state, state->dtr0);
            if (state->persistent.max_level < state->persistent.min_level) {
                state->persistent.max_level = state->persistent.min_level;
            }
            break;
        case DALI_CMD_SET_MIN_LEVEL:
            state->persistent.min_level = state->dtr0 == 0U ? 1U : state->dtr0;
            if (state->persistent.min_level > state->persistent.max_level) {
                state->persistent.min_level = state->persistent.max_level;
            }
            break;
        case DALI_CMD_SET_SYSTEM_FAILURE_LEVEL:
            state->persistent.system_failure_level = state->dtr0;
            break;
        case DALI_CMD_SET_POWER_ON_LEVEL:
            state->persistent.power_on_level = state->dtr0;
            break;
        case DALI_CMD_SET_FADE_TIME:
            state->persistent.fade_time = (uint8_t)(state->dtr0 & 0x0FU);
            break;
        case DALI_CMD_SET_FADE_RATE:
            state->persistent.fade_rate = (uint8_t)(state->dtr0 & 0x0FU);
            break;
        case DALI_CMD_INITIALISE:
            state->initialise_enabled = true;
            state->withdrawn = false;
            break;
        case DALI_CMD_RANDOMISE:
            if (state->initialise_enabled) {
                state->random_address = 0x0055AA11UL;
            }
            break;
        case DALI_CMD_COMPARE:
            if (state->initialise_enabled && !state->withdrawn) {
                set_response(state, 0xFFU);
            }
            break;
        case DALI_CMD_WITHDRAW:
            state->withdrawn = true;
            break;
        case DALI_CMD_PROGRAM_SHORT_ADDRESS:
            if (state->initialise_enabled) {
                state->persistent.short_address = (uint8_t)(state->dtr0 & 0x3FU);
                state->status_bits &= (uint8_t)~DALI_STATUS_MISSING_SHORT_ADDRESS;
            }
            break;
        case DALI_CMD_VERIFY_SHORT_ADDRESS:
            if (state->persistent.short_address == (uint8_t)(state->dtr0 & 0x3FU)) {
                set_response(state, 0xFFU);
            }
            break;
        case DALI_CMD_QUERY_SHORT_ADDRESS:
        case DALI_CMD_QUERY_STATUS:
        case DALI_CMD_QUERY_CONTROL_GEAR_PRESENT:
        case DALI_CMD_QUERY_ACTUAL_LEVEL:
        case DALI_CMD_QUERY_MAX_LEVEL:
        case DALI_CMD_QUERY_MIN_LEVEL:
        case DALI_CMD_QUERY_POWER_ON_LEVEL:
        case DALI_CMD_QUERY_SYSTEM_FAILURE_LEVEL:
        case DALI_CMD_QUERY_FADE_TIME:
        case DALI_CMD_QUERY_FADE_RATE:
        case DALI_CMD_QUERY_TC:
        case DALI_CMD_QUERY_TC_COOL:
        case DALI_CMD_QUERY_TC_WARM:
            handle_query(state, state, data);
            break;
        case DALI_CMD_SET_TC:
            DaliDeviceType_SetTc(state, state->dtr_tc_mirek);
            break;
        case DALI_CMD_SET_TC_COOL:
            state->persistent.tc_cool_mirek = DaliDeviceType_ClampTc(state, state->dtr_tc_mirek);
            break;
        case DALI_CMD_SET_TC_WARM:
            state->persistent.tc_warm_mirek = DaliDeviceType_ClampTc(state, state->dtr_tc_mirek);
            break;
        default:
            if ((data >= DALI_CMD_GO_TO_SCENE_0) && (data < (DALI_CMD_GO_TO_SCENE_0 + FW_DALI_MAX_SCENES))) {
                const uint8_t scene = (uint8_t)(data - DALI_CMD_GO_TO_SCENE_0);
                if (state->persistent.scene_levels[scene] != FW_DALI_LAST_ACTIVE) {
                    set_target_level(state, state->persistent.scene_levels[scene]);
                }
                if (state->persistent.scene_tcs[scene] != 0U) {
                    DaliDeviceType_SetTc(state, state->persistent.scene_tcs[scene]);
                }
            } else if ((data >= DALI_CMD_QUERY_SCENE_LEVEL_0) && (data < (DALI_CMD_QUERY_SCENE_LEVEL_0 + FW_DALI_MAX_SCENES))) {
                handle_query(state, state, data);
            }
            break;
    }
}
