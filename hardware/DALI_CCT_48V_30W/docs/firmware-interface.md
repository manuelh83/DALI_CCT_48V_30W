# Firmware Interface Documentation – DALI CCT 48V 30W Rev A

---

## 1. Overview

The firmware runs on a STM32G031K8T6 (ARM Cortex-M0+, 64MHz, 64KB flash, 8KB SRAM). It implements:

- DALI-2 control gear behavior (IEC 62386-101 and -102)
- DALI-2 Device Type 8 – Tunable White Tc (IEC 62386-209)
- Analog current control of two LED channels (WW and CW) via I2C DAC (MCP4728)
- NVM management via I2C EEPROM (AT24C32E)
- Hardware fault monitoring and protection
- Thermal management via NTC ADC

---

## 2. MCU Pin Roles

| Pin | Direction | Net | Function | Notes |
|---|---|---|---|---|
| PA0 | ADC in | ADC_NTC | NTC temperature sense | 12-bit ADC; divider: 100kΩ pull-up to 3.3V, NTC to GND |
| PA1 | ADC in | ADC_VIN | 48V input voltage monitor | Divider: 750kΩ / 38.3kΩ → ~0.048×Vin |
| PA2 | ADC in | ADC_VBUS | LED bus voltage monitor | Divider: 750kΩ / 38.3kΩ → ~0.048×Vbus |
| PA4 | DAC out | MCU_DAC_WW | WW current setpoint (backup) | 12-bit DAC; only used if MCP4728 I2C fails |
| PA5 | DAC out | MCU_DAC_CW | CW current setpoint (backup) | 12-bit DAC backup |
| PA6 | GPIO in | OCP_WW | WW over-current latch signal | Active-high from LM393 comparator |
| PA7 | GPIO in | OCP_CW | CW over-current latch signal | Active-high from LM393 comparator |
| PA9 | USART1_TX | DALI_TX | DALI transmit (to optocoupler) | Half-duplex; open-drain recommended |
| PA10 | USART1_RX | DALI_RX | DALI receive (from optocoupler) | Pull-up to 3.3V |
| PA13 | SWD | SWDIO | SWD data line | Programming and debug |
| PA14 | SWD | SWDCLK | SWD clock | Programming and debug |
| PB0 | GPIO out | EN_WW | WW channel enable | Active-high; drives NAND latch for Q_WW gate |
| PB1 | GPIO out | EN_CW | CW channel enable | Active-high |
| PB3 | GPIO out | LED_STATUS | Green status LED | Active-high; 1kΩ series |
| PB4 | GPIO out | LED_FAULT | Red fault LED | Active-high; 1kΩ series |
| PB6 | I2C1_SCL | I2C_SCL | I2C bus clock | 400kHz; 4.7kΩ pull-up to 3.3V |
| PB7 | I2C1_SDA | I2C_SDA | I2C bus data | 400kHz; 4.7kΩ pull-up to 3.3V |
| PB8 | GPIO out | BB_SHDN | Buck-boost enable/shutdown | Active-low shutdown; high = running |
| NRST | Reset | NRST | Hardware reset | 100nF to GND; external reset pin |

### I2C Device Addresses

| Device | I2C Address | Function |
|---|---|---|
| MCP4728 | 0x60 | 12-bit quad DAC for WW/CW setpoints |
| AT24C32E | 0x50 | EEPROM for DALI NVM |

---

## 3. DALI Protocol Implementation

### 3.1 Standards Targeted

| Standard | Scope |
|---|---|
| IEC 62386-101 | DALI-2 general requirements, electrical interface |
| IEC 62386-102 | Control gear (slave) general behavior |
| IEC 62386-209 | Device type 8: colour control (Tunable White Tc) |

**Note**: DALI-2 certification is not claimed. The firmware is designed to follow the above standards but must pass official DALI Alliance test suites before the product may be labeled as DALI-2 certified.

### 3.2 DALI Physical Layer

- Baud rate: 1200 baud (Manchester encoding)
- Bit period: 833µs; half-bit period: 416µs
- Bus voltage: 9.5V–22.5V (sourced by DALI master)
- Transmission: MCU drives optocoupler TX open-collector pull-down
- Reception: optocoupler RX drives MCU PA10 (USART1_RX)
- USART configuration: 1200 baud, 8N1, single-wire half-duplex mode; Manchester coding implemented in software via bit-banging on TIM1 or via hardware USART in Manchester mode (STM32G0 supports hardware Manchester)

### 3.3 DALI Device Behavior (IEC 62386-102)

The firmware must implement the following command sets:

