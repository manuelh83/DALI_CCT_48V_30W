#ifndef DALI_PHYSICAL_H
#define DALI_PHYSICAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

size_t DaliPhysical_EncodeManchester(uint16_t frame, uint8_t *symbols, size_t capacity);
bool DaliPhysical_DecodeManchester(const uint8_t *symbols, size_t count, uint16_t *frame_out);

#endif
