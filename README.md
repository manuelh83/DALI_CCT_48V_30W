# DALI CCT 48V 30W

DALI-2 DT8 Tunable White LED driver reference design for 48V DC SELV input, 30W maximum LED output.

> **⚠️ Reference design only – see hardware project README for critical disclaimers.**

## Hardware Project

→ **[hardware/DALI_CCT_48V_30W/](hardware/DALI_CCT_48V_30W/README.md)**

KiCad 8+ project files, complete BOM, and engineering documentation for a 150 × 60 mm, 4-layer PCB featuring:

- 48V DC SELV input (43.2–52.8V operating, 60V transient protection)
- 30W max LED output via three terminals: `LED+`, `WW−`, `CW−` (common-anode CCT)
- 4-switch synchronous buck-boost (LT8390A) → 44V regulated LED bus
- Two linear low-side constant-current sinks (0–700 mA per channel, analog control)
- Analog/continuous current dimming: 0.1–100%, no low-frequency PWM
- Galvanically isolated DALI-2 DT8 Tc interface (2.5kVrms goal)
- STM32G031K8T6 MCU + AT24C32E EEPROM NVM
- Aluminium enclosure thermal coupling (B.Cu thermal zone + thermal vias)

## Documentation

| Document | Description |
|---|---|
| [hardware/DALI_CCT_48V_30W/README.md](hardware/DALI_CCT_48V_30W/README.md) | Project overview, architecture, opening instructions |
| [docs/design-specification.md](hardware/DALI_CCT_48V_30W/docs/design-specification.md) | Full electrical specification |
| [docs/bom.csv](hardware/DALI_CCT_48V_30W/docs/bom.csv) | Bill of Materials with MPN, ratings, alternates |
| [docs/net-class-and-layout-rules.md](hardware/DALI_CCT_48V_30W/docs/net-class-and-layout-rules.md) | PCB layer stackup, net classes, routing rules |
| [docs/bring-up-and-test-plan.md](hardware/DALI_CCT_48V_30W/docs/bring-up-and-test-plan.md) | Prototype bring-up sequence and acceptance criteria |
| [docs/open-items-and-risks.md](hardware/DALI_CCT_48V_30W/docs/open-items-and-risks.md) | Open design items and risk register |
| [docs/firmware-interface.md](hardware/DALI_CCT_48V_30W/docs/firmware-interface.md) | MCU pin roles, DALI protocol/DT8 implementation, NVM, fault handling |
