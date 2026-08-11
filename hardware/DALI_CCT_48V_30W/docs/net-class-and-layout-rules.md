# Net Classes and Layout Rules

## PCB Stackup

| Layer | Name | Weight | Role |
|---|---|---|---|
| 1 (top) | F.Cu | 35µm (1oz) | Signal, DALI isolated domain, logic routing |
| 2 | In1.Cu | 35µm (1oz) | Uninterrupted GND reference plane |
| 3 | In2.Cu | 35µm (1oz) | Inner signal/power routing |
| 4 (bottom) | B.Cu | 70µm (2oz) | Power/thermal; couples to aluminium enclosure |

Board material: FR4, Tg ≥ 150°C. Thickness: 1.6mm.

---

## Net Classes

### PWR_HV (48V Input Power)

Applies to nets matching: `48V*`, `PGND*`

| Rule | Value | Rationale |
|---|---|---|
| Minimum track width | 2.0 mm | 1.5A continuous; temperature rise < 10°C in 1oz Cu |
| Minimum clearance | 0.5 mm | 60V SELV + creepage margin |
| Via drill | 0.6 mm | Sufficient for power |
| Via annular ring | 0.3 mm | Minimum for outer layers |
| Differential pair gap | N/A | Not differential |

### PWR_LED (LED Bus and Channels)

Applies to nets matching: `LED*`, `LED_BUS*`, `WW-`, `CW-`

| Rule | Value | Rationale |
|---|---|---|
| Minimum track width | 1.5 mm | 700mA peak per channel; temperature rise < 10°C |
| Minimum clearance | 0.3 mm | 44V + margin |
| Via drill | 0.5 mm | Power via |
| Via annular ring | 0.25 mm | Standard |

### DALI_ISO (DALI Isolated Domain)

Applies to nets matching: `DALI*`

| Rule | Value | Rationale |
|---|---|---|
| Minimum track width | 0.5 mm | Low current; wide for robustness |
| Minimum clearance | 0.5 mm | Within isolated domain |
| **Isolation gap to non-isolated domain** | **8 mm minimum** | 2.5kVrms isolation goal; see note |
| Via drill | 0.4 mm | Standard signal via |

**Isolation Note**: The 8mm PCB clearance between DALI and non-isolated domains is designed for 48V SELV input plus a 2.5kVrms isolation component goal. This value is subject to final safety standard review per IEC 62368-1 / EN 61347-1, pollution degree, and overvoltage category. No safety certification is claimed.

### SIGNAL (MCU and Logic)

Applies to nets matching: `MCU_*`, `I2C_*`, `ADC_*`, `EN_*`, `OCP_*`, `SWD*`, `NRST`

| Rule | Value | Rationale |
|---|---|---|
| Minimum track width | 0.25 mm | Low current; 25mA max |
| Minimum clearance | 0.2 mm | Standard 50V signal |
| Via drill | 0.4 mm | Standard signal |

### KELVIN (Current Sense Traces)

Applies to nets matching: `*SENSE*`, `*KELVIN*`, `WW_SENSE_*`, `CW_SENSE_*`

| Rule | Value | Rationale |
|---|---|---|
| Minimum track width | 0.15 mm | Voltage sense only; no current |
| Minimum clearance | 0.2 mm | Must not share return path with power |
| Must route as pair | Yes | Kelvin differential sense |
| Must not cross switching nodes | Yes | Keep > 5mm from SW nodes of buck-boost |
| Via drill | 0.3 mm | Minimal parasitic resistance |

---

## Layout Rules and Guidelines

### Critical Placement Rules

1. **Buck-boost switching loop**: Q1–Q4 + L2 + C4–C7 + R_CS_H/L must form a compact loop. Minimize area of the high-side switch to output inductor loop. Place LT8390A immediately adjacent to gate drive pins of Q1–Q4.

2. **Isolation keepout zone**: A 8mm-wide band running the full board height (0–60mm) centered approximately at x=26mm separates DALI components (x < 22mm) from non-isolated components (x > 30mm). This zone must contain:
   - No copper traces or pours on any layer
   - No vias of any type
   - No component bodies or footprints
   - Only the isolation component (optocoupler or DALI IC transformer) may bridge this zone

