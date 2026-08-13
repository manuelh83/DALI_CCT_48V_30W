#include "dali_link.h"

void DaliLink_Init(DaliState *state)
{
    if (state != 0) {
        state->frame_received = false;
        state->has_response = false;
        state->pending_frame = 0U;
        state->response = 0U;
    }
}

void DaliLink_ReceiveFrame(DaliState *state, uint16_t frame)
{
    if (state != 0) {
        state->pending_frame = frame;
        state->frame_received = true;
    }
}

bool DaliLink_PopFrame(DaliState *state, uint16_t *frame)
{
    if ((state == 0) || (frame == 0) || !state->frame_received) {
        return false;
    }

    *frame = state->pending_frame;
    state->frame_received = false;
    return true;
}

void DaliLink_SetResponse(DaliState *state, uint8_t response)
{
    if (state != 0) {
        state->response = response;
        state->has_response = true;
    }
}

bool DaliLink_GetResponse(DaliState *state, uint8_t *response)
{
    if ((state == 0) || (response == 0) || !state->has_response) {
        return false;
    }

    *response = state->response;
    state->has_response = false;
    return true;
}
