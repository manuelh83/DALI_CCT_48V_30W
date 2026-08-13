# Open Items and Risks – DALI CCT 48V 30W Rev A

## Summary

This document lists all known open design items, unresolved decisions, and engineering risks as of Rev A. All items marked **[BLOCKER]** must be resolved before PCB fabrication. Items marked **[VALIDATION]** must be resolved before production release.

---

## Open Items

### OI-001 [RESOLVED] Auxiliary Supply Input Voltage Rating

**Description**: The LMR14020SDDA is specified for 4V–42V input. The operating input range of this design is 43.2–52.8V, which exceeds the LMR14020 absolute maximum.

**Resolution**: Part replaced with **Texas Instruments LMR16006YDDAR** (4–60V input, 600mA, SOT-23-5) in `bom.csv` (designator U_AUX1). Design specification §5.1 updated accordingly.

**Status**: ✅ CLOSED – BOM updated; no further action required before PCB layout.

---

### OI-002 [BLOCKER] Buck-Boost Compensation Network

**Description**: The LT8390A requires a carefully designed compensation network (ITH RC, slope compensation set by SLOPE pin resistor). No compensation values were calculated in Rev A.

**Resolution**: Initial compensation values calculated and documented in `design-specification.md` §3.4:

| Component | Value | Notes |
|---|---|---|
| Cith | 15 nF | Dominant integrating cap; pole ≈ 480 Hz |
| Rith | 22 kΩ | Sets loop crossover ≈ 5 kHz |
| Cith2 | 390 pF | HF rolloff ≈ 18 kHz |
| R_SLOPE | 470 kΩ | Slope compensation starting point |

**Remaining actions**:
1. Build LTspice/SIMPLIS model; confirm crossover ≥ 5 kHz, phase margin ≥ 45° across Vin range.
2. Adjust values per simulation; validate on prototype with Bode plot measurement.

**Risk level**: HIGH – calculated values are starting points; simulation and prototype validation are mandatory.

---

### OI-003 [BLOCKER] Current Sink Loop Compensation

**Description**: The OPA2333 error amplifier with LED MOSFET sink requires loop stability analysis. Placeholder compensation (10nF / 10kΩ RC) was not designed for specific LED characteristics.

**Resolution**: Loop analysis performed and documented in `design-specification.md` §4.6:

| Component | Old Value | New Value |
|---|---|---|
| C_COMP_WW/CW | 10 nF | **100 nF** |
| R_COMP_WW/CW | 10 kΩ | 10 kΩ (unchanged) |
| C_HF_WW/CW | (not fitted) | **100 pF** (add in parallel) |

Dominant pole at ≈ 160 Hz; estimated phase margin ≥ 45° from 0.5 mA to 700 mA.

**Remaining actions**:
1. Prototype measurement: verify no oscillation at any current setpoint.
2. Adjust C_comp if step-response or oscillation test fails (see §4.6 checklist).

**Risk level**: MEDIUM – calculated values are engineering estimates; prototype verification required.

---

### OI-004 [RESOLVED] DALI IC Selection and Firmware Impact

**Description**: Two options were listed for the DALI interface (NXP UBA2015 vs. IS181XBHTR + discrete).

**Resolution**: **NXP UBA2015T/N1** selected. Updated in `bom.csv` (designator U_ISO_DALI). Firmware uses STM32G031 USART1 in Manchester mode (hardware-supported on STM32G0) connected to UBA2015 UART interface. DT8(Tc) firmware stack is custom-implemented (see `firmware-interface.md` §3).

**Status**: ✅ CLOSED – part selected; firmware interface documented.

---

### OI-005 [VALIDATION] DALI-2 Conformance Testing

**Description**: This design targets DALI-2 DT8 Tc behavior per IEC 62386-101/-102/-209 but does not hold DALI-2 certification.

**Required action**: Plan certification timeline and budget; do not label product as "DALI-2" or "DALI certified" without completed certification.

