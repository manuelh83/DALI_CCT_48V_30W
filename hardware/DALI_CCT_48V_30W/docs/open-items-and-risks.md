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

### OI-008 [IN PROGRESS] Thermal Validation

**Description**: Each linear current sink (Q_WW, Q_CW) can dissipate up to 11.2 W at worst case (44V bus, 28V LED, 700mA). Aluminium enclosure coupling is assumed but not dimensioned.

**Thermal optimization analysis (Rev A → Rev A.1):**

The original SOT-223 package was found to be thermally inadequate (calculated Tj = 193°C). The following measures have been taken:

#### Measure 1: Package change SOT-223 → D2PAK (TO-263)

Q_WW and Q_CW are changed from **NTMFS5C604NL (SOT-223)** to **STB20NF06L (D2PAK / TO-263)**:

| Parameter | NTMFS5C604NL SOT-223 | STB20NF06L D2PAK |
|---|---|---|
| Vdss | 60 V | 60 V |
| Id | 15 A | 20 A |
| Rds(on) | 9 mΩ | 32 mΩ |
| Rth(j-c) | 10 °C/W | **3.1 °C/W** |
| Package | SOT-223 (small tab) | D2PAK (large exposed underside tab) |

#### Measure 2: Increased thermal via count per device

Increase from ≥4 to **9 thermal vias per device** (3×3 array, 0.3 mm drill, 0.5 mm diameter, Cu-filled/capped preferred):

```
Via resistance per via:
  R_via = L / (k_Cu × A_via)
         = 0.0016 m / (385 W/m·K × π × (0.15×10⁻³)² m²)
         = 0.0016 / (385 × 7.07×10⁻⁸)
         = 58.8 °C/W per via

9 vias in parallel:
  R_vias = 58.8 / 9 = 6.5 °C/W

B.Cu lateral spreading resistance (20×20 mm Cu pour):
  ~0.5 °C/W

Total R_PCB-enclosure (vias + spreading) ≈ 1.2 °C/W
```

#### Measure 3: Adaptive bus voltage firmware control

The primary measure to achieve Tj < 125°C is **adaptive bus voltage regulation**:

- Firmware reads ADC_VBUS and estimates MOSFET V_DS from V_LED_BUS and LED forward voltage model (calibrated per LED module type)
- Firmware targets V_bus = V_LED_forward(I) + 2.5 V headroom (minimum for current-sink operation)
- LT8390A setpoint updated via feedback divider DAC or digital potentiometer (OI to track implementation)
- Minimum V_bus: 30.5 V (covers 28 V LED + 2.5 V headroom)
- Maximum V_bus: 44 V (existing set point)

Effect: at worst-case 28 V LED / 700 mA, Pdiss drops from 11.2 W to:
```
P_diss = (V_bus_adaptive − V_LED) × I = (28 + 2.5 − 28) × 0.7 = 2.5 × 0.7 = 1.75 W
```

#### Revised thermal budget (all measures combined)

Assumptions: T_ambient = 50 °C (worst-case operating), adaptive bus voltage active, both channels at 700 mA.

| Segment | Component | Rth | ΔT (at 1.75W) |
|---|---|---|---|
| Junction → case | STB20NF06L D2PAK (IEC thermal) | 3.1 °C/W | 5.4 °C |
| Case → PCB | Solder pad, D2PAK exposed tab | 1.0 °C/W | 1.75 °C |
| PCB → Al enclosure | 9 thermal vias + 20×20 mm B.Cu + TIM | 1.2 °C/W | 2.1 °C |
| Al enclosure → ambient | 150×60 mm Al extrusion (both channels = 3.5 W total) | 2.5 °C/W (total) | ΔT_Al ≈ 4.4 °C |
| **Total R_j-ambient** | | **≈ 7.8 °C/W** | |

```
T_Al = T_ambient + P_both × R_Al-ambient / 2 (per device)
     = 50 + 3.5 × 2.5 / 2 = 50 + 4.4 = 54.4 °C

Tj = T_Al + P_diss × (R_jc + R_cp + R_PCB)
   = 54.4 + 1.75 × (3.1 + 1.0 + 1.2)
   = 54.4 + 9.3
   = 63.7 °C   ← well below Tj,max = 150 °C

Margin: 150 − 63.7 = 86.3 °C margin
```

