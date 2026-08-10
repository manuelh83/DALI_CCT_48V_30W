# Open Items and Risks – DALI CCT 48V 30W Rev A

## Summary

This document lists all known open design items, unresolved decisions, and engineering risks as of Rev A. All items marked **[BLOCKER]** must be resolved before PCB fabrication. Items marked **[VALIDATION]** must be resolved before production release.

---

## Open Items

### OI-001 [BLOCKER] Auxiliary Supply Input Voltage Rating

**Description**: The LMR14020SDDA is specified for 4V–42V input. The operating input range of this design is 43.2–52.8V, which exceeds the LMR14020 absolute maximum.

**Required action**: Replace with a 60V-rated buck regulator. Candidates:
- Texas Instruments LMR16006YDDAR (4–60V input, 600mA, SOT-23-5)
- Diodes Inc AP65150A-U (6–60V input, 1.5A, SOT-23-6)
- Torex XC9271B (4.5–65V input, 300mA)
- Alternatively: use a small isolated flyback with UC3843 or similar for 5V/500mA isolation and better EMI control

**Risk level**: HIGH – current part will fail in normal use.

---

### OI-002 [BLOCKER] Buck-Boost Compensation Network

**Description**: The LT8390A requires a carefully designed compensation network (ITH RC, slope compensation set by SLOPE pin resistor). No compensation values are calculated in Rev A.

**Required action**:
1. Build SIMPLIS or LTspice model of the 4-switch topology with actual L2 and C4–C6 values
2. Characterize LED load impedance (static and dynamic) across temperature
3. Calculate ITH network (Cith, Rith, Cith2) per LT8390A datasheet section "Setting the Error Amplifier Compensation"
4. Verify loop crossover ≥ 5kHz with phase margin ≥ 45°
5. Validate on prototype

**Risk level**: HIGH – unstable converter will cause oscillation, EMI, or LED flicker.

---

### OI-003 [BLOCKER] Current Sink Loop Compensation

**Description**: The OPA2333 error amplifier with LED MOSFET sink requires loop stability analysis. Placeholder compensation (10nF / 10kΩ RC) is not designed for specific LED characteristics.

**Required action**:
1. Model LED dynamic impedance (typically 1Ω–10Ω for high-power LEDs)
2. Model MOSFET transconductance at operating point
3. Calculate or simulate dominant pole frequency and phase margin
4. Design compensation to ensure stable operation from 0.5mA to 700mA
5. Verify no oscillation at any current setpoint via prototype measurement

**Risk level**: HIGH – oscillation at low currents will cause visible flicker.

---

### OI-004 [VALIDATION] DALI IC Selection and Firmware Impact

**Description**: Two options are listed for the DALI interface:
- NXP UBA2015 (integrated DALI slave IC): simplifies hardware but firmware must interface via UART; check DT8 Tc support in any available firmware library
- IS181XBHTR + discrete driver: requires full DALI Manchester encoder/decoder in firmware

**Required action**:
- Confirm UBA2015 availability and DT8(Tc) software support
- Alternatively commit to custom DALI firmware stack
- Evaluate DALI-2 certification path for chosen approach

**Risk level**: MEDIUM – affects firmware complexity and certification path.

---

### OI-005 [VALIDATION] DALI-2 Conformance Testing

**Description**: This design targets DALI-2 DT8 Tc behavior per IEC 62386-101/-102/-209 but does not hold DALI-2 certification. Official certification requires:
- Membership or engagement with DALI Alliance (DIIA)
- Submission to accredited test laboratory
- Passing of all mandatory test cases for device type DT8(Tc)
- Publication in DALI Alliance certified product database

**Required action**: Plan certification timeline and budget; do not label product as "DALI-2" or "DALI certified" without completed certification.

**Risk level**: MEDIUM – label compliance issue.

---

### OI-006 [VALIDATION] Safety Certification

**Description**: No safety assessment has been performed. For EU market, relevant standards may include:
- IEC 62368-1 (Audio/video, IT equipment – Safety)
- EN 61347-2-13 (Particular requirements for DC or AC supplied electronic controlgear for LED modules)
- EN 60598 (Luminaires) if supplied as part of a luminaire
- 2014/35/EU (Low Voltage Directive) for CE marking

**Required action**:
- Engage qualified test laboratory for safety assessment
- Verify creepage and clearance dimensions in final PCB layout
- Verify isolation component ratings (2.5kVrms goal vs. required test voltage per standard)
- Verify thermal and flammability compliance

