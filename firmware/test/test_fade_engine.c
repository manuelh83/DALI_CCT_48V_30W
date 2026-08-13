#include "unity.h"

#include "control/fade_engine.h"
#include "dali/dali_application.h"

int main(void)
{
    DaliState state;
    DaliApplication_Init(&state);
    state.persistent.fade_time = 5U;
    state.actual_level = 0U;
    state.target_level = 24U;
    state.actual_tc_mirek = 200U;
    state.target_tc_mirek = 224U;

    TEST_ASSERT_TRUE(FadeEngine_Update(&state));
    TEST_ASSERT_EQUAL_UINT(6U, state.actual_level);
    TEST_ASSERT_EQUAL_UINT(224U, state.actual_tc_mirek);

    TEST_ASSERT_TRUE(FadeEngine_Update(&state));
    TEST_ASSERT_EQUAL_UINT(12U, state.actual_level);
    return 0;
}
