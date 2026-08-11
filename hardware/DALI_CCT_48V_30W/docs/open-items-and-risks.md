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

### OI-006 [VALIDATION] Safety Certification

**Description**: No safety assessment has been performed. For EU market, relevant standards may include IEC 62368-1, EN 61347-2-13, EN 60598, 2014/35/EU (LVD).

**Required action**:
- Engage qualified test laboratory for safety assessment.
- Verify creepage and clearance dimensions in final PCB layout (8 mm DALI isolation gap documented in `net-class-and-layout-rules.md`).
- Verify isolation component ratings (2.5 kVrms goal vs. required test voltage per standard).
- Verify thermal and flammability compliance.

**Status**: 🔲 OPEN – process item; requires final PCB layout before assessment. Layout isolation rules pre-documented.

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

### OI-008 [VALIDATION] Thermal Validation

**Description**: Each linear current sink (Q_WW, Q_CW) can dissipate up to 11.2 W at worst case (44V bus, 28V LED, 700mA). Aluminium enclosure coupling is assumed but not dimensioned.

**Thermal budget (estimated)**:

| Node | Rth (est.) | ΔT at 11.2W |
|---|---|---|
| Junction → package (NTMFS5C604NL SOT-223) | 10 °C/W | 112 °C |
| Package → PCB (solder + exposed pad) | 3 °C/W | 34 °C |
| PCB → Al enclosure (thermal vias + TIM) | 2 °C/W | 22 °C |
| Al enclosure → ambient | dependent on enclosure | — |

At 25 °C ambient and Rth_junc-ambient ≈ 15 °C/W (junction to PCB): Tj ≈ 25 + 11.2 × 15 = **193 °C** – unacceptable. Layout must use ≥ 4 thermal vias per MOSFET through to bottom copper pour, which reduces PCB thermal resistance to ~2 °C/W. With effective Rth_junc-enclosure ≈ 8 °C/W: Tj ≈ 25 + 11.2 × 8 = **115 °C** (acceptable, Tj,max = 150 °C for NTMFS5C604NL).

**Required action**:
- Measure junction temperature on prototype under worst-case conditions.
- Characterize thermal resistance through layout.
- If Tj > 125°C at 50°C ambient: add more thermal vias or reduce maximum current firmware limit.

**Status**: 🔲 OPEN – estimated budget provided; validate on prototype. Stage 6 of bring-up-and-test-plan.md.

**Risk level**: HIGH – thermal failure will reduce reliability or cause immediate shutdown.

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
| LED thermal failure (OI-008) | Medium | Premature failure | Thermal budget estimated; thermal vias required; measure on prototype | OPEN |
| DALI non-conformance (OI-005) | N/A until tested | Cannot sell as DALI-2 | Plan certification after prototype | OPEN |
| Safety non-compliance (OI-006) | N/A until assessed | Cannot sell in EU | Engage test lab after PCB layout | OPEN |
| EMC failure (OI-007) | Medium | Cannot sell | Pre-compliance test on prototype; input filter pre-designed | OPEN |
| Minimum dimming not achievable (OI-010) | Low (typical parts OK) | Non-compliant dimming | Firmware offset compensation; dual-range PWM fallback | IN PROGRESS |
