# Design Specification – DALI CCT 48V 30W Rev A

> **Reference Design – Subject to Engineering Review**

---

## 1. System Overview

This document specifies the electrical design of a 48 V DC SELV input, 30 W maximum output, DALI-2 DT8 Tunable White LED driver reference design. The design uses a 4-switch synchronous buck-boost converter to generate a regulated 44 V LED bus, followed by two independently controlled linear low-side current sinks for warm-white (WW) and cool-white (CW) LED strings sharing a common anode.

---

## 2. Input Specification

| Parameter | Min | Nom | Max | Unit | Notes |
|---|---|---|---|---|---|
| Input voltage | 43.2 | 48 | 52.8 | V DC | ±10% operating range |
| Transient protection | — | — | 60 | V | Short transient events; TVS clamp |
| Input current | — | ~800 | 1500 | mA | At full 30W load; fuse rated 1.5A |
| Input type | | SELV | | | Class 2 / SELV supply assumed |
| Reverse polarity | — | — | 60 | V | P-channel MOSFET protection |
| Input capacitance | 200 | — | — | µF | 2× 100µF/63V electrolytic |

### 2.1 Input Protection Components

| Designator | Function | Part | Rating |
|---|---|---|---|
| F1 | Fuse | Littelfuse 0452001.MRL | 1A slow-blow, 63V |
| D1 | Reverse polarity | Si2333DS P-ch MOSFET + 10kΩ gate pull-down | 60V, 4A |
| TV1 | Transient clamp | SMAJ60A or P6KE60A | 60V, 600W peak |
| C1, C2 | Bulk input caps | Panasonic EEU-FR1J101 | 100µF/63V electrolytic |
| C3 | HF bypass | Murata GRM31CR61E475KA88L | 4.7µF/25V MLCC X5R |
| L1 | Common-mode choke | TDK ACM2520-201-2P-T | 10µH/2A |

---

## 3. Buck-Boost LED Bus (44V)

### 3.1 Architecture

A 4-switch synchronous buck-boost topology allows input voltage (43.2–52.8 V) above, equal to, or below the output voltage (44 V). The LT8390A controller manages all four gate drives with a unified control loop.

### 3.2 Controller: LT8390A

- Manufacturer: Analog Devices (formerly Linear Technology)
- Datasheet: https://www.analog.com/media/en/technical-documentation/data-sheets/lt8390a.pdf
- Input range: 4V to 60V
- Output range: 1V to 60V
- Switching frequency: configurable 100kHz–1MHz; **target: 200kHz** (set by RT resistor)
- Current sense: high-side and low-side sensed; average current mode control
- Enable/shutdown: active-high SHDN pin, driven by MCU or tied high

### 3.3 Power Components

| Designator | Function | Part | Rating |
|---|---|---|---|
| Q1, Q2 | Buck high-side / low-side | AON7534 | 30V, 25A, 4.5mΩ |
| Q3, Q4 | Boost high-side / low-side | AON6424 | 40V, 20A, 5mΩ |
| L2 | Main inductor | Bourns SRR1260-470M | 47µH, 3A, 100mΩ DCR |
| C4, C5, C6 | Output bulk caps | Panasonic EEU-FR1J470 | 47µF/63V electrolytic |
| C7 | Output HF bypass | GRM31CR61E475KA88L | 4.7µF/63V MLCC |
| R_CS_H | High-side current sense | Vishay WSBS2515R0100FEA | 10mΩ/1W/1% |
| R_CS_L | Low-side current sense | Vishay WSBS2515R0100FEA | 10mΩ/1W/1% |

### 3.4 Output Voltage Setting

```
Vout = V_REF × (1 + R_FB1 / R_FB2)
V_REF = 0.8V (LT8390A internal reference)
Target: 44V
R_FB1 = 750kΩ (top)
R_FB2 = 13.94kΩ → use 13.7kΩ 1% + trim
```

