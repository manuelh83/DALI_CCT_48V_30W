# DALI Implementation Notes

The firmware models a DALI control gear with DT8 tunable-white behavior and persistent last-state storage.

## Implemented Behaviors

- direct arc power level updates
- OFF / MIN / MAX / step up / step down
- scene recall and scene query hooks
- min/max/power-on/system-failure/fade parameter storage through DTR-backed writes
- initialise / randomise / compare / withdraw / program short address / verify short address / query short address
- DT8 Tc set/query and configurable cool/warm bounds
- status and actual level queries

## Timing Model

- 1200 baud DALI framing helpers
- 416 µs half-bit Manchester symbol model
- 10 ms scheduler fade updates
- 5 s debounce before writing dynamic state to EEPROM slots

## Persistence Model

- static DALI config stored at `0x00..0x3E`
- dynamic last-state ring stored at `0x40..0x23F` as 64 sequence-tagged slots
