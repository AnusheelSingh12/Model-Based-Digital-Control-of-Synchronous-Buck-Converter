# Synchronous Buck Converter — Firmware Architecture Guide
## TI LaunchXL-F28379D | Cascaded PI Control | 100kHz

---

## 1. File Structure

```
buck_converter/
├── src/
│   ├── main.c          → System init, startup sequence
│   ├── epwm.c          → PWM generation, duty cycle update
│   ├── adc.c           → ADC configuration, scaling constants
│   ├── controller.c    → PI controller math, anti-windup
│   └── interrupts.c    → ADC ISR, complete control loop
├── include/
│   ├── epwm.h          → PWM defines, function prototypes
│   ├── adc.h           → ADC channel assigns, scaling, globals
│   ├── controller.h    → PI struct, gains, prototypes
│   └── interrupts.h    → ISR prototype
└── ARCHITECTURE.md     → This file
```

---

## 2. Control Flow

```
EPWM1 Counter = 0 (every 10us)
        ↓
SOCA Trigger → ADCA + ADCB sample simultaneously
        ↓
ADC Conversion (~300ns)
        ↓
ADCINT1 fires → ADC_ISR enters
        ↓
Read Vout raw, Iout raw
        ↓
Scale to V and A
        ↓
Voltage PI: error_v = Vref - Vout → Iref
        ↓
Current PI: error_i = Iref - Iout → duty
        ↓
EPWM_setDutyCycle(duty) → CMPA shadow register
        ↓
Clear ADCINT1 flag + PIE ACK Group 1
        ↓
ISR exits → CPU idle until next cycle
        ↓
Next counter zero: shadow CMPA → active CMPA
```

---

## 3. Simulink to Firmware Mapping

| Simulink Block              | Firmware Location              |
|-----------------------------|--------------------------------|
| ADC (Vout channel)          | `adc.c` ADCA SOC0              |
| ADC (Iout channel)          | `adc.c` ADCB SOC0              |
| ADC Scaling gain            | `g_vOut = raw * VOLTAGE_SCALE` |
| Voltage reference (12V)     | `g_vref = VREF` in controller.h|
| Voltage PI block            | `PI_update(&voltagePI, ...)`   |
| Current PI block            | `PI_update(&currentPI, ...)`   |
| Anti-windup saturation      | Integrator clamp in PI_update()|
| Duty cycle saturation       | EPWM_setDutyCycle() clamp      |
| PWM Generator               | `epwm.c` EPWM1 AQ config       |
| Sample Time (Ts = 10us)     | EPWM SOCA trigger, 100kHz      |

---

## 4. Key Register Reference

### ePWM Registers
| Register | Purpose | This Project |
|----------|---------|--------------|
| TBPRD    | Sets switching frequency | 1000 → 100kHz |
| CMPA     | Sets duty cycle | 50-950 (5%-95%) |
| TBCTL    | Counter mode | Up-Down |
| AQCTLA   | Output logic | HIGH up, LOW down |
| DBRED    | Rising edge dead time | 20 counts = 100ns |
| DBFED    | Falling edge dead time | 20 counts = 100ns |
| ETSEL    | ADC trigger source | Counter zero |
| ETPS     | Trigger prescale | Every cycle |

### ADC Registers
| Register | Purpose | This Project |
|----------|---------|--------------|
| ADCCTL2  | Clock prescaler | /4 = 50MHz |
| ADCSOC0CTL | Channel, trigger, window | ADCIN0, EPWM1SOCA, 15 cycles |
| ADCINTSEL | Interrupt source | SOC0 EOC |
| ADCRESULT0 | Conversion result | 0-4095 counts |

---

## 5. Watch Window Variables (Debug)

Add these to Expressions window in CCS:

| Variable      | Description                    | Expected at 12V/1A |
|---------------|--------------------------------|--------------------|
| `g_vOut`      | Measured output voltage (V)    | ~12.0              |
| `g_iOut`      | Measured output current (A)    | ~1.0               |
| `g_vref`      | Voltage reference (V)          | 12.0               |
| `g_iref`      | Current reference from Vloop   | ~1.0               |
| `g_duty`      | Duty cycle (normalized)        | ~0.5               |
| `g_error_v`   | Voltage error (V)              | ~0.0 at steady state|
| `g_error_i`   | Current error (A)              | ~0.0 at steady state|
| `g_adcVoutRaw`| Raw ADC count (debug)          | ~1858              |
| `g_adcIoutRaw`| Raw ADC count (debug)          | ~varies with sensor|

---

## 6. PI Gain Tuning Procedure

### Step 1: Start Open Loop
Comment out the PI calls in interrupts.c and set fixed duty:
```c
g_duty = 0.5f;  // Fixed 50% for testing
EPWM_setDutyCycle(g_duty);
```
Verify Vout ≈ 12V on scope. Verify ADC reads correct value.

### Step 2: Tune Current Loop First (Inner)
Re-enable current PI only. Set Iref manually:
```c
g_iref = 1.0f;  // 1A reference, fixed
g_duty = PI_update(&currentPI, g_iref, g_iout);
```
Increase CURRENT_KP until current waveform oscillates. Back off 50%.
Then increase CURRENT_KI until steady state error is zero.

### Step 3: Tune Voltage Loop (Outer)
Re-enable both loops. Start with very small VOLTAGE_KP (0.01).
Increase until Vout responds quickly to step changes in Vref.
Add VOLTAGE_KI to eliminate steady-state error.

### Step 4: Verify on Scope
- CH1: EPWM1A (GPIO0) — 100kHz square wave
- CH2: Vout — steady 12V with small ripple
- CH3: Inductor current (if current probe available)

---

## 7. Expected Converter Parameters

```
Duty Cycle (D)    = Vout/Vin = 12/24 = 0.5 (50%)
Inductor Ripple   = Vin*D*(1-D) / (L*Fsw) = 24*0.5*0.5 / (50e-6*100e3) = 1.2A
Capacitor Ripple  = ΔIL / (8*C*Fsw) = 1.2 / (8*12.5e-6*100e3) = 120mV
Output Current    = Vout/Rload = 12/12 = 1A
```

These are your theoretical verification targets on the oscilloscope.

---

## 8. Hardware Connections

```
LaunchPad Pin    Signal         Connect To
─────────────────────────────────────────────
GPIO0 (J4 Pin2)  EPWM1A HS PWM  Gate driver IN+ (High Side)
GPIO1 (J4 Pin4)  EPWM1B LS PWM  Gate driver IN- (Low Side)
ADCINA0          Vout feedback  Resistor divider output
ADCINB0          Iout feedback  Current sensor output
GND              Ground         Shared GND with converter
3.3V             Vref           Current sensor supply
```

---

## 9. Safety Checklist Before Powering Converter

- [ ] Dead band verified on scope (no shoot-through)
- [ ] Duty cycle limits confirmed (5% min, 95% max)
- [ ] Current limit in voltage PI output (IREF_MAX = 2A)
- [ ] ADC scaling verified (g_vOut reads correct voltage)
- [ ] Both gate drive signals present on scope before connecting power
- [ ] Start with Vin = 5V, verify Vout, then increase to 24V
