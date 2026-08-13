#include "unity.h"

#include "dali/dali_application.h"
#include "dali/dali_link.h"

static uint16_t command_frame(uint8_t short_address, uint8_t command)
{
    return (uint16_t)((((short_address << 1U) | 1U) << 8U) | command);
}

static uint16_t dapc_frame(uint8_t short_address, uint8_t level)
{
    return (uint16_t)(((short_address << 1U) << 8U) | level);
}

static uint16_t broadcast_command_frame(uint8_t command)
{
    return (uint16_t)((0xFFU << 8U) | command);
}

int main(void)
{
    DaliState state;
    uint8_t response = 0U;
    DaliApplication_Init(&state);
    state.initialise_enabled = true;
    state.dtr0 = 23U;
    DaliApplication_Process(&state, broadcast_command_frame(DALI_CMD_PROGRAM_SHORT_ADDRESS), 10U);
    TEST_ASSERT_EQUAL_UINT(23U, state.persistent.short_address);

    DaliApplication_Process(&state, dapc_frame(23U, 120U), 20U);
    TEST_ASSERT_EQUAL_UINT(120U, state.target_level);

    state.dtr_tc_mirek = 300U;
    DaliApplication_Process(&state, command_frame(23U, DALI_CMD_SET_TC), 30U);
    TEST_ASSERT_EQUAL_UINT(300U, state.target_tc_mirek);

    DaliApplication_Process(&state, command_frame(23U, DALI_CMD_QUERY_STATUS), 40U);
    TEST_ASSERT_TRUE(DaliLink_GetResponse(&state, &response));
    TEST_ASSERT_EQUAL_HEX8(state.status_bits, response);
    return 0;
}