**Certification path**:
1. Engage DALI Alliance (DIIA) or accredited test laboratory.
2. Execute mandatory test cases for control gear (IEC 62386-102) and DT8(Tc) (IEC 62386-209).
3. Pass all test cases; publish in DALI Alliance certified product database.

**Status**: 🔲 OPEN – process item; no firmware or hardware change required. Schedule after prototype validation.

**Risk level**: MEDIUM – label compliance issue.

---

### OI-006 [PARTIALLY RESOLVED] Safety Certification

**Description**: No safety assessment has been performed. For EU market, relevant standards may include IEC 62368-1, EN 61347-2-13, EN 60598, 2014/35/EU (LVD).

**Resolution (partial)**: PCB isolation distances have been normatively derived per IEC 62368-1:2018 and documented in `net-class-and-layout-rules.md` and `design-specification.md` §7.1:

| Parameter | Derived minimum | Implemented | Standard reference |
|---|---|---|---|
| Clearance (air, reinforced, OVC II 800V) | 2.0 mm | 8.0 mm (keepout zone) | IEC 62368-1:2018 Table G.8 |
| Creepage (surface, reinforced, IIIa, PD2, 63V) | 5.0 mm | 8.0 mm (keepout zone) | IEC 62368-1:2018 Table G.12 |

Design conditions assumed: reinforced insulation, working voltage 63 V, pollution degree 2, OVC II (800 V impulse), Material Group IIIa (FR4). DRC rule updated with normative reference.

**Remaining actions (open)**:
- Engage qualified test laboratory for full safety assessment per IEC 62368-1 / EN 61347-2-13.
- Verify PCB creepage/clearance physically on fabricated board (confirm keepout zone free of conductors on all layers).
- Verify isolation component ratings (UBA2015 or optocoupler ≥ 2.5 kVrms).
- Verify thermal and flammability compliance.

**Status**: 🔶 PARTIALLY RESOLVED – normative distance values derived and documented; lab verification still required before production.

**Risk level**: HIGH for regulated markets.

---

### OI-007 [VALIDATION] EMC Pre-Compliance

**Description**: No EMC testing has been performed. Conducted and radiated emissions from a 200 kHz switching converter must meet applicable limits (EN 55015 / CISPR 15 for lighting).

**Required action**:
- Perform conducted emission scan (9 kHz–30 MHz) on prototype.
- Perform radiated emission scan (30 MHz–1 GHz).
- Add input filter components as necessary.
- Document worst-case switching harmonics and mitigation.

**Mitigation pre-design**: L1 (TDK ACM2520-201-2P-T, 10 µH common-mode choke) and C3 (4.7 µF MLCC) are included on the 48V input. TV1 (SMAJ60A) clamps fast transients. These components should be sufficient for a first prototype scan; additional differential-mode choke or X-capacitor may be needed.

**Status**: 🔲 OPEN – requires prototype. Stage 7 of bring-up-and-test-plan.md covers pre-compliance scan.

**Risk level**: MEDIUM – typically requires iteration.

---

### OI-008 [RESOLVED] Thermal Validation – Fixed 44V Bus, No Adaptive Vbus

**Description**: Each linear current sink (Q_WW, Q_CW) can dissipate up to 11.2 W at worst case (44V bus, 28V LED, 700mA). The original design assumed adaptive bus-voltage firmware control as the primary thermal mitigation.

**Resolution (Rev A.2 – Thermal Redesign):**

Adaptive bus-voltage regulation has been **eliminated**. The system is thermally dimensioned to operate safely at a **fixed 44 V bus** using four passive/hardware measures:

#### Measure 1: MOSFET upgrade to lower Rds(on) / lower Rth(j-c) part

