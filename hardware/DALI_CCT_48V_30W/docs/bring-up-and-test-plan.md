# Bring-Up and Test Plan – DALI CCT 48V 30W Rev A

> **Safety Warning**: This board operates at 48V DC and may have live conductors at up to 52.8V. Use appropriate insulation, isolation barriers, current-limited bench power supplies, and PPE during all testing. Have a means to immediately cut power.

---

## Pre-Power Checks (Assembled PCB, No Power Applied)

### Step 1: Visual Inspection
- [ ] All SMD components correctly oriented (polarized caps, ICs, MOSFETs)
- [ ] No solder bridges at fine-pitch ICs (LT8390A, STM32G031, MCP4728)
- [ ] Through-hole connectors J1–J4 fully seated and soldered
- [ ] No flux residue bridging isolation keepout zone
- [ ] Isolation barrier (x=22–30mm) visually clear of all conductors

### Step 2: Resistance Checks (DMM, unpowered)
| Measurement | Expected | Action if fail |
|---|---|---|
| 48V_IN+ to PGND | > 1kΩ (no short) | Check F1, D1, C1/C2 |
| 3.3V_MCU to PGND | > 100Ω | Check U_LDO output |
| LED_BUS+ to PGND | > 1kΩ | Check C4–C6 |
| WW- to PGND | > 1kΩ (Q_WW off) | Check Q_WW gate |
| CW- to PGND | > 1kΩ (Q_CW off) | Check Q_CW gate |
| DALI_A/B to PGND | > 10kΩ | Check isolation barrier |
| DALI_GND to PGND | > 100kΩ | Verify galvanic isolation |

---

## Stage 1: Auxiliary Power Supply Bring-Up

**Equipment**: Bench PSU (current limited to 100mA), DMM

### Step 1.1: First Power-On
1. Set bench PSU to 48V, current limit 100mA.
2. Connect to J1 (48V_IN+ / 48V_IN-).
3. Apply power; observe current draw.
4. **Expected**: < 50mA at idle (no load on LEDs).

### Step 1.2: Aux Rail Verification
| Measurement | Expected | Tolerance |
|---|---|---|
| TP_5V (5V_AUX) | 5.0V | ±2% (4.9–5.1V) |
| TP_3V3 (3.3V_MCU) | 3.3V | ±2% (3.23–3.37V) |
| TP_5VDALI (5V_DALI) | 5.0V | ±5% |

- If any rail missing: check respective U_AUX1 / U_LDO / isolated supply circuit.

### Step 1.3: Input Voltage Monitor
1. Verify MCU ADC reading via SWD debug:
   - ADC_VIN should read proportional to 48V input (after divider)
   - Expected code: ~48V × (38.3k / (750k + 38.3k)) / 3.3V × 4096 ≈ 764 LSB

---

## Stage 2: MCU Firmware Load and Basic Verification

**Equipment**: ST-LINK V3 (or equivalent SWD programmer), STM32CubeProgrammer

### Step 2.1: Firmware Programming
1. Connect ST-LINK to J4 (SWD: GND, SWDIO, SWDCLK, 3.3V).
2. Open STM32CubeProgrammer; detect STM32G031K8T6.
3. Program firmware binary.
4. Verify CRC on programmed flash.

### Step 2.2: MCU Basic Operation
1. After programming, verify:
   - LED_STATUS (green, PB3) blinks at ~1Hz (firmware alive indicator)
   - LED_FAULT (red, PB4) is OFF
2. Via SWD debug console, verify:
   - Firmware version string readable
   - NVM (AT24C32E) accessible on I2C (I2C address 0x50)
   - MCP4728 DAC accessible on I2C (I2C address 0x60)
   - ADC readings: NTC, VIN, VBUS within expected ranges

---

## Stage 3: Buck-Boost LED Bus Bring-Up

**Equipment**: Bench PSU 48V/2A, DMM, oscilloscope (100MHz min), electronic load