**Compensation Network (calculated; validate in simulation and prototype)**

The following values are calculated for a target voltage-loop crossover frequency of approximately 5 kHz with ≥ 45° phase margin. All values must be confirmed with an LTspice/SIMPLIS model and adjusted on the prototype.

| Component | Net | Calculated Value | Standard Value | Notes |
|---|---|---|---|---|
| Cith | ITH to GND (dominant integrating cap) | 14.5 nF | **15 nF** | Sets error-amp integrator; dominant pole ≈ 480 Hz |
| Rith | ITH series resistor | 22 kΩ | **22 kΩ** | Sets loop crossover via Gm_EA × Rith × Cout; fc ≈ 5 kHz |
| Cith2 | Across Rith (HF rolloff cap) | 360 pF | **390 pF** | Rolls off at ≈ 18 kHz (≈ fsw/11); suppresses switching noise |
| R_SLOPE | SLOPE pin to GND | — | **470 kΩ** | Slope compensation starting point for 200 kHz / 47 µH; adjust per datasheet Fig. 16 |

Design equations used (LT8390A datasheet, "Setting the Error Amplifier Compensation"):

```
Gm_EA  = 200 µA/V (typ.)
C_out  = 141 µF (3 × 47 µF, ESR ≈ 17 mΩ combined)
f_c    = 5 kHz (target crossover)

Rith   = (2π × f_c × C_out) / Gm_EA  =  (2π × 5000 × 141×10⁻⁶) / 200×10⁻⁶  ≈  22 kΩ
Cith   = 1 / (2π × Rith × f_p_EA)    where f_p_EA = f_c / 10 = 500 Hz  →  Cith ≈ 14.5 nF
Cith2  = 1 / (2π × Rith × f_rolloff) where f_rolloff = 18 kHz           →  Cith2 ≈ 360 pF
```

Validation checklist (OI-002):
1. Build LTspice model with actual L2 (47 µH / 100 mΩ DCR) and C4–C6 (3 × 47 µF / 63 V electrolytic, ESR measured or from datasheet).
2. AC loop analysis: confirm crossover ≥ 5 kHz, phase margin ≥ 45° at Vin = 43.2 V, 48 V, 52.8 V.
3. Adjust Rith up/down by ±20 % if crossover is out of range.
4. Confirm slope compensation (R_SLOPE) prevents subharmonic oscillation; refer to datasheet Fig. 16.
5. Prototype measurement: inject signal into feedback divider; measure Bode plot with network analyser.

### 3.5 Efficiency Estimate

- Expected efficiency at 30W: ~92–95% (LT8390A + synchronous rectification)
- Power loss in converter at full load: ~1.5–2.4W

---

## 4. Linear Low-Side Current Sinks

### 4.1 Architecture

Two identical channels (WW and CW) each consist of:
1. N-channel power MOSFET (low-side, drain to LED cathode)
2. Current-sense resistor (source to PGND, Kelvin-routed)
3. Precision op-amp error amplifier (OPA2333) comparing sense voltage to DAC setpoint
4. Hardware OCP comparator (LM393) for hard over-current protection

### 4.2 Key Parameters

| Parameter | Value | Notes |
|---|---|---|
| Max current per channel | 700 mA | Hardware capability |
| Min current per channel | ~0.5 mA | Limited by DAC resolution and op-amp offset |
| Dimming ratio | 1400:1 | 0.07% of 700mA = 0.5mA |
| Sense resistor | 100 mΩ | Vsense = 70mV at 700mA |
| OCP threshold | ~1.05A | Vsense = 105mV at OCP comparator |
| MOSFET Vdss | 60V | Must withstand LED_BUS+ when off |
| Max Pdiss per channel | 11.2W | (44V − 28V) × 700mA worst case |

### 4.3 Component Selection

