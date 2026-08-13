#ifndef PROTECTION_H
#define PROTECTION_H

#include <stdbool.h>

#include "config.h"
#include "dali_state.h"

typedef struct {
    float current_limit_ma;
    bool buck_enable;
    bool ww_enable;
    bool cw_enable;
} ProtectionDecision;

void Protection_Init(void);
ProtectionDecision Protection_Monitor(DaliState *state, float vin_v, float vbus_v, float temperature_c, bool ocp_ww, bool ocp_cw);

#endif
