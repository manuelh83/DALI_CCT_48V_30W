#ifndef DALI_DEVICE_TYPE_H
#define DALI_DEVICE_TYPE_H

#include <stdint.h>

#include "dali_state.h"

uint16_t DaliDeviceType_ClampTc(const DaliState *state, uint16_t tc_mirek);
void DaliDeviceType_SetTc(DaliState *state, uint16_t tc_mirek);

#endif
