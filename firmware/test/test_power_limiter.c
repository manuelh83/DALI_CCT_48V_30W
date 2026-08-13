#include "unity.h"

#include "control/power_limiter.h"

int main(void)
{
    ChannelCurrents currents = {300.0f, 300.0f};
    PowerLimiter_Apply(44.0f, 28.0f, &currents);
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 300.0f, currents.ww_ma);
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 300.0f, currents.cw_ma);

    currents.ww_ma = 400.0f;
    currents.cw_ma = 400.0f;
    PowerLimiter_Apply(44.0f, 28.0f, &currents);
    TEST_ASSERT_TRUE((currents.ww_ma + currents.cw_ma) < 640.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, currents.ww_ma, currents.cw_ma);
    return 0;
}