**Addressing and Initialization**
- INITIALISE, RANDOMISE, COMPARE, WITHDRAW, SEARCH ADDRESS, PROGRAM SHORT ADDRESS, VERIFY SHORT ADDRESS, QUERY SHORT ADDRESS
- Support for broadcast, group, and individual short address

**Arc Power Level Control**
- DAPC (Direct Arc Power Control): 0–254 arc-power levels
- ON, OFF, RECALL MAX LEVEL, RECALL MIN LEVEL, STEP UP, STEP DOWN
- GO TO SCENE (16 scenes: 0–15)
- SET MAX LEVEL, SET MIN LEVEL, SET SYSTEM FAILURE LEVEL, SET POWER ON LEVEL
- Fade: FADE TIME (0–15), FADE RATE (1–15), NON-FADE TIME
- QUERY commands: QUERY STATUS, QUERY CONTROL GEAR PRESENT, QUERY LIGHT SOURCE, QUERY ACTUAL LEVEL, QUERY MAX/MIN/POWER-ON/SYSTEM-FAILURE LEVEL, QUERY FADE TIME/RATE, QUERY SCENE LEVEL

**DALI Arc-Power Dimming Curve**:
```
If arc_power_level == 0: output = OFF (current = 0)
If arc_power_level == 1: output = MIN_LEVEL (firmware default: ~0.1% = ~0.7mA at 700mA FSR)
If arc_power_level == 254: output = MAX_LEVEL (firmware default: 100% = full current)
Physical_current = max_current × (10 ^ ((arc_power_level - 254) / 253 × log10(min/max)))
```
The logarithmic curve maps 254 steps across a configurable dimming range.

### 3.4 DT8 Tc Implementation (IEC 62386-209)

**Colour Temperature Control**

Commands implemented:
- SET COLOUR TEMPERATURE TC (value in mirek = 1,000,000 / Kelvin)
- QUERY COLOUR VALUE: Colour Temperature TC (current and target)
- QUERY COLOUR TEMPERATURE TC COOLEST, QUERY COLOUR TEMPERATURE TC WARMEST
- SET COLOUR TEMPERATURE TC COOL BOUND, SET COLOUR TEMPERATURE TC WARM BOUND

Default Tc range: 153 mirek (6536K) to 370 mirek (2703K). Configurable via NVM.

**WW/CW Mixing Algorithm**

Given a target colour temperature Tc_target and a total current I_total derived from arc-power level:

```c
// Normalise: 0.0 = coolest (Tc_cool), 1.0 = warmest (Tc_warm)
float alpha = (Tc_target - Tc_cool) / (Tc_warm - Tc_cool);
alpha = clamp(alpha, 0.0f, 1.0f);

// Linear mix (default; replace with calibrated table for accuracy — see OI-009)
float I_WW = I_total * alpha;
float I_CW = I_total * (1.0f - alpha);

// Power limit enforcement
float v_bus = adc_to_volts(ADC_VBUS);
float p_est = v_bus * (I_WW + I_CW);
if (p_est > P_MAX_LIMIT) {
    float scale = P_MAX_LIMIT / p_est;
    I_WW *= scale;
    I_CW *= scale;
}
```

**Calibrated mixing table (production implementation):**

For accurate CCT control, the linear approximation above should be replaced with a calibrated lookup table derived from photometric measurement of the actual LED module. The calibration procedure is:

1. Measure CIE 1931 (x, y) chromaticity of WW LED at 100 %, 50 %, 10 % current; record lm/mA.
2. Measure CIE 1931 (x, y) chromaticity of CW LED at 100 %, 50 %, 10 % current; record lm/mA.
3. For each target Tc in the operating range (e.g., every 100 mirek), solve the 2-primary mixing equations:

```
x_mix = (I_WW × lm_WW × x_WW + I_CW × lm_CW × x_CW) / (I_WW × lm_WW + I_CW × lm_CW)
y_mix = (I_WW × lm_WW × y_WW + I_CW × lm_CW × y_CW) / (I_WW × lm_WW + I_CW × lm_CW)
```

   Subject to: x_mix, y_mix lie on the Planckian locus at the target Tc.

4. Build a lookup table: `uint16_t ww_ratio_table[TC_STEPS]` where the value represents I_WW / I_total × 4095.
5. Store lookup table in MCU flash. At runtime, interpolate linearly between table entries.
6. Verify gamut: confirm the LED pair can achieve all Tc values in [Tc_cool, Tc_warm]; document the achievable gamut.

