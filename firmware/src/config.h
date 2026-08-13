#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FW_VERSION_STRING "0.1.0"
#define FW_DEVICE_TYPE_DT8 0x08U
#define FW_DALI_UNASSIGNED_ADDRESS 0xFFU
#define FW_DALI_MAX_SCENES 16U
#define FW_DALI_RESET_TC_MIREK 261U
#define FW_DALI_TC_COOL_DEFAULT 153U
#define FW_DALI_TC_WARM_DEFAULT 370U
#define FW_DALI_MAX_LEVEL 254U
#define FW_DALI_MIN_LEVEL 1U
#define FW_DALI_LAST_ACTIVE 0xFFU
#define FW_FADE_TICK_MS 10U
#define FW_STATE_STORE_DEBOUNCE_MS 5000U
#define FW_BUS_LOSS_TIMEOUT_MS 500U
#define FW_HALF_BIT_US 416U
#define FW_I_MAX_MA 600.0f
#define FW_I_THERMAL_MA 480.0f
#define FW_POWER_LIMIT_W 28.0f
#define FW_FIXED_BUS_VOLTAGE 44.0f
#define FW_INPUT_UV_V 43.0f
#define FW_INPUT_UV_RECOVER_V 44.0f
#define FW_INPUT_OV_V 55.0f
#define FW_INPUT_OV_RECOVER_V 53.0f
#define FW_TEMP_WARN_C 70.0f
#define FW_TEMP_WARN_CLEAR_C 60.0f
#define FW_TEMP_SHUTDOWN_C 85.0f
#define FW_TEMP_SHUTDOWN_CLEAR_C 75.0f
#define FW_STATUS_LED_BLINK_MS 500U
#define FW_EEPROM_SIZE 4096U
#define FW_EEPROM_RETRIES 3U
#define FW_DYNAMIC_SLOT_COUNT 64U
#define FW_DYNAMIC_SLOT_SIZE 8U

typedef enum {
    ADC_CHANNEL_NTC = 0,
    ADC_CHANNEL_VIN,
    ADC_CHANNEL_VBUS,
    ADC_CHANNEL_COUNT
} AdcChannel;

typedef struct {
    float ww_ma;
    float cw_ma;
} ChannelCurrents;

typedef enum {
    FAULT_NONE = 0,
    FAULT_OCP_WW = 1u << 0,
    FAULT_OCP_CW = 1u << 1,
    FAULT_TEMP_WARN = 1u << 2,
    FAULT_TEMP_SHUTDOWN = 1u << 3,
    FAULT_INPUT_UV = 1u << 4,
    FAULT_INPUT_OV = 1u << 5,
    FAULT_I2C_DAC = 1u << 6,
    FAULT_NVM = 1u << 7
} FaultFlag;

#endif
