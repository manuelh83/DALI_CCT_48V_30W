#include "wear_leveling.h"

#include <string.h>

#include "eeprom_driver.h"
#include "nvm_layout.h"

static uint16_t slot_address(uint8_t slot)
{
    return (uint16_t)(NVM_DYNAMIC_PAGE_ADDRESS + ((uint16_t)slot * NVM_DYNAMIC_SLOT_SIZE));
}

bool WearLeveling_Load(DaliDynamicState *dynamic_state)
{
    DaliDynamicState candidate = {0};
    bool found = false;
    for (uint8_t slot = 0U; slot < NVM_DYNAMIC_SLOT_COUNT; ++slot) {
        DaliDynamicState temp = {0};
        if (EepromDriver_Read(slot_address(slot), (uint8_t *)&temp, sizeof(temp)) && (temp.valid_marker == 0xA5U)) {
            candidate = temp;
            found = true;
        }
    }
    if (found) {
        *dynamic_state = candidate;
    }
    return found;
}

bool WearLeveling_Store(const DaliDynamicState *dynamic_state)
{
    DaliDynamicState current = {0};
    uint8_t next_slot = 0U;
    for (uint8_t slot = 0U; slot < NVM_DYNAMIC_SLOT_COUNT; ++slot) {
        DaliDynamicState temp = {0};
        if (EepromDriver_Read(slot_address(slot), (uint8_t *)&temp, sizeof(temp)) && (temp.valid_marker == 0xA5U)) {
            current = temp;
            next_slot = (uint8_t)((slot + 1U) % NVM_DYNAMIC_SLOT_COUNT);
        }
    }
    (void)current;
    return EepromDriver_Write(slot_address(next_slot), (const uint8_t *)dynamic_state, sizeof(*dynamic_state));
}