```c
// Production mixing with lookup table
uint16_t idx     = tc_to_table_index(Tc_target);     // mirek → table index
float    ratio   = ww_ratio_table[idx] / 4095.0f;    // WW fraction
float    I_WW    = I_total * ratio;
float    I_CW    = I_total * (1.0f - ratio);
```

**Note**: Linear interpolation assumes equal efficacy per mA for WW and CW. Real LEDs differ in lumens/mA and correlated colour temperature vs. current. The calibrated lookup table is recommended for production.

---

## 4. NVM Behavior (EEPROM AT24C32E)

### 4.1 DALI NVM Requirements

The following parameters must be stored in non-volatile memory per IEC 62386-102/-209:

| Parameter | Size | Default |
|---|---|---|
| Short address | 1 byte | 0xFF (unassigned) |
| Group membership (groups 0–15) | 2 bytes | 0x0000 |
| Scene table: dim levels (16 × 1 byte) | 16 bytes | 0xFF (MASK = no action) |
| Scene table: CCT values (16 × 2 bytes) | 32 bytes | 0x0000 (no-change) |
| MAX LEVEL | 1 byte | 0xFE (254) |
| MIN LEVEL | 1 byte | 0x01 (1) |
| POWER ON LEVEL | 1 byte | 0xFF (LAST ACTIVE) |
| SYSTEM FAILURE LEVEL | 1 byte | 0xFF (LAST ACTIVE) |
| FADE TIME | 1 byte | 0 (no fade) |
| FADE RATE | 1 byte | 7 |
| Physical min level | 1 byte | 0x01 |
| Device type | 1 byte | 0x08 (DT8) |
| Tc cool bound (mirek) | 2 bytes | 153 |
| Tc warm bound (mirek) | 2 bytes | 370 |
| Last active arc-power level | 1 byte | 0xFE |
| Last active Tc (mirek) | 2 bytes | 261 (3831K neutral) |

Total static NVM: ~70 bytes. With wear-leveling overhead, allocate 512 bytes minimum.

### 4.2 Wear-Leveling Strategy

- Static DALI config (address, groups, scenes): written only on explicit DALI write commands. Expected: < 1000 writes per lifetime. No wear-leveling needed; write to fixed page.
- Dynamic state (last arc-power, last Tc): written on state change after a 5-second debounce timer. Circular buffer of 64 slots × 4 bytes = 256 bytes. Rotates write pointer on each write.
  - At 1M page-write endurance per slot × 64 slots = 64M writes before wear-out
  - At worst case 10 writes/hour × 8760 hours/year = 87,600 writes/year → >730 years effective life
- Firmware reads back NVM after write and verifies correctness (read-after-write check).

### 4.3 Power-On State Restoration

On power-up:
1. Read POWER ON LEVEL from NVM.
2. If POWER ON LEVEL == 0xFF (LAST ACTIVE): read last-active dim level and Tc from circular buffer.
3. Ramp outputs from 0 to target values over FADE TIME (if non-zero).
4. If NVM read fails (I2C error, CRC fail): use firmware-compiled defaults (50% dim, 4000K).

On DALI bus loss (no valid DALI frame received for >500ms):
- Maintain current output without change.
- Set "LAMP FAILURE" status bit (query will return it cleared once bus resumes).

---

## 5. Brightness and CCT to WW/CW Current Mapping

### 5.1 Current Scale

- Maximum hardware current per channel: 700 mA
- Maximum firmware-enforced current per channel: **600 mA** (firmware constant `I_MAX = 600mA`)
- Maximum total firmware-enforced power: firmware enforces P_total ≤ 28W
- DAC full scale: MCP4728 = 4096 counts → 0–1.0V → 0–700mA through op-amp control loop

```
DAC_WW_code = round(I_WW_target / I_MAX * 4095)
DAC_CW_code = round(I_CW_target / I_MAX * 4095)
```

Where `I_MAX = 600mA` (firmware constant; hardware OCP set to 900mA absolute limit).

### 5.2 Power Limiting

The bus voltage is fixed at 44 V (LT8390A setpoint; not adjusted by firmware). Power limiting is enforced by capping total current:

```c
// Fixed bus voltage: no ADC_VBUS monitoring for adaptive control
const float V_BUS_FIXED = 44.0f;   // V; fixed by hardware feedback divider
const float P_MAX_LIMIT = 28.0f;   // W; 28W leaves headroom below 30W spec

float p_est = V_BUS_FIXED * (i_ww_set + i_cw_set);
if (p_est > P_MAX_LIMIT) {
    float scale = P_MAX_LIMIT / p_est;
    i_ww_set *= scale;
    i_cw_set *= scale;
}
```