> ⚠️ Do not connect LED loads during this stage.

### Step 3.1: Pre-Enable Check
1. Verify MCU holds BB_SHDN low (buck-boost disabled).
2. Measure: LED_BUS+ = 0V (no output).

### Step 3.2: Enable Buck-Boost (No Load)
1. Via firmware command (or SWD debug), enable buck-boost (BB_SHDN = high).
2. Measure LED_BUS+ with DMM.
   - **Expected**: 44V ± 1V
3. Measure with oscilloscope at LED_BUS+ (10:1 probe):
   - Ripple < 200mV pk-pk at 200kHz
   - No oscillation or instability

### Step 3.3: Load Test (Electronic Load)
1. Connect electronic load to LED_BUS+ / PGND.
2. Step load from 0W to 10W to 20W to 30W.
3. For each step:
   - Record LED_BUS+ voltage (expected: 44V ±2V across load range)
   - Record input current (expected: proportional to P_out / (Vin × η))
   - Record ripple (< 500mV at full load)
4. Measure efficiency: η = P_out / P_in

---

## Stage 4: Current Sink Channel Bring-Up

**Equipment**: Bench PSU for LED_BUS+ supply, current meter in LED string, oscilloscope

### Step 4.1: WW Channel Test
1. Connect LED load (or resistive dummy load: 60Ω/25W between LED_BUS+ and WW-).
2. Via firmware DAC command, ramp WW current from 0 to 100mA in 10mA steps.
3. For each step:
   - Measure WW- current with meter (verify matches DAC setpoint)
   - Measure TP_SENSE_WW voltage (expected: I × 0.1Ω, e.g. 10mV at 100mA)
   - Verify linear relationship between DAC code and current

### Step 4.2: CW Channel Test
- Repeat Step 4.1 for CW channel (Q_CW, TP_SENSE_CW, DAC CW channel).

### Step 4.3: Full Range Test (0.1% to 100%)
1. Sweep WW from minimum (0.1% = ~0.7mA target) to 700mA.
2. Log actual measured current vs. DAC code.
3. Calculate linearity error; acceptable < 2% FSR.

### Step 4.4: OCP Test
1. Set WW current to 600mA.
2. Briefly short WW- to PGND (simulate LED short).
3. Verify: OCP comparator trips within < 1ms; WW channel turns off; MCU sets fault flag.
4. Remove short; reset fault via firmware; verify WW channel recovers.

---

## Stage 5: DALI Interface Verification

**Equipment**: DALI master controller or DALI analyser (e.g., Lunatone DALI Bridge, Tridonic MasterControl), DALI 2-wire bus.

### Step 5.1: Bus Physical Layer
1. Connect DALI master to J2 (DALI_A / DALI_B).
2. Verify DALI bus voltage at TP_DALI_A / TP_DALI_B: 9.5–22.5V.
3. Oscilloscope: verify Manchester encoding of MCU transmission.
4. Verify bus polarity independence (swap DALI_A/B; device must still respond).

### Step 5.2: DALI-2 Addressing
1. Send DALI broadcast "INITIALISE" (0xFF00), "RANDOMISE" (0xFF07).
2. Execute binary search addressing: verify device accepts short address.
3. Query address: QUERY SHORT ADDRESS → device responds with assigned address.

### Step 5.3: DT8 Tc Functionality
1. Send SET COLOUR TEMPERATURE TC command (per IEC 62386-209).
2. Vary Tc from minimum to maximum (e.g., 153 mirek = 6536K to 370 mirek = 2703K).
3. Verify WW/CW current changes proportionally; verify CCT endpoint behavior.
4. Query QUERY COLOUR VALUE: verify response matches commanded value.

### Step 5.4: DALI Bus Loss Test
1. Apply running dim/CCT state.
2. Disconnect DALI bus (remove J2 connection).
3. Wait 10 seconds.
4. Verify LED output remains constant at last valid level.

