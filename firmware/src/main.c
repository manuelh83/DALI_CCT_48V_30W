#include <stdbool.h>
#include <stdint.h>

#include "config.h"
#include "control/current_sink.h"
#include "control/fade_engine.h"
#include "dali/dali_application.h"
#include "dali/dali_link.h"
#include "fault/ntc_thermistor.h"
#include "fault/protection.h"
#include "hal/hal_adc.h"
#include "hal/hal_dac_i2c.h"
#include "hal/hal_gpio.h"
#include "nvm/nvm_handler.h"
#include "system/board_init.h"
#include "system/scheduler.h"

#ifndef HOST_BUILD
void SystemInit(void);
#else
void SystemInit(void)
{
}
#endif

static void DaliRx(DaliState *state)
{
    (void)state;
}

int main(void)
{
    DaliState state;
    SystemInit();
    BoardInit(&state);
    FadeEngine_Init(&state);

#ifdef HOST_BUILD
    const uint32_t max_iterations = 8U;
    for (uint32_t iteration = 0U; iteration < max_iterations; ++iteration) {
#else
    while (1) {
#endif
        WaitForTick();
        (void)AdcRead(ADC_CHANNEL_NTC);
        DaliRx(&state);

        uint16_t frame = 0U;
        if (DaliLink_PopFrame(&state, &frame)) {
            DaliApplication_Process(&state, frame, Scheduler_NowMs());
        }

        (void)FadeEngine_Update(&state);

        const float vin_v = HalAdc_ToVoltage(ADC_CHANNEL_VIN, AdcRead(ADC_CHANNEL_VIN));
        const float vbus_v = HalAdc_ToVoltage(ADC_CHANNEL_VBUS, AdcRead(ADC_CHANNEL_VBUS));
        const float temperature_c = NtcThermistor_AdcToCelsius(AdcRead(ADC_CHANNEL_NTC));
        const ProtectionDecision decision = Protection_Monitor(
            &state,
            vin_v,
            vbus_v,
            temperature_c,
            HalGpio_ReadOcpWw(),
            HalGpio_ReadOcpCw());

        const CurrentSinkOutput sink = CurrentSink_Update(&state, decision.current_limit_ma);
        HalGpio_SetBuckEnable(decision.buck_enable);
        HalGpio_SetEnWw(decision.ww_enable && sink.enable_ww);
        HalGpio_SetEnCw(decision.cw_enable && sink.enable_cw);
        (void)HalDac_WriteChannels(sink.dac_ww, sink.dac_cw);

        StatusLedUpdate();
        if (state.state_changed && Scheduler_TimerExpired(state.last_state_store_ms, FW_STATE_STORE_DEBOUNCE_MS)) {
            (void)NvmHandler_SaveDynamic(&state);
            state.state_changed = false;
        }
    }

    return 0;
}
