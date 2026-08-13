#include "board_init.h"

#include "control/current_sink.h"
#include "dali_application.h"
#include "fault/protection.h"
#include "hal_adc.h"
#include "hal_dac_i2c.h"
#include "hal_eeprom_i2c.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_timer.h"
#include "hal_usart.h"
#include "nvm/nvm_handler.h"
#include "scheduler.h"

void BoardInit(DaliState *state)
{
    Scheduler_Init();
    HalGpio_Init();
    HalAdc_Init();
    HalI2c_Init();
    HalDac_Init();
    HalEeprom_Init();
    HalUsart_Init();
    CurrentSink_Init();
    Protection_Init();
    DaliApplication_Init(state);
    (void)NvmHandler_Load(state);
}

void StatusLedUpdate(void)
{
    static bool led_on;
    led_on = !led_on;
    HalGpio_WriteStatusLed(led_on);
}
