# API Summary

## HAL

- `hal_gpio` – enable pins, OCP inputs, status LEDs
- `hal_adc` – 12-bit sampled inputs with 10-sample moving average
- `hal_i2c` – host-visible I2C backing store for DAC/EEPROM wrappers
- `hal_dac_i2c` – MCP4728 code cache and write helper
- `hal_eeprom_i2c` – AT24C32E-compatible byte store
- `hal_usart` – circular RX/TX buffers for DALI transport experiments
- `hal_timer` – 10 ms scheduler tick source

## DALI

- `dali_physical` – Manchester encode/decode helpers
- `dali_link` – frame queue and response register
- `dali_application` – level, scene, configuration, query, and DT8 command handling
- `dali_device_type` – Tc clamping for DT8 tunable white control

## Control

- `cct_mixing` – Tc to WW/CW current split with power limiting
- `power_limiter` – 28 W total output guard
- `fade_engine` – simultaneous dim and Tc fade progression
- `current_sink` – current-to-DAC mapping and output enable decisions

## NVM / Faults / System

- `nvm_handler` / `wear_leveling` – static config + rotating last-state storage
- `protection` / `fault_log` / `ntc_thermistor` – thermal, input, and OCP behavior
- `scheduler` / `board_init` / `debug` – integration helpers
