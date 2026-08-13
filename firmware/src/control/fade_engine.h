#ifndef FADE_ENGINE_H
#define FADE_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

#include "dali_state.h"

typedef struct {
    uint8_t actual_level;
    uint8_t target_level;
    uint16_t actual_tc_mirek;
    uint16_t target_tc_mirek;
} FadeSnapshot;

void FadeEngine_Init(DaliState *state);
bool FadeEngine_Update(DaliState *state);
uint8_t FadeEngine_LevelStep(uint8_t fade_time);
uint16_t FadeEngine_TcStep(uint8_t fade_time);
FadeSnapshot FadeEngine_Snapshot(const DaliState *state);

#endif
