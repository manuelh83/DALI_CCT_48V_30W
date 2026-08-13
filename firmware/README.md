# Firmware

Production-oriented STM32G031 firmware skeleton and host-testable logic for the DALI CCT 48V 30W reference design.

## Contents

- `src/` – HAL wrappers, DALI stack, control logic, NVM, protection, scheduler
- `test/` – host-side unit tests for the critical control and parser logic
- `docs/` – build, API, DALI, and troubleshooting notes
- `lib/` – optional STM32CubeG0 integration hook

See `docs/build.md` for build and flash instructions.
