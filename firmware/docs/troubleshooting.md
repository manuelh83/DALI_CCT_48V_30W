# Troubleshooting

## Build Problems

- Missing `arm-none-eabi-gcc`: use the host build for tests, then install the ARM toolchain for `firmware.hex` generation.
- Missing STM32CubeG0: initialise the submodule only if you need the vendor headers and HAL in an IDE.

## Runtime Checks

- If output never enables, inspect VIN scaling and thermal shutdown thresholds first.
- If Tc commands appear ignored, confirm the configured Tc bounds and DTR mirror values.
- If last state is not restored, inspect EEPROM bytes starting at `0x40` for the rotating valid marker `0xA5`.

## Bring-Up Hints

- Inject OCP through `HalGpio_InjectOcp()` during host simulation.
- Seed ADC channels with `HalAdc_SetRaw()` before running the scheduler on host.
