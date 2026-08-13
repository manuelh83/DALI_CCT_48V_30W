#include "fade_engine.h"

static uint8_t abs_diff_u8(uint8_t a, uint8_t b)
{
    return (a > b) ? (uint8_t)(a - b) : (uint8_t)(b - a);
}

static uint16_t abs_diff_u16(uint16_t a, uint16_t b)
{
    return (a > b) ? (uint16_t)(a - b) : (uint16_t)(b - a);
}

void FadeEngine_Init(DaliState *state)
{
    state->actual_level = state->target_level;
    state->actual_tc_mirek = state->target_tc_mirek;
}

uint8_t FadeEngine_LevelStep(uint8_t fade_time)
{
    return (uint8_t)((fade_time == 0U) ? FW_DALI_MAX_LEVEL : (fade_time > 10U ? 1U : (11U - fade_time)));
}

uint16_t FadeEngine_TcStep(uint8_t fade_time)
{
    return (uint16_t)((fade_time == 0U) ? 0xFFFFU : (uint16_t)(FadeEngine_LevelStep(fade_time) * 4U));
}

bool FadeEngine_Update(DaliState *state)
{
    bool changed = false;
    const uint8_t level_step = FadeEngine_LevelStep(state->persistent.fade_time);
    const uint16_t tc_step = FadeEngine_TcStep(state->persistent.fade_time);

    if (state->actual_level != state->target_level) {
        const uint8_t diff = abs_diff_u8(state->actual_level, state->target_level);
        const uint8_t step = (diff < level_step) ? diff : level_step;
        state->actual_level = (state->actual_level < state->target_level)
            ? (uint8_t)(state->actual_level + step)
            : (uint8_t)(state->actual_level - step);
        changed = true;
    }

    if (state->actual_tc_mirek != state->target_tc_mirek) {
        const uint16_t diff = abs_diff_u16(state->actual_tc_mirek, state->target_tc_mirek);
        const uint16_t step = (diff < tc_step) ? diff : tc_step;
        state->actual_tc_mirek = (state->actual_tc_mirek < state->target_tc_mirek)
            ? (uint16_t)(state->actual_tc_mirek + step)
            : (uint16_t)(state->actual_tc_mirek - step);
        changed = true;
    }

    if (changed) {
        state->status_bits |= DALI_STATUS_FADE_RUNNING;
    } else {
        state->status_bits &= (uint8_t)~DALI_STATUS_FADE_RUNNING;
    }

    return changed;
}

FadeSnapshot FadeEngine_Snapshot(const DaliState *state)
{
    FadeSnapshot snapshot;
    snapshot.actual_level = state->actual_level;
    snapshot.target_level = state->target_level;
    snapshot.actual_tc_mirek = state->actual_tc_mirek;
    snapshot.target_tc_mirek = state->target_tc_mirek;
    return snapshot;
}