### Step 5.5: Power Cycle Memory Test
1. Set specific dim level and CCT.
2. Remove and restore 48V power.
3. Verify output returns to the dim level and CCT that were set.

---

## Stage 6: Thermal Validation

**Equipment**: Thermocouple or thermal camera, DMM, oscilloscope, bench PSU with current meter, environmental chamber (optional, recommended for 50°C ambient test).

### Step 6.1: Steady-State Thermal – Fixed Bus Voltage

1. **Disable adaptive bus voltage** in firmware (set V_bus = fixed 44V for this test).
2. Run WW channel at worst-case: I_WW = 700mA with V_LED_forward ≈ 28V (use resistive dummy load: R = 40Ω / 25W connected from LED_BUS+ to WW−).
3. Allow 30 minutes to reach steady state.
4. Measure Q_WW case temperature (thermocouple on D2PAK top surface).
5. **Pass criterion (fixed bus, 25°C ambient)**: T_case < 100°C (expected ≈ 80°C from calc).
6. **Warning criterion**: T_case > 110°C → increase thermal via count or add TIM.
7. Repeat for CW channel.

### Step 6.2: Steady-State Thermal – Adaptive Bus Voltage

1. **Enable adaptive bus voltage** firmware (V_bus = V_LED + 2.5V headroom).
2. Repeat Step 6.1 with same conditions.
3. Verify V_bus drops from 44V to ~30.5V when connected to 28V dummy load.
4. **Pass criterion (adaptive bus, 25°C ambient)**: T_case < 40°C (expected ≈ 35°C from calc; Pdiss ≈ 1.75W).
5. **Pass criterion (adaptive bus, 50°C ambient chamber)**: T_case < 60°C (Tj < 80°C).
6. **Fail criterion**: Tj > 100°C at 50°C ambient → review thermal via placement; increase to 12+ vias.

### Step 6.3: NTC Response Test

1. Heat PCB with hot-air gun near NTC1.
2. Verify firmware reads increasing temperature.
3. Verify warning threshold at 70°C: current reduced by 20%.
4. Verify shutdown at 85°C: both channels turn off; LED_FAULT activates.

### Step 6.4: Thermal Camera Survey

1. Run both channels at 700mA (adaptive bus voltage active) for 30 minutes.
2. Take thermal camera image of B.Cu (bottom side).
3. Verify thermal spread across 20×20mm Cu pours; hotspot should be on D2PAK tab, not on traces.
4. Verify enclosure case temperature < 70°C at worst-case condition (50°C ambient).

---

## Stage 7: EMC Pre-Compliance (Optional at Prototype Stage)

1. Conducted emissions scan (9kHz–30MHz) per CISPR 15 / EN 55015 limits.
2. Radiated emissions scan (30MHz–1GHz).
3. Expected problematic frequencies: 200kHz switching + harmonics.
4. If failures observed: add additional input filtering, adjust layout, evaluate spread-spectrum option on LT8390A.

---

## Acceptance Criteria Summary

| Test | Pass Criterion |
|---|---|
| Aux rails | 5V ±2%, 3.3V ±2% |
| LED bus voltage | 44V ±2V at 0W–30W |
| LED bus efficiency | ≥ 88% at 20W |
| Current accuracy | ±3% of setpoint, 1mA–700mA |
| Ripple (LED current) | < 5% pk-pk of setpoint |
| DALI addressing | Responds correctly to short-address assignment |
| DALI DT8 Tc | CCT commands accepted; WW/CW mix correct |
| DALI bus loss | Output holds for ≥ 60 seconds without change |
| Power cycle memory | Output restored within 1 second of power-on |
| OCP | Trips within 1ms at 1.05A; auto-recovers after 100ms |
| NTC shutdown | Channels off at 85°C ±5°C |
| Thermal (Q_WW/CW) | T_package < 100°C at 25°C ambient, full load |