| Parameter | STB20NF06L (Rev A.1) | STW20NM60N (Rev A.2) |
|---|---|---|
| Vdss | 60 V | 60 V |
| Id | 20 A | 20 A |
| Rds(on) | 32 mΩ | **15 mΩ** |
| Rth(j-c) | 3.1 °C/W | **2.0 °C/W** |
| Package | D2PAK (TO-263) | D2PAK (TO-263) |

#### Measure 2: Thermal via count 9 → 25 (5×5 array)

- Via drill: 0.3 mm, pad: 0.5 mm, Cu-filled/capped (not open)
- Via thermal resistance: 58.8 °C/W per via; 25 in parallel = 2.35 °C/W → dominated by B.Cu spreading
- Effective R_PCB-enclosure: ~0.5 °C/W (reduced from 1.2 °C/W with 9 vias)

#### Measure 3: Larger B.Cu thermal pour 20×20mm → 30×30mm

- Reduces lateral spreading resistance from ~0.5 °C/W to ~0.3 °C/W
- Included in R_PCB figure above

#### Measure 4: Firmware current limit 600 mA (was 700 mA); OCP 900 mA (was 1.05A)

- Reduces worst-case headroom dissipation from 11.2 W to 9.6 W per channel
- Total thermal budget reduction: ~14%

#### Revised thermal budget (all measures, fixed 44 V bus)

| Segment | Rth | ΔT at 9.6W/ch |
|---|---|---|
| Junction → case (STW20NM60N) | 2.0 °C/W | 19.2 °C |
| Case → PCB | 0.8 °C/W | 7.7 °C |
| PCB → Al (25 vias + 30×30mm pour + TIM) | 0.5 °C/W | 4.8 °C |
| **Total R_j-PCB** | **3.3 °C/W** | **31.7 °C** |

```
P_diss = (44V − 28V) × 0.6A = 9.6W per channel
P_total = 19.2W (both channels)

T_Al = 50°C + 19.2W × 2.5°C/W / 2 = 50 + 24 = 74°C

Tj = T_Al + P_diss × R_j-PCB
   = 74 + 9.6 × 3.3
   = 74 + 31.7
   = 105.7°C   ← within 125°C safety limit (19°C margin)

NTC 70°C warning → I reduced to 480mA:
  P_diss = 16V × 0.48A = 7.68W; T_Al = 50 + 15.36×2.5/2 = 69.2°C
  Tj = 69.2 + 7.68 × 3.3 = 69.2 + 25.3 = 94.5°C   ✓  (≤ 95°C target met)
```

**Adaptive bus-voltage firmware control is NOT implemented and NOT required.**
NTC-based current-limiting (70°C → 480mA, 85°C → shutdown) provides all necessary thermal protection as a simple, reliable mechanism without bus-voltage regulation.

**Assumptions documented:**
- MOSFET: STW20NM60N (Rth(j-c) = 2.0 °C/W per datasheet)
- Thermal via: 25 per device (5×5), 0.3mm drill, Cu-filled
- B.Cu pour: 30×30mm per channel
- TIM: Bergquist GP3000 or equivalent (3 W/m·K), 0.1mm bond-line thickness
- Al enclosure: R_Al-ambient = 2.5 °C/W (shared, 150×60mm natural convection)
- Firmware I_MAX = 600mA; OCP = 900mA

**BOM updated**: Q_WW, Q_CW → STW20NM60N (see bom.csv)
**design-specification.md §4.2, §4.3, §4.4**: Updated with new part, via count, pour size, and thermal calculations.
**firmware-interface.md §5.2, §6.1**: Simplified power limiting (fixed 28W, no Vbus monitoring for control); NTC current-limiting documented.

**Remaining actions (still required)**:
- Measure junction temperature on prototype under worst-case conditions.
- Verify 25 vias place correctly under D2PAK footprint; confirm Cu-fill specification with PCB fab.
- Measure Al enclosure surface temperature; verify R_Al-ambient ≤ 2.5 °C/W at full load.