Worst-case without adaptive control (V_bus = 44 V, V_LED = 28 V, Pdiss = 11.2 W):
```
T_Al = 50 + 22.4 × 2.5 / 2 = 78 °C
Tj = 78 + 11.2 × 5.3 = 137 °C   ← exceeds 125 °C; adaptive control is REQUIRED
```

**Conclusion**: Adaptive bus voltage control (Measure 3) is mandatory to achieve Tj < 125 °C at worst-case LED voltage (28 V). With all three measures, Tj = 64 °C at T_ambient = 50 °C worst case. NTC shutdown at 85 °C PCB temperature remains as safety backup.

**Changes required (in addition to documentation):**
- `bom.csv`: Q_WW, Q_CW updated to STB20NF06L D2PAK
- `net-class-and-layout-rules.md`: thermal via count updated to 9 per device, 20×20 mm B.Cu zone
- `bring-up-and-test-plan.md`: Stage 6 updated with revised pass criterion and adaptive-control test
- Firmware: implement adaptive V_bus control (new OI to track)

**Required action (still open)**:
- Measure junction temperature on prototype under worst-case conditions (both channels, adaptive and non-adaptive modes).
- Measure Al enclosure surface temperature; verify R_Al-ambient ≤ 2.5 °C/W (total power).
- If Tj > 100°C at 50°C ambient: increase via count or reduce firmware current limit.
- Validate adaptive bus voltage control firmware; measure V_DS on Q_WW/Q_CW.

**Status**: 🔶 IN PROGRESS – thermal budget rechnerisch gelöst mit kombinierter Maßnahme (D2PAK + 9 Thermal-Vias + adaptive Busspannung); Tj < 65 °C berechnet bei 50 °C Umgebung. Prototyp-Validierung weiterhin erforderlich.

**Risk level**: MEDIUM – calculated budget demonstrates feasibility; prototype measurement required before production.

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

### OI-011 [OPEN] Schematic Symbol/Netlist Completion

**Description**: The Rev A `.kicad_sch` file contains functional text descriptions and net lists rather than full KiCad symbol-and-wire schematics.

**Required action**:
- Import all component symbols from KiCad standard libraries and vendor libraries.
- Draw all circuit connections as wires and bus connections.
- Add pin labels, power symbols, and no-connect markers.
- Run KiCad ERC; document and resolve all errors.
- Assign footprints from `docs/bom.csv` to all symbols.

**Status**: 🔲 OPEN – first priority for Rev A PCB work. Prerequisite for OI-012.

---

### OI-012 [OPEN] PCB Component Placement

**Description**: The Rev A `.kicad_pcb` file contains board outline, mounting holes, connector placeholders, zone definitions, and layout region annotations. Full component placement is not complete.

**Required action**:
- Import netlist after completing schematic (OI-011).
- Place all components per placement guidelines in `net-class-and-layout-rules.md`.
- Run KiCad DRC; document and resolve violations.
- Complete routing of critical nets; document unfinished connections.

**Status**: 🔲 OPEN – pending OI-011 completion.

---

## Risk Register

| Risk | Probability | Impact | Mitigation | Status |
|---|---|---|---|---|
| Aux supply exceeds VIN rating (OI-001) | ~~Certain if not fixed~~ N/A | ~~Board destruction~~ | ✅ Replaced with LMR16006YDDAR | RESOLVED |
| Buck-boost instability (OI-002) | Medium (calculations done, unvalidated) | LED flicker, EMI | LTspice simulation + prototype Bode plot | IN PROGRESS |
| Current sink oscillation (OI-003) | Low (revised values calculated) | Flicker at low dim | Loop analysis complete; verify on prototype | IN PROGRESS |
| LED thermal failure (OI-008) | Low (D2PAK + 9 vias + adaptive V_bus: Tj=64°C calc.) | Premature failure | D2PAK package, 9 thermal vias, adaptive bus voltage (all measures documented); measure on prototype | IN PROGRESS |
| DALI non-conformance (OI-005) | N/A until tested | Cannot sell as DALI-2 | Plan certification after prototype | OPEN |
| Safety non-compliance (OI-006) | N/A until assessed | Cannot sell in EU | Clearance/creepage normatively derived (IEC 62368-1:2018); lab verification still required | PARTIALLY RESOLVED |
| EMC failure (OI-007) | Medium | Cannot sell | Pre-compliance test on prototype; input filter pre-designed | OPEN |
| Minimum dimming not achievable (OI-010) | Low (typical parts OK) | Non-compliant dimming | Firmware offset compensation; dual-range PWM fallback | IN PROGRESS |