3. **Current sink thermal**: Q_WW and Q_CW must be placed on the bottom of the board (B.Cu thermal zone) with:
   - Exposed pad connected to B.Cu thermal pour
   - Minimum 4× thermal vias per device (0.3mm drill, 0.5mm diameter, filled/capped preferred)
   - B.Cu thermal zone coupled to aluminium enclosure via thermal interface material

4. **Kelvin routing**: R_SENSE_WW and R_SENSE_CW must have four-terminal (Kelvin) connections:
   - Force terminals carry load current (wide traces ≥ 1.5mm)
   - Sense terminals connect at resistor body only (0.15mm traces to op-amp inputs)
   - Return sense trace (WW_SENSE_L / CW_SENSE_L) must return to a quiet PGND star point, not the main power return

5. **Analog ground star**: Connect all analog return paths (op-amp references, DAC AGND, ADC AGND) to a single AGND star point near the MCU VSSA pin. Connect AGND star to PGND at one point only (single-point ground).

6. **DALI connector placement**: J2 must be at the left edge (x < 5mm) of the board, in the isolated DALI domain.

7. **Power connectors**: J1 (48V input) and J3 (LED output) must be at board edges (left and right respectively) for cable exit.

8. **SWD connector**: J4 should be accessible from the board edge or front face; not buried under components.

### Impedance and Signal Integrity

- MCU clock net (if external crystal): route as 50Ω coplanar waveguide with GND pour; keep < 10mm
- I2C traces: keep < 30mm total; 4.7kΩ pull-ups to 3.3V_MCU
- DALI_TX/DALI_RX: keep in isolated domain; route away from buck-boost switching nodes
- Gate drive traces (Q1–Q4): keep < 10mm from LT8390A gate outputs; 47Ω gate resistors placed close to MOSFET gates

### Test Point Placement

| Test Point | Net | Location |
|---|---|---|
| TP_48VIN | 48V_IN+ | Near J1 input |
| TP_48VFILT | 48V_FILT+ | After D1/TVS, before buck-boost |
| TP_LEDBUS | LED_BUS+ | Buck-boost output |
| TP_3V3 | 3.3V_MCU | Aux supply output |
| TP_5V | 5V_AUX | Before LDO |
| TP_DALI_A | DALI_A | DALI isolated side |
| TP_DALI_B | DALI_B | DALI isolated side |
| TP_SENSE_WW | WW_SENSE_H | Current sense WW |
| TP_SENSE_CW | CW_SENSE_H | Current sense CW |
| TP_SWDIO | SWDIO | SWD data |
| TP_SWDCLK | SWDCLK | SWD clock |
| TP_NRST | NRST | MCU reset |
| TP_GND | PGND | Ground reference |

All test points: 0.5mm SMD pad, or 1mm through-hole.

### Silkscreen Labels

- All connectors: reference designator (J1, J2, J3, J4) + pin 1 marker + net name (48V+, GND, DALI_A, LED+, etc.)
- Polarity markers on electrolytic capacitors
- Revision label: "REV A" on F.SilkS
- Warning: "REFERENCE DESIGN – NOT FOR PRODUCTION WITHOUT REVIEW" on F.SilkS
- Mounting hole labels: "M3 NPTH" near each mounting hole
- Isolation zone marker: "ISO BARRIER >8mm" spanning the isolation keepout zone

---

## DRC Constraints (KiCad Design Rules)

Add to project `.kicad_pro` or `.kicad_dru` file:

```
(rule "PWR_HV clearance"
  (constraint clearance (min 0.5mm))
  (condition "A.NetClass == 'PWR_HV' || B.NetClass == 'PWR_HV'")
)

(rule "DALI isolation"
  (constraint clearance (min 8mm))
  (condition "(A.NetClass == 'DALI_ISO') != (B.NetClass == 'DALI_ISO')")
)

(rule "Kelvin sense width"
  (constraint track_width (max 0.2mm))
  (condition "A.NetClass == 'KELVIN'")
)

(rule "Power track width"
  (constraint track_width (min 2mm))
  (condition "A.NetClass == 'PWR_HV'")
)

(rule "LED track width"
  (constraint track_width (min 1.5mm))
  (condition "A.NetClass == 'PWR_LED'")
)

(rule "Board edge clearance"
  (constraint edge_clearance (min 0.5mm))
)
```
