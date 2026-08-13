#ifndef DALI_STATE_H
#define DALI_STATE_H

#include "config.h"

typedef struct {
    uint8_t short_address;
    uint16_t group_mask;
    uint8_t scene_levels[FW_DALI_MAX_SCENES];
    uint16_t scene_tcs[FW_DALI_MAX_SCENES];
    uint8_t max_level;
    uint8_t min_level;
    uint8_t power_on_level;
    uint8_t system_failure_level;
    uint8_t fade_time;
    uint8_t fade_rate;
    uint8_t physical_min_level;
    uint8_t device_type;
    uint16_t tc_cool_mirek;
    uint16_t tc_warm_mirek;
} DaliPersistentState;

typedef struct {
    uint8_t last_arc_level;
    uint16_t last_tc_mirek;
    uint8_t valid_marker;
} DaliDynamicState;

typedef struct {
    DaliPersistentState persistent;
    DaliDynamicState dynamic;
    uint8_t actual_level;
    uint8_t target_level;
    uint16_t actual_tc_mirek;
    uint16_t target_tc_mirek;
    uint8_t status_bits;
    uint8_t dtr0;
    uint8_t dtr1;
    uint16_t dtr_tc_mirek;
    uint32_t random_address;
    bool initialise_enabled;
    bool withdrawn;
    bool state_changed;
    bool frame_received;
    bool has_response;
    uint16_t pending_frame;
    uint8_t response;
    uint32_t last_valid_rx_ms;
    uint32_t last_state_store_ms;
    uint8_t ocp_retry_ww;
    uint8_t ocp_retry_cw;
} DaliState;

enum {
    DALI_STATUS_BALLAST_FAILURE = 1u << 0,
    DALI_STATUS_LAMP_FAILURE = 1u << 1,
    DALI_STATUS_ARC_POWER_ON = 1u << 2,
    DALI_STATUS_LIMIT_ERROR = 1u << 3,
    DALI_STATUS_FADE_RUNNING = 1u << 4,
    DALI_STATUS_RESET_STATE = 1u << 5,
    DALI_STATUS_MISSING_SHORT_ADDRESS = 1u << 6,
    DALI_STATUS_POWER_FAILURE = 1u << 7
};

void DaliState_Init(DaliState *state);

#endif