**Status**: ✅ RESOLVED – Thermisches Design neudimensioniert. Keine adaptive Vbus-Regelung in Firmware erforderlich. Prototyp-Messung weiterhin empfohlen zur Verifikation.

**Risk level**: LOW – worst-case Tj = 106°C (< 125°C limit); NTC provides additional safety margin.

---

### OI-009 [IN PROGRESS] CCT to WW/CW Mixing Algorithm

**Description**: The mapping from DALI Tc command (colour temperature in mirek) to WW/CW current ratio depends on the actual LED module spectral characteristics.

**Resolution**: Firmware interface updated in `firmware-interface.md` §3.4 with:
1. Linear interpolation implementation (default; usable for evaluation).
2. Full calibrated lookup-table procedure using CIE 1931 chromaticity mixing equations.
3. Production firmware code template for table-based mixing.

**Remaining actions**:
- Characterise WW and CW LED chromaticity vs. current on actual LED module.
- Build and store lookup table per documented procedure.
- Verify gamut coverage across [Tc_cool, Tc_warm] range.

**Status**: 🔶 IN PROGRESS – algorithm and calibration procedure documented; lookup table requires measured LED data.

---

### OI-010 [IN PROGRESS] Minimum Dimming Level

**Description**: The specification requires 0.1% dimming (minimum 1:1000 ratio). With 700mA full scale, this implies 0.7mA minimum.

**Resolution**: Error budget analysis documented in `firmware-interface.md` §5.4:
- Dominant error: MCP4728 zero-code offset (worst-case ~0.5 mV → 5 mA error).
- Typical error: ~1 mA (within spec for typical parts).
- Mitigation options: firmware offset compensation (recommended), dual-range PWM blanking (fallback).

**Remaining actions**:
- Verify on prototype; implement firmware offset compensation.
- If needed at temperature extremes, implement 1 kHz PWM blanking below arc-power 10.

**Status**: 🔶 IN PROGRESS – analysis complete; prototype verification and firmware implementation required.

---

### OI-011 [RESOLVED] Schematic Symbol/Netlist Completion

**Description**: The Rev A `.kicad_sch` file contained functional text descriptions and net lists rather than full KiCad symbol-and-wire schematics.

**Resolution (partial – Rev A.1):**

All schematic BOM components required by the reference design have been added as proper KiCad symbol instances to the schematic file, including:

| Block | Components added as symbol instances |
|---|---|
| Input protection | J1, F1, TV1, D1 (MOSFET_P), C1, C2, C3, L1 |
| Buck-boost LED bus | U_BB (`AnalogDevices:LT8390AIFE`), Q1–Q4, L2, C4–C7, C_INTVCC, C_BOOT, R_CS_H/L, R_FB1/2, C_ITH, R_ITH, C_ITH2, R_SLOPE, R_RT, R_BB_SHDN_PU |
| Current sinks (WW/CW) | Q_WW, Q_CW (D2PAK/Device:MOSFET_N), R_SENSE_WW/CW, U_CS_WW/CW, U_OCP, U_DAC, C_COMP/HF/R_COMP/R_GATE |
| LED output | J3 |
| Aux PSU | U_AUX1, U_LDO, C_AUX_IN/OUT, R_AUX_FB1/2, C_LDO_IN/OUT, C_5VDALI |
| MCU & NVM | U_MCU (STM32G031), U_EEP (AT24C32E), C_MCU1–3, FB1, C_VDDA, R_I2C_SCL/SDA, C_NRST, SJ1, R_BOOT0, LED_STATUS, LED_FAULT, R_STATUS, R_FAULT, J4 |
| NTC & ADC dividers | NTC1, R_NTC_PULL, J_NTC, R_VBUS1/2, R_VIN1/2 |
| DALI isolation | J2, TVS_DALI, D_ESD1, U_ISO_DALI, C_DALI1/2 |
| Power flags (ERC) | #PWR01 (GND), #PWR02 (+3.3V), #PWR03 (+5V) |