ADC_VBUS (PA2) is still read for monitoring and fault detection (over/under-voltage), but is **not** used to adjust the bus voltage setpoint. The LT8390A feedback divider fixes V_bus at 44 V.

### 5.3 Fade Implementation

DALI fade timer runs at 100Hz (every 10ms). On each tick during a fade:

```
target_arc = commanded arc-power level
target_tc  = commanded Tc

// Dimming fade
if (current_arc != target_arc):
    step = fade_step_per_tick(fade_time)    // from DALI fade table
    current_arc = approach(current_arc, target_arc, step)

// Tc fade (simultaneous with dimming fade)
if (current_tc != target_tc):
    tc_step = fade_step_per_tick(fade_time)
    current_tc = approach(current_tc, target_tc, tc_step)

// Compute and update DAC
update_currents(current_arc, current_tc)
```

Fade occurs simultaneously for arc-power and colour temperature.

### 5.4 Minimum Dimming Level Analysis (OI-010)

The specification requires 0.1 % dimming (1:1000 ratio), corresponding to 0.7 mA at 700 mA full scale.

**Error budget at minimum current:**

| Error source | Value | Current error at 100 mΩ sense |
|---|---|---|
| OPA2333 input offset (max, 25°C) | 10 µV | 0.10 mA |
| OPA2333 input offset drift (max, −40 to +85°C) | 0.05 µV/°C × 60°C | +0.03 mA |
| MCP4728 zero-code output (max) | 0.5 mV | 5.0 mA |
| MCP4728 INL (max, 12-bit) | 2 LSB = 0.488 mV | 0.49 mA |
| PCB leakage / sense resistor offset | < 10 µV | < 0.10 mA |
| **Total (worst-case, RSS)** | | **≈ 5.1 mA** |

The dominant error is the **MCP4728 zero-code offset** (~0.5 mV at DAC output = 0). The offset is trimmed per device; typical offset is < 0.1 mV.

**Achievable minimum current (typical):** 0.7 mA (MCP4728 typical offset 0.1 mV → 1 mA total error; firmware zeros DAC and cuts EN_WW/EN_CW to achieve true off below 0.5 mA).

**Mitigation for worst-case parts:**
1. **Firmware offset compensation**: At start-up, characterise per-unit DAC offset by measuring V_SENSE at DAC code 0 with channel enabled; store correction in NVM; subtract from all DAC codes.
2. **Dual-range option (if 0.1 % not achievable)**: Below arc-power level 10 (≈ 1 %), enable PWM blanking: pulse EN_WW/EN_CW at 1 kHz with duty cycle proportional to dim level. This extends effective range to 1:10 000. Ensure PWM frequency ≥ 1 kHz to avoid visible flicker.
3. **Sense resistor increase**: Replacing 100 mΩ with 200 mΩ halves the offset-referred error at cost of 2× higher minimum voltage drop (140 mV at 700 mA); acceptable if MOSFET Vds headroom permits.

**Recommendation**: Implement firmware offset compensation (option 1) first; evaluate on prototype. Dual-range PWM (option 2) is a fallback if offset compensation is insufficient at temperature extremes.

---

## 6. Fault Handling

### 6.1 Fault Types

| Fault | Detection | Action | Recovery |
|---|---|---|---|
| OCP WW channel | PA6 high (LM393) | EN_WW = 0 immediately; log fault; set LAMP FAILURE | MCU resets after 100ms; 3 retry attempts; permanent off after 3 fails |
| OCP CW channel | PA7 high (LM393) | EN_CW = 0 immediately; log fault | Same as WW |
| Over-temperature warn (70°C) | ADC_NTC > threshold | Reduce I_MAX by 20% (600mA → 480mA); set LED_FAULT blink pattern | Auto-clear when temp < 60°C |
| Over-temperature shutdown (85°C) | ADC_NTC > threshold | EN_WW = 0; EN_CW = 0; BB_SHDN = 0; LED_FAULT solid | Auto-restart when temp < 75°C |
| Input under-voltage (<43V) | ADC_VIN < threshold | BB_SHDN = 0 (shutdown converter); channels off | Auto-enable when Vin > 44V |
| Input over-voltage (>55V) | ADC_VIN > threshold | BB_SHDN = 0 immediately | Auto-enable when Vin < 53V |
| I2C DAC failure | I2C NACK from MCP4728 | Switch to MCU internal DAC (PA4/PA5); log fault | Retry I2C init on next power cycle |
| NVM read failure | I2C NACK or CRC fail | Use defaults; log fault; LED_FAULT blink | Retry on power cycle |

#### NTC-based Current Limiting (Thermal Management)

