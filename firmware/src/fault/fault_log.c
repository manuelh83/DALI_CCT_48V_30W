#include "fault_log.h"

static uint32_t g_fault_mask;

void FaultLog_Clear(void)
{
    g_fault_mask = 0U;
}

void FaultLog_Set(FaultFlag fault)
{
    g_fault_mask |= (uint32_t)fault;
}

void FaultLog_Reset(FaultFlag fault)
{
    g_fault_mask &= ~((uint32_t)fault);
}

bool FaultLog_IsSet(FaultFlag fault)
{
    return (g_fault_mask & (uint32_t)fault) != 0U;
}

uint32_t FaultLog_GetMask(void)
{
    return g_fault_mask;
}
