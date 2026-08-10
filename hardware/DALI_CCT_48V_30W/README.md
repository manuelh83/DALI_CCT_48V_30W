# DALI CCT 48V 30W – Hardware Reference Design Rev A

> **⚠️ WARNING – REFERENCE DESIGN ONLY**
>
> This is an engineering reference design. It **must not be manufactured or deployed** without:
> - Independent schematic review and simulation
> - Prototype build and validation
> - Thermal design verification
> - EMC testing per applicable standards (e.g., EN 55015 / CISPR 15)
> - Electrical safety assessment (e.g., IEC 62368-1 / EN 61347-2-13)
> - DALI-2 conformance testing (DIIA/DALI Alliance test suites)
> - All applicable regulatory approvals for target market

---

## Project Scope

A production-oriented KiCad 8+ reference design for a **48 V SELV, 30 W maximum, DALI-2 DT8 Tunable White LED driver** with the following key features:

| Parameter | Value |
|---|---|
| Input voltage | 48 V DC SELV, operating range 43.2–52.8 V |
| Transient protection | Designed for short transients up to 60 V |
| LED output | 30 W max total; three terminals LED+, WW−, CW− (common-anode CCT) |
| LED string range | 28–42 V per channel |
| Max current per channel | 700 mA (firmware limits total power to ≤30 W) |
| Dimming | 0.1–100%, continuous/analog current control, no low-frequency PWM |
| DALI | DALI-2 control gear, DT8 Tc Tunable White (IEC 62386-101/-102/-209) |
| PCB size | 150 × 60 mm, 4 layers |
| Component height | ≤30 mm |
| Thermal | Aluminium enclosure coupling via B.Cu thermal zone + thermal vias |

---

## Architecture

```
48V DC SELV IN
    │
    ├── [J1] 48V_IN+/48V_IN− screw terminal (5.08mm)
    │
    ├── BLOCK 1: Input Protection
    │     F1: Fuse 1A slow-blow
    │     D1: P-channel MOSFET reverse-polarity protection
    │     TV1: TVS 60V/600W bidirectional clamp
    │     C1,C2: 100µF/63V bulk caps + EMI filter
    │
    ├── BLOCK 4: Auxiliary Supply 48V→5V→3.3V
    │     U_AUX1: LMR14020SDDA synchronous buck (5V/2A)
    │     U_LDO: AP2112K-3.3 LDO (3.3V/600mA)
    │
    ├── BLOCK 2: 4-Switch Synchronous Buck-Boost LED Bus
    │     U_BB: LT8390A controller, Q1-Q4 MOSFETs, L2 47µH
    │     Output: LED_BUS+ nominally 44V
    │     → [J3-pin1] LED+ (common anode)
    │
    ├── BLOCK 3: Two Linear Low-Side Current Sinks
    │     Q_WW + R_SENSE_WW + U_CS_WW (OPA2333) → [J3-pin2] WW−
    │     Q_CW + R_SENSE_CW + U_CS_CW (OPA2333) → [J3-pin3] CW−
    │     Hardware OCP via LM393 comparator (threshold ~1.05A)
    │     Analog current setpoint from MCU DAC (MCP4728 16-bit I2C)
    │
    ├── BLOCK 5: MCU, NVM, SWD
    │     U_MCU: STM32G031K8T6 (ARM Cortex-M0+, 64KB flash)
    │     U_EEP: AT24C32E I2C EEPROM (DALI NVM)
    │     U_DAC: MCP4728 16-bit quad DAC (current setpoints)
    │     [J4] SWD programming header (1.27mm)
    │
    └── BLOCK 6: Galvanically Isolated DALI Interface
          Isolation goal: 2.5kVrms (subject to safety standard review)
          U_DALI: NXP UBA2015 or optocoupler-based frontend
          D_ESD: PRTR5V0U2X ESD protection
          [J2] DALI_A/DALI_B screw terminal (5.08mm)
```

### Power Architecture Note

This design uses a **common regulated 4-switch synchronous buck-boost LED bus** (nominally 44 V) followed by **two independently controlled, linear low-side constant-current sinks** (WW and CW). This is the correct representation: a single regulated voltage bus with common LED anode, and individual current control on the low side per channel.

---

## Opening in KiCad 8+

1. Open KiCad 8.0 or later.
2. File → Open Project → select `DALI_CCT_48V_30W.kicad_pro`.
3. Open the schematic editor: `DALI_CCT_48V_30W.kicad_sch`.
4. Open the PCB editor: `DALI_CCT_48V_30W.kicad_pcb`.

### ERC/DRC Status

This reference design file contains schematic text-block descriptions of the circuit. A full symbol-and-wire netlist with footprint assignments is required before KiCad ERC/DRC can produce meaningful results. The schematic `kicad_sch` file uses annotation text blocks to describe all circuits; converting these to full KiCad symbols is the first engineering task.

**Intentional open items / known DRC violations:**
- Routing is incomplete; ratsnest will show unconnected nets
- Footprint assignments are placeholders for connector/mounting-hole symbols
- No full component library links; see `docs/bom.csv` for part numbers

See `docs/open-items-and-risks.md` for full list of open items.

---

## Directory Structure

```
hardware/DALI_CCT_48V_30W/
├── DALI_CCT_48V_30W.kicad_pro      KiCad 8 project file
├── DALI_CCT_48V_30W.kicad_sch      Schematic (functional block descriptions + net list)
├── DALI_CCT_48V_30W.kicad_pcb      PCB layout (board outline, layer setup, key placements)
├── README.md                        This file
└── docs/
    ├── design-specification.md      Full electrical specification
    ├── bom.csv                      Bill of Materials
    ├── net-class-and-layout-rules.md  PCB routing rules and net classes
    ├── bring-up-and-test-plan.md    Prototype bring-up sequence
    ├── open-items-and-risks.md      Open design items and risks
    └── firmware-interface.md        MCU/firmware interface documentation
```

---

## Practical Power and Current Limits

| Condition | WW Current | CW Current | Total Power |
|---|---|---|---|
| Full warm white (2700K) | 700 mA | 0 mA | ≤28 W at 40V LED |
| Full cool white (6500K) | 0 mA | 700 mA | ≤28 W at 40V LED |
| Neutral white (4000K) | ~350 mA | ~350 mA | ≤25 W typical |
| Firmware hard limit | | | 30 W total |
| Hardware OCP per channel | 1.05 A | 1.05 A | — |

The firmware enforces `P_total = V_LED_BUS × (I_WW + I_CW) ≤ 30 W` using ADC-measured bus voltage and DAC-controlled current.

---

## License / Disclaimer

This reference design is provided as-is for engineering reference purposes only. No warranty of fitness for any purpose is expressed or implied. The designer and contributors accept no liability for any direct or indirect damages arising from use of this material. All component datasheets, safety standards, and certification requirements must be independently verified by a qualified engineer before manufacture.