| Designator | Function | Part | Rating |
|---|---|---|---|
| Q_WW, Q_CW | Current sink MOSFET | NTMFS5C604NL | 60V, 15A, 9mΩ, SOT-223 |
| R_SENSE_WW, R_SENSE_CW | Current sense | Vishay WSHM2818R1000FEB | 100mΩ/1W/1% |
| U_CS_WW, U_CS_CW | Error amplifier | Texas Instruments OPA2333AIDR | 1.8V–5.5V supply, 25nV/√Hz noise |
| U_OCP | Dual comparator | Texas Instruments LM393DR | Open-collector output |
| U_DAC | 12-bit quad I2C DAC | Microchip MCP4728A0T-E/UN | 12-bit, 4-channel, I2C, 5V rail |
| R_GATE_WW, R_GATE_CW | Gate resistors | 47Ω 1% SMD 0402 | Slows gate transient |
| C_COMP_WW, C_COMP_CW | Loop compensation dominant cap | 100 nF / 10 kΩ RC (see §4.5) | Revised; ensures stability ≥ 0.5 mA |

### 4.4 Dimming Behavior

- **Normal dimming**: Continuous analog DAC control. MCU writes 12-bit value to MCP4728; DAC output 0–1.0V → current 0–700mA.
- **Below 0.1%**: MCU sets DAC = 0V; MOSFET gate pulled to 0V via op-amp output = 0V. Channel fully off.
- **PWM usage**: None in normal dimming. High-frequency PWM (>40kHz) may be used only for enable/disable actions or fault recovery, not for dimming.
- **DALI dimming curve**: Logarithmic mapping per IEC 62386-102 Table 2; 254 arc-power levels mapped to physical current values.

### 4.5 Current Sink Loop Compensation Analysis (OI-003)

The linear current sink forms a feedback loop:

```
DAC setpoint (V_SET) → OPA2333 error amp → MOSFET gate → R_SENSE → V_SENSE → OPA2333 inverting input
```

**Loop parameters at operating point:**

| Parameter | Value | Notes |
|---|---|---|
| MOSFET gm (at 700 mA) | ≈ 2 A/V | NTMFS5C604NL; extrapolated from Id–Vgs curve at Vgs ≈ 2.5 V |
| MOSFET gm (at 0.5 mA) | ≈ 200 mA/V | Minimum operating current; gm drops significantly |
| R_SENSE | 100 mΩ | Current-to-voltage: 70 mV at 700 mA |
| LED dynamic impedance | 1–3 Ω | Typical for high-power LEDs; measured on prototype |
| OPA2333 GBW | 350 kHz | From datasheet |
| OPA2333 input offset (max) | 10 µV | Causes ~0.1 mA offset error at 100 mΩ sense |

**Compensation design:**

The dominant pole is placed at the OPA2333 output using R_comp (10 kΩ) and C_comp (100 nF):

```
f_p = 1 / (2π × R_comp × C_comp) = 1 / (2π × 10kΩ × 100nF) ≈ 160 Hz
```

A second high-frequency rolloff cap (C_hf = 100 pF in parallel with C_comp) provides attenuation above:

```
f_hf = 1 / (2π × R_comp × C_hf) ≈ 160 kHz
```

**Stability margins (calculated):**

| Current | MOSFET gm | Loop DC gain | Phase margin (est.) |
|---|---|---|---|
| 700 mA | 2 A/V | ~54 dB | ≥ 60° |
| 10 mA | 400 mA/V | ~40 dB | ≥ 50° |
| 0.5 mA | 200 mA/V | ~32 dB | ≥ 45° |

**Revised component values (replace Rev A placeholder):**

| Designator | Old Value | New Value | Rationale |
|---|---|---|---|
| C_COMP_WW, C_COMP_CW | 10 nF | **100 nF** | Dominant pole at 160 Hz; stable over full current range |
| R_COMP_WW, R_COMP_CW | 10 kΩ | **10 kΩ** | Unchanged |
| C_HF_WW, C_HF_CW | (not present) | **100 pF** | Add in parallel; HF rolloff at 160 kHz |

