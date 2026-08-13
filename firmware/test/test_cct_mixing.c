#include "unity.h"

#include "control/cct_mixing.h"

int main(void)
{
    ChannelCurrents cool = CctMixing_Compute(153U, 153U, 370U, 600.0f, 44.0f, 28.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, cool.ww_ma);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 600.0f, cool.cw_ma);

    ChannelCurrents warm = CctMixing_Compute(370U, 153U, 370U, 600.0f, 44.0f, 28.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 600.0f, warm.ww_ma);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, warm.cw_ma);

    ChannelCurrents mid = CctMixing_Compute(261U, 153U, 370U, 600.0f, 44.0f, 28.0f);
    TEST_ASSERT_TRUE(mid.ww_ma > 290.0f && mid.ww_ma < 310.0f);
    TEST_ASSERT_TRUE(mid.cw_ma > 290.0f && mid.cw_ma < 310.0f);
    return 0;
}
