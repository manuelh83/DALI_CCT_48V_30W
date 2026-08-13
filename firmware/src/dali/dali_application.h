#ifndef DALI_APPLICATION_H
#define DALI_APPLICATION_H

#include <stdbool.h>
#include <stdint.h>

#include "dali_state.h"

enum {
    DALI_CMD_OFF = 0x00,
    DALI_CMD_UP = 0x01,
    DALI_CMD_DOWN = 0x02,
    DALI_CMD_STEP_UP = 0x03,
    DALI_CMD_STEP_DOWN = 0x04,
    DALI_CMD_RECALL_MAX = 0x05,
    DALI_CMD_RECALL_MIN = 0x06,
    DALI_CMD_GO_TO_SCENE_0 = 0x10,
    DALI_CMD_SET_MAX_LEVEL = 0x2A,
    DALI_CMD_SET_MIN_LEVEL = 0x2B,
    DALI_CMD_SET_SYSTEM_FAILURE_LEVEL = 0x2C,
    DALI_CMD_SET_POWER_ON_LEVEL = 0x2D,
    DALI_CMD_SET_FADE_TIME = 0x2E,
    DALI_CMD_SET_FADE_RATE = 0x2F,
    DALI_CMD_INITIALISE = 0xA5,
    DALI_CMD_RANDOMISE = 0xA7,
    DALI_CMD_COMPARE = 0xA9,
    DALI_CMD_WITHDRAW = 0xAB,
    DALI_CMD_PROGRAM_SHORT_ADDRESS = 0xB7,
    DALI_CMD_VERIFY_SHORT_ADDRESS = 0xB9,
    DALI_CMD_QUERY_SHORT_ADDRESS = 0xBB,
    DALI_CMD_SET_TC = 0xE0,
    DALI_CMD_QUERY_TC = 0xE1,
    DALI_CMD_SET_TC_COOL = 0xE2,
    DALI_CMD_SET_TC_WARM = 0xE3,
    DALI_CMD_QUERY_TC_COOL = 0xE4,
    DALI_CMD_QUERY_TC_WARM = 0xE5,
    DALI_CMD_QUERY_STATUS = 0x90,
    DALI_CMD_QUERY_CONTROL_GEAR_PRESENT = 0x91,
    DALI_CMD_QUERY_ACTUAL_LEVEL = 0xA0,
    DALI_CMD_QUERY_MAX_LEVEL = 0xA1,
    DALI_CMD_QUERY_MIN_LEVEL = 0xA2,
    DALI_CMD_QUERY_POWER_ON_LEVEL = 0xA3,
    DALI_CMD_QUERY_SYSTEM_FAILURE_LEVEL = 0xA4,
    DALI_CMD_QUERY_FADE_TIME = 0xA6,
    DALI_CMD_QUERY_FADE_RATE = 0xA8,
    DALI_CMD_QUERY_SCENE_LEVEL_0 = 0xB0
};

void DaliApplication_Init(DaliState *state);
void DaliApplication_Process(DaliState *state, uint16_t frame, uint32_t now_ms);
float DaliApplication_LevelToCurrentMa(const DaliState *state, uint8_t level);
bool DaliApplication_AddressMatches(const DaliState *state, uint8_t address_byte);

#endif