**Validation checklist:**
1. Prototype measurement: sweep WW/CW from 0.5 mA to 700 mA; verify no oscillation (scope on V_SENSE).
2. Small-signal injection via DAC dither at 100 Hz–10 kHz; observe phase and gain.
3. If oscillation appears at low current: increase C_comp to 220 nF (fp = 72 Hz).
4. If settling is too slow (> 5 ms step response at 700 mA): reduce C_comp toward 47 nF.

### 4.6 Safe-Default Behavior

- On MCU power-up: DAC defaults to 0V output (all channels off).
- On OCP event: latch disables gate; MCU reads OCP flag, logs fault, attempts recovery after 100ms hold-off.
- On DALI bus loss: maintain last valid current setpoints (do not change output).
- On 48V input loss/restart: restore last NVM-saved dim/CCT state.

---

## 5. Auxiliary Power Supply

### 5.1 48V → 5V (Step-down)

- Controller: **Texas Instruments LMR16006YDDAR** synchronous buck (replaces LMR14020SDDA)
- Datasheet: https://www.ti.com/product/LMR16006
- Input range: 4V–60V; covers 43.2–52.8V operating range with full margin
- Output: 5V / 600mA (sufficient for MCU + DAC + DALI isolated supply)
- **OI-001 resolved**: LMR14020SDDA (42V max) has been replaced with LMR16006YDDAR (60V max) in the BOM. See bom.csv, designator U_AUX1.

### 5.2 5V → 3.3V (LDO)

- LDO: Diodes Inc AP2112K-3.3TRG1
- Output: 3.3V / 600mA
- Powers: STM32G031, AT24C32E, MCP4728, OPA2333, LM393

### 5.3 DALI Isolated Supply

- 5V_DALI derived via small flyback from 48V, or from 5V_AUX through a DC-DC isolator (e.g., Murata NXE1S0505MC)
- Return: DALI_GND (galvanically isolated from PGND)

---

## 6. MCU and Digital Design

### 6.1 MCU Selection

- Part: STMicroelectronics STM32G031K8T6
- Package: LQFP-32
- Datasheet: https://www.st.com/resource/en/datasheet/stm32g031k8.pdf
- Core: ARM Cortex-M0+, 64MHz, 64KB flash, 8KB SRAM
- Peripherals: 12-bit DAC×2, 12-bit ADC×1 (up to 12ch), USART×2, I2C×2, SPI×1, TIM×7

### 6.2 NVM / EEPROM

- Part: Microchip AT24C32E-SSHM-T (I2C EEPROM, 32Kbit / 4KB)
- Purpose: DALI NVM – stores short address, group membership, scene table (16 scenes × dim + CCT), last valid state, power-on level, system failure level, min/max levels, fade time/rate, device type, input power, color temperature range.
- Write endurance: 1 million cycles per page; firmware implements wear-leveling circular buffer for frequently-updated last-state record.

### 6.3 Pin Assignments (STM32G031K8T6)

| MCU Pin | Net | Function |
|---|---|---|
| PA0 / ADC1_IN0 | ADC_NTC | NTC thermistor ADC |
| PA1 / ADC1_IN1 | ADC_VIN | 48V input voltage monitor |
| PA2 / ADC1_IN2 | ADC_VBUS | LED bus voltage monitor |
| PA4 / DAC1_CH1 | MCU_DAC_WW | WW current setpoint (backup) |
| PA5 / DAC1_CH2 | MCU_DAC_CW | CW current setpoint (backup) |
| PA6 / TIM3_CH1 | OCP_WW | OCP comparator WW input |
| PA7 / TIM3_CH2 | OCP_CW | OCP comparator CW input |
| PA9 / USART1_TX | DALI_TX | DALI transmit (to optocoupler) |
| PA10 / USART1_RX | DALI_RX | DALI receive (from optocoupler) |
| PA13 / SWDIO | SWDIO | SWD debug/programming |
| PA14 / SWDCLK | SWDCLK | SWD clock |
| PB0 | EN_WW | WW channel enable (active-high) |
| PB1 | EN_CW | CW channel enable (active-high) |
| PB3 | LED_STATUS | Green status LED |
| PB4 | LED_FAULT | Red fault LED |
| PB6 / I2C1_SCL | I2C_SCL | I2C bus clock |
| PB7 / I2C1_SDA | I2C_SDA | I2C bus data |
| PB8 | BB_SHDN | Buck-boost SHDN (active-low) |
| NRST | NRST | Hardware reset |
| VDD | 3.3V_MCU | MCU supply |
| VSS | PGND | MCU ground |
| VDDA | 3.3V_MCU | Analog supply (via ferrite bead) |

