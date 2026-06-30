//#############################################################################
//
// FILE:    interrupts.c
//
// TITLE:   ADC ISR - Cascaded Control Loop Execution
//
// DESCRIPTION:
//   This ISR is the heart of the entire control system.
//   It executes at exactly 100kHz (once per PWM cycle) and performs:
//
//     1. Read ADC results (Vout, Iout)
//     2. Convert counts to physical units (V, A)
//     3. Run Voltage PI → compute Iref
//     4. Run Current PI → compute duty cycle
//     5. Update ePWM CMPA (apply new duty cycle)
//     6. Clear interrupt flags
//
// EXECUTION TIMELINE (within one 10us PWM period):
//
//   t=0us:   EPWM1 counter = 0 → SOCA fires → ADC starts sampling
//   t=0.3us: ADC conversion complete → ADCINT1 fires → ISR enters
//   t=0.3us: Read Vout, Iout raw counts
//   t=0.5us: Scale to physical units
//   t=1.0us: Voltage PI executes → Iref computed
//   t=1.5us: Current PI executes → duty computed
//   t=2.0us: CMPA updated (shadow register)
//   t=2.0us: Flags cleared → ISR exits
//   t=10us:  Next EPWM period begins, shadow → active register transfer
//
// SIMULINK MAPPING:
//   This entire ISR = one Simulink model step at Ts = 10us.
//   The execution order here mirrors the signal flow in your Simulink diagram:
//   ADC → Scale → Voltage PI → Current PI → Duty → PWM
//
//#############################################################################

#include "driverlib.h"
#include "device.h"
#include "adc.h"
#include "controller.h"
#include "epwm.h"
#include "interrupts.h"

//
// ADC_ISR - Main Control Loop ISR
//
// Triggered by: ADCA End-of-Conversion (SOC0 complete)
// Frequency:    100kHz (every PWM cycle)
// PIE Group:    Group 1 (ADCINT1 = INT_ADCA1)
//
__interrupt void ADC_ISR(void)
{
    //
    // =========================================================================
    // STEP 1: READ ADC RESULTS
    // =========================================================================
    //
    // ADC result registers are updated by hardware after conversion.
    // Reading immediately after EOC interrupt guarantees fresh data.
    //
    // ADCReadResult() returns 12-bit value (0-4095)
    // ADCA result 0 = voltage measurement
    // ADCB result 0 = current measurement
    //

    g_adcVoutRaw = ADC_readResult(ADCARESULT_BASE, ADC_VOUT_RESULT);
    g_adcIoutRaw = ADC_readResult(ADCBRESULT_BASE, ADC_IOUT_RESULT);

    //
    // =========================================================================
    // STEP 2: CONVERT ADC COUNTS TO PHYSICAL UNITS
    // =========================================================================
    //
    // Voltage:
    //   Vout = raw_count * VOLTAGE_SCALE
    //   VOLTAGE_SCALE = (Vref / 4095) * voltage_divider_ratio
    //   Example: count=1489 → 1489 * 0.006458 = 9.6V (not yet at 12V setpoint)
    //
    // Current:
    //   Iout = (raw_count * (3.3/4095) - 2.5V_offset) / 0.185V_per_A
    //   ACS712 output = 2.5V at 0A, shifts 185mV per Amp
    //   Example: count=2482 → (2482*0.000806 - 2.5) / 0.185 = 1.0A
    //
    // SIMULINK MAPPING:
    //   Replaces the ADC scaling gain blocks and offset blocks in Simulink.
    //

    g_vOut = (float32_t)g_adcVoutRaw * VOLTAGE_SCALE;

    g_iOut = ((float32_t)g_adcIoutRaw * CURRENT_SCALE - CURRENT_OFFSET)
             / CURRENT_SENSOR_SENS;

    //
    // Clamp current to zero (sensor may read slightly negative at no load)
    //
    if(g_iOut < 0.0f) g_iOut = 0.0f;

    //
    // =========================================================================
    // STEP 3: OUTER VOLTAGE LOOP (PI)
    // =========================================================================
    //
    // Input:  Vref (12.0V) and Vout (measured)
    // Output: Iref (current reference for inner loop)
    //
    // The voltage PI controller compares the measured output voltage to the
    // 12V reference. If Vout < 12V, it increases Iref to demand more current,
    // which eventually increases duty cycle and raises Vout.
    //
    // SIMULINK MAPPING:
    //   Voltage PI block in outer feedback loop.
    //   Iref = voltage PI output, fed as reference to inner current loop.
    //

    g_error_v = g_vref - g_vOut;               // Compute voltage error

    g_iref = PI_update(&voltagePI,              // Run voltage PI
                        g_vref,
                        g_vOut);

    //
    // =========================================================================
    // STEP 4: INNER CURRENT LOOP (PI)
    // =========================================================================
    //
    // Input:  Iref (from voltage loop) and Iout (measured)
    // Output: Duty cycle (normalized 0.0 to 1.0)
    //
    // The current PI controller is the inner (faster) loop.
    // It tracks the current reference from the voltage loop.
    // If Iout < Iref, duty cycle increases to push more current through inductor.
    //
    // WHY CASCADED?
    //   Direct voltage control (single loop) is slower and more sensitive to
    //   inductor dynamics. The current inner loop linearizes the plant for the
    //   outer voltage loop, improving bandwidth and transient response.
    //   This matches exactly your Simulink cascaded control model.
    //
    // SIMULINK MAPPING:
    //   Current PI block in inner feedback loop.
    //   Output feeds directly into the PWM generator duty cycle input.
    //

    g_error_i = g_iref - g_iOut;               // Compute current error

    g_duty = PI_update(&currentPI,              // Run current PI
                        g_iref,
                        g_iOut);

    //
    // =========================================================================
    // STEP 5: UPDATE PWM DUTY CYCLE
    // =========================================================================
    //
    // Convert duty cycle (0.0-1.0) to CMPA counts and write to shadow register.
    // Shadow register transfers to active register at next counter zero.
    // This ensures the new duty cycle takes effect cleanly at the next period.
    //
    // CMPA = duty * TBPRD = duty * 1000
    // Example: duty = 0.5 → CMPA = 500 → 50% duty cycle
    //
    // SIMULINK MAPPING:
    //   Replaces the "Duty to CMPA" conversion block and the
    //   PWM generator's shadow register update mechanism.
    //

    EPWM_setDutyCycle(g_duty);

    //
    // =========================================================================
    // STEP 6: CLEAR INTERRUPT FLAGS
    // =========================================================================
    //
    // Two flags must be cleared:
    //
    // 1. ADCA Interrupt Flag:
    //    If not cleared, ADCINT1 will not fire again next cycle.
    //    The ADC module sets this flag on EOC and holds it until cleared.
    //
    // 2. PIE Group 1 ACK:
    //    The PIE controller groups 8 interrupts per CPU INT line.
    //    After an ISR from a group runs, the PIE holds that group's
    //    acknowledge bit until software clears it.
    //    If not cleared: no further interrupts from Group 1 will fire.
    //    This is the most common bug in C2000 interrupt code.
    //

    ADC_clearInterruptStatus(ADC_VOUT_BASE, ADC_INT_NUMBER1);
    // Clear ADCINT1 flag in ADCA — allows next EOC to trigger interrupt

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
    // Clear PIE Group 1 ACK — allows next Group 1 interrupt to be serviced
    // ADCINT1 is in PIE Group 1, INT 1.1
}

//
// End of File
//
