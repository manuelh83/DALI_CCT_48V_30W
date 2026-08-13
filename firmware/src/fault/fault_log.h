#ifndef FAULT_LOG_H
#define FAULT_LOG_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

void FaultLog_Clear(void);
void FaultLog_Set(FaultFlag fault);
void FaultLog_Reset(FaultFlag fault);
bool FaultLog_IsSet(FaultFlag fault);
uint32_t FaultLog_GetMask(void);

#endif
