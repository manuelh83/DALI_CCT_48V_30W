#include "unity.h"

#include "dali/dali_application.h"
#include "hal/hal_eeprom_i2c.h"
#include "nvm/nvm_handler.h"

int main(void)
{
    DaliState state;
    DaliState restored;

    HalEeprom_Init();
    DaliApplication_Init(&state);

    for (uint32_t i = 0U; i < 65U; ++i) {
        state.actual_level = (uint8_t)(10U + i);
        state.actual_tc_mirek = (uint16_t)(200U + i);
        TEST_ASSERT_TRUE(NvmHandler_SaveDynamic(&state));
    }

    DaliApplication_Init(&restored);
    TEST_ASSERT_TRUE(NvmHandler_Load(&restored));
    TEST_ASSERT_EQUAL_UINT(state.dynamic.sequence, restored.dynamic.sequence);
    TEST_ASSERT_EQUAL_UINT(state.actual_level, restored.actual_level);
    TEST_ASSERT_EQUAL_UINT(state.actual_tc_mirek, restored.actual_tc_mirek);
    return 0;
}