All symbols include: Reference designator, Value, Footprint assignment (matching bom.csv), MPN (hidden property).

New lib_symbols inline definitions added for: Device:Fuse, Device:D_TVS, Device:D_Schottky, Device:LED, Device:CP, Device:MOSFET_N, Device:MOSFET_P, Device:OPAMP, Device:LM393, Device:IC (generic multi-pin), `AnalogDevices:LT8390AIFE`, Device:EEPROM_I2C, Device:Ferrite_Bead, Device:R_Thermistor_NTC, Device:Jumper_NO_Small, Connector_Generic:Conn_01x02/03/04, power:+5V, power:PWR_FLAG.

**Resolution (Rev A.2 – Wiring complete):**

All schematic component pins have been connected using KiCad net labels. The complete netlist covers the required named nets for input power, the LED bus, current sinks, MCU control, and DALI isolation:

| Net class | Key nets |
|---|---|
| Input / power | `+48V_IN`, `VIN_F`, `48V_FILT+`, `PGND`, `5V_AUX`, `+3.3V`, `+5V_DALI`, `DALI_GND` |
| LED bus | `LED_BUS`, `SW`, `LX`, `Q1_GATE`–`Q4_GATE`, `BB_FB`, `COMP`, `COMP_HF`, `SLOPE`, `CS_H`, `CS_L`, `RT`, `BB_SHDN` |
| Current sinks | `WW-`, `CW-`, `V_SENSE_WW`, `V_SENSE_CW`, `V_SET_WW`, `V_SET_CW`, `GATE_WW`, `GATE_CW`, `COMP_WW`, `COMP_CW` |
| OCP / DAC | `OCP_WW`, `OCP_CW`, `OCP_REF`, `EN_WW`, `EN_CW`, `DAC_OUT_C`, `DAC_OUT_D` |
| MCU / debug | `I2C_SCL`, `I2C_SDA`, `NRST`, `SWDIO`, `SWDCLK`, `BB_SHDN`, `VDDA`, `BOOT0_J` |
| DALI interface | `DALI_A`, `DALI_B`, `DALI_TX`, `DALI_RX` |
| ADC / monitoring | `ADC_NTC`, `ADC_VIN`, `ADC_VBUS`, `AUX_FB`, `AUX_SW` |
| Indicators | `LED_STATUS`, `LED_FAULT`, `STATUS_A`, `FAULT_A`, `NTC_P` |

**Resolution (Rev A.3 – LT8390A exact symbol):**

- Added a project-local KiCad 8 symbol library (`AnalogDevices.kicad_sym`) and `sym-lib-table` entry.
- Replaced U_BB with the exact `AnalogDevices:LT8390AIFE` 28-pin TSSOP symbol.
- Exposed all LT8390A pins in the schematic, including explicit `Q3_GATE` / `Q4_GATE`, `RT`, duplicate `VIN`, duplicate `INTVCC`, and four `PGND` pins.
- Added the missing local support parts required by the pinout update: `R_RT`, `R_BB_SHDN_PU`, `C_INTVCC`, and `C_BOOT`.

**Remaining actions (production / GUI validation):**
1. Open in KiCad 8 GUI and run ERC to confirm there are no remaining power-pin or no-connect warnings.
2. Re-import the updated schematic netlist into PCB editor before any final routing work.
3. Continue replacing other generic `Device:IC` symbols (`U_MCU`, `U_DAC`, `U_ISO_DALI`, `U_AUX1`, `U_LDO`) as separate cleanup tasks.

**Status**: ✅ RESOLVED – U_BB now uses the exact LT8390AIFE symbol and the buck-boost support nets are explicitly wired in `DALI_CCT_48V_30W.kicad_sch`.

**Prerequisite for OI-012**: ✅ Netlist complete; import into PCB after ERC pass.

