#ifndef WEAR_LEVELING_H
#define WEAR_LEVELING_H

#include <stdbool.h>
#include <stdint.h>

#include "dali_state.h"

bool WearLeveling_Load(DaliDynamicState *dynamic_state);
bool WearLeveling_Store(const DaliDynamicState *dynamic_state);

#endif