**Risk level**: HIGH for regulated markets.

---

### OI-007 [VALIDATION] EMC Pre-Compliance

**Description**: No EMC testing has been performed. Conducted and radiated emissions from a 200kHz switching converter must meet applicable limits (EN 55015 / CISPR 15 for lighting).

**Required action**:
- Perform conducted emission scan (9kHz–30MHz) on prototype
- Perform radiated emission scan (30MHz–1GHz)
- Add input filter components as necessary
- Document worst-case switching harmonics and mitigation

**Risk level**: MEDIUM – typically requires iteration.

---

### OI-008 [VALIDATION] Thermal Validation

**Description**: Each linear current sink (Q_WW, Q_CW) can dissipate up to 11.2W at worst case (44V bus, 28V LED, 700mA). Aluminium enclosure coupling is assumed but not dimensioned.

**Required action**:
- Measure junction temperature on prototype under worst-case conditions
- Characterize thermal resistance through: junction → package → PCB → thermal vias → Al enclosure
- If Tj > 125°C at ambient 50°C: increase MOSFET thermal pad area, add more thermal vias, or reduce maximum current firmware limit
- Determine maximum enclosure temperature for rated operation

**Risk level**: HIGH – thermal failure will reduce reliability or cause immediate shutdown.

---

### OI-009 [OPEN] CCT to WW/CW Mixing Algorithm

**Description**: The mapping from DALI Tc command (colour temperature in mirek) to WW/CW current ratio depends on the actual LED module spectral characteristics.

**Required action**:
- Characterize WW and CW LED chromaticity (CIE xy) vs. current
- Implement calibrated mixing table or formula
- Verify that commanded Tc can be achieved within LED colour gamut
- Consider optional factory calibration procedure

**Status**: Firmware placeholder with linear interpolation; replace with measured data.

---

### OI-010 [OPEN] Minimum Dimming Level

**Description**: The specification requires 0.1% dimming (minimum 1:1000 ratio). With 700mA full scale, this implies 0.7mA minimum. Op-amp input offset voltage of OPA2333 (max 10µV) causes ~0.1mA current error at 100mΩ sense resistor.

**Required action**:
- Verify actual minimum achievable current on prototype
- If 0.1% is not achievable with current architecture at all temperatures, consider dual-range approach (switch to smaller sense resistor below 1%)

**Status**: Under evaluation.

---

### OI-011 [OPEN] Schematic Symbol/Netlist Completion

**Description**: The Rev A `.kicad_sch` file contains functional text descriptions and net lists rather than full KiCad symbol-and-wire schematics. This is a known limitation of the initial reference deliverable.

**Required action**:
- Import all component symbols from KiCad standard libraries and vendor libraries
- Draw all circuit connections as wires and bus connections
- Add pin labels, power symbols, and no-connect markers
- Run KiCad ERC; document and resolve all errors
- Assign footprints from `docs/bom.csv` to all symbols

**Status**: First priority for next revision.

---

### OI-012 [OPEN] PCB Component Placement

**Description**: The Rev A `.kicad_pcb` file contains board outline, mounting holes, connector placeholders, zone definitions, and layout region annotations. Full component placement is not complete.

**Required action**:
- Import netlist after completing schematic (OI-011)
- Place all components per placement guidelines in `net-class-and-layout-rules.md`
- Run KiCad DRC; document and resolve violations
- Complete routing of critical nets; document unfinished connections

**Status**: Pending OI-011 completion.

---

## Risk Register

| Risk | Probability | Impact | Mitigation |
|---|---|---|---|
| Aux supply exceeds VIN rating (OI-001) | Certain if not fixed | Board destruction | Replace with LMR16006 |
| Buck-boost instability (OI-002) | High without analysis | LED flicker, EMI | SPICE simulation + prototype |
| Current sink oscillation (OI-003) | High without analysis | Flicker at low dim | Loop analysis + prototype |
| LED thermal failure (OI-008) | Medium | Premature failure | Thermal vias + measurement |
| DALI non-conformance (OI-005) | N/A until tested | Cannot sell as DALI-2 | Plan certification |
| Safety non-compliance (OI-006) | N/A until assessed | Cannot sell in EU | Engage test lab |
| EMC failure (OI-007) | Medium | Cannot sell | Pre-compliance test |