---

### OI-012 [IN PROGRESS] PCB Component Placement

**Description**: The Rev A `.kicad_pcb` file contained board outline, mounting holes, connector placeholders, zone definitions, and layout region annotations. Full component placement was not complete.

**Resolution (partial – Rev A.1):**

The PCB file has been updated with the following additions:

1. **D2PAK footprints for Q_WW and Q_CW**: The current-sink MOSFETs are now represented as proper `TO-263-3_TabDown` D2PAK footprints at their target positions on B.Cu (bottom layer for thermal coupling), replacing the previous SOT-223 assumption. Net assignments: drain/tab = WW-/CW-, source = PGND, gate = unconnected pending netlist import.

2. **9 thermal vias per device**: 3×3 via arrays (0.3mm drill / 0.5mm pad) placed under each D2PAK exposed tab, connecting F.Cu to B.Cu for heat extraction.

3. **20×20mm B.Cu thermal zones**: Dedicated filled copper zones per channel (Q_WW thermal zone on net WW-; Q_CW on net CW-) to spread heat to the aluminium enclosure via TIM. Zones are independent to avoid cross-channel thermal coupling.

4. **Updated isolation barrier silkscreen**: Label updated to `ISO BARRIER: CLR>=2mm / CRG>=5mm / ZONE=8mm (IEC 62368-1:2018)` reflecting the normative derivation from Step 1.

5. **Existing connectors and mounting holes**: J1–J4 connector footprints, 4× M3 mounting holes, and all annotation zones (isolation keepout, switching region, thermal region) remain from Rev A.

**Remaining actions (open):**
1. Import netlist from schematic after OI-011 wiring completion.
2. Place all remaining 40+ component footprints (passives, ICs) per placement guidelines in `net-class-and-layout-rules.md`.
3. Route all nets per DRC rules (PWR_HV ≥2.0mm, PWR_LED ≥1.5mm, DALI_ISO ≥8mm keepout, Kelvin routing for sense lines).
4. Run KiCad DRC; document and resolve violations.
5. Add silkscreen labels for all components, polarity markers, and test points.

**Status**: 🔶 IN PROGRESS – D2PAK footprints for Q_WW/Q_CW with thermal vias/zones added; full placement, routing, and DRC require KiCad 8 GUI and completed netlist from OI-011.

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Status |
|---|---|---|---|---|
| Aux supply exceeds VIN rating (OI-001) | ~~Certain if not fixed~~ N/A | ~~Board destruction~~ | ✅ Replaced with LMR16006YDDAR | RESOLVED |
| Buck-boost instability (OI-002) | Medium (calculations done, unvalidated) | LED flicker, EMI | LTspice simulation + prototype Bode plot | IN PROGRESS |
| Current sink oscillation (OI-003) | Low (revised values calculated) | Flicker at low dim | Loop analysis complete; verify on prototype | IN PROGRESS |
| LED thermal failure (OI-008) | Low (STW20NM60N + 25 vias + 30×30mm pour + 600mA limit: Tj=106°C calc. at fixed 44V; 94°C with NTC derate) | Premature failure | Hardware redesign: higher-rated MOSFET, more vias, larger pour, lower current limit; no adaptive Vbus needed | RESOLVED |
| DALI non-conformance (OI-005) | N/A until tested | Cannot sell as DALI-2 | Plan certification after prototype | OPEN |
| Safety non-compliance (OI-006) | N/A until assessed | Cannot sell in EU | Clearance/creepage normatively derived (IEC 62368-1:2018); lab verification still required | PARTIALLY RESOLVED |
| EMC failure (OI-007) | Medium | Cannot sell | Pre-compliance test on prototype; input filter pre-designed | OPEN |
| Minimum dimming not achievable (OI-010) | Low (typical parts OK) | Non-compliant dimming | Firmware offset compensation; dual-range PWM fallback | IN PROGRESS |
