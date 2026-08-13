#ifndef NVM_HANDLER_H
#define NVM_HANDLER_H

#include <stdbool.h>

#include "dali_state.h"

bool NvmHandler_Load(DaliState *state);
bool NvmHandler_SavePersistent(const DaliState *state);
bool NvmHandler_SaveDynamic(DaliState *state);

#endif