The NTC thermistor (NTC1, 100kΩ B3950, mounted near Q_WW/Q_CW on B.Cu thermal pour) is read via ADC_NTC (PA0) every 500 ms. Current limiting is purely **current-based** – the bus voltage (44 V fixed) is never adjusted by firmware.

```c
// Thermal current-limiting state machine (no Vbus modification)
float pcb_temp_c = ntc_adc_to_celsius(adc_read(ADC_NTC));

if (pcb_temp_c >= NTC_SHUTDOWN_TEMP) {       // 85°C
    i_max_thermal = 0.0f;                    // total shutdown
    set_fault(FAULT_OVERTEMP_SHUTDOWN);
} else if (pcb_temp_c >= NTC_WARN_TEMP) {    // 70°C
    i_max_thermal = I_MAX_NOMINAL * 0.80f;   // 600mA × 0.80 = 480mA
    set_fault(FAULT_OVERTEMP_WARN);
} else if (pcb_temp_c < NTC_WARN_TEMP - NTC_HYSTERESIS) {  // < 60°C
    i_max_thermal = I_MAX_NOMINAL;           // 600mA; normal operation
    clear_fault(FAULT_OVERTEMP_WARN);
}

// Apply thermal limit before DAC update
float i_ww_clamped = fminf(i_ww_set, i_max_thermal);
float i_cw_clamped = fminf(i_cw_set, i_max_thermal);
```

**Key constants:**
- `I_MAX_NOMINAL = 600mA` – firmware maximum under normal conditions
- `NTC_WARN_TEMP = 70°C` – triggers 20% current reduction
- `NTC_SHUTDOWN_TEMP = 85°C` – triggers full shutdown
- `NTC_HYSTERESIS = 10°C` – re-enable hysteresis (warn clears at 60°C, shutdown at 75°C)

**Important**: This is purely current-limiting. The firmware does **not** read `ADC_VBUS` to adjust the LT8390A output voltage. The bus remains at a fixed 44 V at all times.

### 6.2 DALI Fault Reporting

The DALI QUERY STATUS command returns:
- Bit 0: Ballast failure (set if hardware fault prevents operation)
- Bit 1: Lamp failure (set if LED OCP latch is active)
- Bit 2: Lamp arc power on (set if arc-power > 0)
- Bit 3: Limit error (set if requested level was clipped by MIN or MAX)
- Bit 4: Fade running
- Bit 5: Reset state (set after power-on before any DALI command received)
- Bit 6: Missing short address
- Bit 7: Power failure (set after unexpected power-off detected via NVM)

---

## 7. Test and Programming Interface

### 7.1 SWD Interface (J4)

| Pin | Signal | Notes |
|---|---|---|
| 1 | GND | Reference |
| 2 | SWDIO | SWD data; also TP_SWDIO test pad |
| 3 | SWDCLK | SWD clock; also TP_SWDCLK test pad |
| 4 | 3.3V | Target VCC (powered by board) |

Compatible debuggers: ST-LINK V2, ST-LINK V3, J-Link, CMSIS-DAP.
Connector: 1.27mm pitch 4-pin (e.g., Samtec FTSH-104-01-L-DV) or solder pads.

### 7.2 Factory Test Procedure

1. Connect ST-LINK to J4.
2. Program production firmware binary.
3. Run built-in self-test (BIST) via UART or SWD:
   - Verify I2C DAC responds (MCP4728 at 0x60)
   - Verify EEPROM responds (AT24C32E at 0x50)
   - Verify ADC reads (NTC, VIN, VBUS) within range
   - Verify DALI TX/RX loopback (if test loopback jumper installed)
4. Program DALI factory defaults to EEPROM.
5. Disconnect ST-LINK.

### 7.3 BOOT0 Solder Jumper (SJ1)

- SJ1 open (default): BOOT0 = GND → boot from flash (normal operation)
- SJ1 closed: BOOT0 = VCC → boot from system memory (STM32 USB DFU bootloader)
- DFU mode allows firmware update via USB (if USB physical connector added) or UART bootloader via PA9/PA10

### 7.4 Firmware Build Environment

Recommended toolchain:
- Compiler: GCC ARM Embedded (arm-none-eabi-gcc), version 12 or later
- IDE: STM32CubeIDE or Visual Studio Code with Cortex-Debug extension
- HAL: STM32CubeG0 HAL library
- Build system: CMake or STM32CubeIDE project
- DALI stack: custom implementation or adapt open-source DALI library (verify license compatibility)
  - Reference: DALI-2 library for STM32 (community projects exist; verify completeness for DT8 Tc)
