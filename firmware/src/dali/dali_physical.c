#include "dali_physical.h"

size_t DaliPhysical_EncodeManchester(uint16_t frame, uint8_t *symbols, size_t capacity)
{
    if ((symbols == 0) || (capacity < 32U)) {
        return 0U;
    }

    for (size_t bit = 0; bit < 16U; ++bit) {
        const uint8_t value = (uint8_t)((frame >> (15U - bit)) & 0x1U);
        symbols[(bit * 2U)] = (uint8_t)(value ? 0U : 1U);
        symbols[(bit * 2U) + 1U] = (uint8_t)(value ? 1U : 0U);
    }
    return 32U;
}

bool DaliPhysical_DecodeManchester(const uint8_t *symbols, size_t count, uint16_t *frame_out)
{
    if ((symbols == 0) || (frame_out == 0) || (count != 32U)) {
        return false;
    }

    uint16_t frame = 0U;
    for (size_t bit = 0; bit < 16U; ++bit) {
        const uint8_t first = symbols[bit * 2U];
        const uint8_t second = symbols[(bit * 2U) + 1U];
        if ((first == 1U) && (second == 0U)) {
            frame <<= 1U;
        } else if ((first == 0U) && (second == 1U)) {
            frame = (uint16_t)((frame << 1U) | 1U);
        } else {
            return false;
        }
    }

    *frame_out = frame;
    return true;
}