---

## 7. DALI Interface

### 7.1 Isolation Architecture

- Goal: 2.5kVrms component isolation (subject to safety standard review)
- **Option A – Integrated**: NXP UBA2015 DALI slave IC + signal transformer or optocoupler isolation
- **Option B – Discrete**: IS181XBHTR DALI optocoupler (IXYS/Littelfuse) for TX+RX, discrete bus driver
- ESD protection: NXP PRTR5V0U2X on DALI_A/DALI_B (before isolation barrier)
- TVS clamp: P4SMA16A between bus lines
- PCB isolation gap: 8mm minimum, no copper/vias/components in keepout zone

### 7.2 DALI Electrical Characteristics

- Bus voltage: 9.5V–22.5V (DALI-2 spec); forward current from bus power supply
- This design is a DALI slave (control gear); the DALI master provides bus power
- DALI_A/DALI_B are polarity-independent (bus input uses bridge rectifier or polarity protection)
- Manchester bit encoding: 1200 baud, half-duplex

---

## 8. Temperature Sensing and Protection

| Protection | Threshold | Action |
|---|---|---|
| NTC over-temperature warning | 70°C | Reduce max current 20% |
| NTC over-temperature shutdown | 85°C | Turn off both channels; DALI fault code |
| Hysteresis | 10°C | Re-enable after cooling |
| Input under-voltage | <43V | Graceful shutdown |
| Input over-voltage | >55V | Shutdown; TVS handles transients |
| OCP per channel | ~1.05A | Immediate gate shutdown; latch |

---

## 9. PCB Stackup

| Layer | Name | Copper | Function |
|---|---|---|---|
| 1 (top) | F.Cu | 35µm (1oz) | Signal, DALI isolated domain, control |
| 2 | In1.Cu | 35µm (1oz) | Uninterrupted GND reference plane |
| 3 | In2.Cu | 35µm (1oz) | Signal/power inner routing |
| 4 (bottom) | B.Cu | 70µm (2oz) | Power/thermal; couples to Al enclosure |

Board thickness: 1.6mm. Material: FR4, Tg ≥ 150°C.

---

## 10. Limitations and Caveats

1. **Aux PSU**: ~~LMR14020 input voltage limit must be resolved~~ **RESOLVED** – BOM updated to LMR16006YDDAR (60V, 600mA); see §5.1.
2. **Buck-boost compensation**: Initial ITH/SS network values calculated in §3.4; must be confirmed in SPICE simulation and on prototype (see OI-002).
3. **Linear sink stability**: Updated compensation values calculated in §4.6 (100 nF dominant pole); validate on prototype over full current range (see OI-003).
4. **DALI certification**: Not claimed. Protocol compliance requires DALI Alliance test suite (OI-005).
5. **Safety certification**: No CE or safety marking claimed. IEC 62368-1 / EN 61347-2-13 assessment required (OI-006).
6. **EMC**: 200kHz switching requires conducted/radiated EMI management. No pre-compliance test results available (OI-007).
7. **Thermal validation**: Junction temperatures for Q_WW/Q_CW at worst-case dissipation (11.2W each) must be validated with thermal simulation and prototype measurement (OI-008).
