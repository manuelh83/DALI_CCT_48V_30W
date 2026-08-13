#ifndef CURRENT_SINK_H
#define CURRENT_SINK_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "dali_state.h"

typedef struct {
    ChannelCurrents currents;
    uint16_t dac_ww;
    uint16_t dac_cw;
    bool enable_ww;
    bool enable_cw;
} CurrentSinkOutput;

void CurrentSink_Init(void);
CurrentSinkOutput CurrentSink_Update(const DaliState *state, float current_limit_ma);
uint16_t CurrentSink_CurrentToCode(float current_ma, float current_limit_ma);

#endif
