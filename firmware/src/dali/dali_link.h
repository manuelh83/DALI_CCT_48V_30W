#ifndef DALI_LINK_H
#define DALI_LINK_H

#include <stdbool.h>
#include <stdint.h>

#include "dali_state.h"

void DaliLink_Init(DaliState *state);
void DaliLink_ReceiveFrame(DaliState *state, uint16_t frame);
bool DaliLink_PopFrame(DaliState *state, uint16_t *frame);
void DaliLink_SetResponse(DaliState *state, uint8_t response);
bool DaliLink_GetResponse(DaliState *state, uint8_t *response);

#endif
