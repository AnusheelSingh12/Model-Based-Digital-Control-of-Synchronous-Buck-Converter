//#############################################################################
//
// FILE:    epwm.c
//
// TITLE:   ePWM Module - Synchronous Buck Converter
//
// DESCRIPTION:
//   Configures EPWM1 for synchronous buck converter switching at 100kHz.
//
// EPWM1 SUBMODULES USED:
//   TB  - Time Base        : Sets switching frequency (TBPRD)
//   CC  - Counter Compare  : Sets duty cycle (CMPA)
//   AQ  - Action Qualifier : Defines output logic (HIGH/LOW on compare match)
//   DB  - Dead Band        : Shoot-through protection between HS and LS FETs
//   ET  - Event Trigger    : Triggers ADC at counter zero
//
// SIMULINK MAPPING:
//   This module replaces the PWM Generator block in your Simulink model.
//   CMPA register = duty cycle input to the PWM generator.
//   Shadow register ensures CMPA updates only at counter zero (no glitches).
//
// WAVEFORM (Up-Down Mode, CMPA = 500, TBPRD = 1000):
//
//   Counter:  0___500___1000___500___0
//   EPWM1A:   ______|‾‾‾‾‾‾‾‾|______   HIGH when counter > CMPA going up
//   EPWM1B:   ‾‾‾‾‾‾|________|‾‾‾‾‾   Complementary with dead band
//
//#############################################################################

#include "epwm.h"

//
// EPWM_configure - Full ePWM1 configuration for synchronous buck
//
void EPWM_configure(void)
{
    //
    // --- GPIO Configuration ---
    // GPIO0 = EPWM1A = High Side Gate Drive Signal
    // GPIO1 = EPWM1B = Low Side Gate Drive Signal
    //
    GPIO_setPadConfig(PWM_GPIO_HS, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_0_EPWM1A);

    GPIO_setPadConfig(PWM_GPIO_LS, GPIO_PIN_TYPE_STD);
    GPIO_setPinConfig(GPIO_1_EPWM1B);

    //
    // Freeze TBCLK during configuration.
    // Prevents PWM from starting in an undefined state while we set registers.
    // All ePWM modules share this sync clock — freeze affects all of them.
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    // =========================================================================
    // TIME BASE (TB) SUBMODULE
    // Controls the PWM switching frequency via TBPRD register.
    //
    // Up-Down Count Mode:
    //   Counter counts 0 → TBPRD → 0 → TBPRD → 0 (triangle wave)
    //   One full period = 2 * TBPRD clock cycles
    //   Fsw = SYSCLK / (2 * TBPRD) = 200MHz / (2 * 1000) = 100kHz
    //
    // Advantage over Up-Count: Symmetric PWM, better for motor/converter control
    // =========================================================================

    EPWM_setTimeBasePeriod(EPWM1_BASE, PWM_TBPRD);
    // TBPRD = 1000 → switching frequency = 100kHz

    EPWM_setPhaseShift(EPWM1_BASE, 0U);
    // No phase shift needed for single converter (used in interleaved designs)

    EPWM_setTimeBaseCounter(EPWM1_BASE, 0U);
    // Start counter from zero

    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP_DOWN);
    // Up-Down (triangle) counting mode for symmetric PWM

    EPWM_disablePhaseShiftLoad(EPWM1_BASE);
    // Disable phase shift loading (not needed for single module)

    EPWM_setClockPrescaler(EPWM1_BASE,
                           EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);
    // No clock division — TBCLK = SYSCLK = 200MHz
    // Higher TBCLK = finer duty cycle resolution

    // =========================================================================
    // COUNTER COMPARE (CC) SUBMODULE
    // CMPA register controls the duty cycle.
    //
    // Duty Cycle = CMPA / TBPRD
    // Example: CMPA=500, TBPRD=1000 → Duty = 50%
    //
    // Shadow Register Mode:
    //   CMPA is double-buffered (shadow + active registers).
    //   Writing to CMPA writes to the shadow register first.
    //   Shadow transfers to active register at counter ZERO.
    //   This prevents mid-cycle glitches when updating duty cycle.
    //
    // SIMULINK MAPPING:
    //   In Simulink, the duty cycle output from Current PI feeds directly
    //   into the PWM generator. Here, that duty cycle becomes CMPA.
    // =========================================================================

    EPWM_setCounterCompareValue(EPWM1_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                PWM_DUTY_INIT);
    // Start at 50% duty cycle (open loop startup)

    EPWM_setCounterCompareShadowLoadMode(EPWM1_BASE,
                                         EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    // Shadow register loads to active at counter = 0
    // Ensures glitch-free duty cycle updates from ISR

    // =========================================================================
    // ACTION QUALIFIER (AQ) SUBMODULE
    // Defines the exact logic of the PWM output pin.
    // "What does EPWM1A do when the counter hits CMPA going up/down?"
    //
    // For synchronous buck converter (Up-Down mode):
    //   EPWM1A HIGH when counter = CMPA counting UP   (rising edge)
    //   EPWM1A LOW  when counter = CMPA counting DOWN (falling edge)
    //
    // Result: Symmetric PWM pulse centered around TBPRD
    // This is equivalent to the natural sampling PWM in your Simulink model.
    // =========================================================================

    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);
    // EPWM1A goes HIGH when counter hits CMPA counting up

    EPWM_setActionQualifierAction(EPWM1_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);
    // EPWM1A goes LOW when counter hits CMPA counting down

    // =========================================================================
    // DEAD BAND (DB) SUBMODULE
    // Inserts delay between HS turn-off and LS turn-on (and vice versa).
    // Prevents shoot-through: both FETs ON simultaneously = short circuit.
    //
    // Dead time = 100ns = 20 counts at 200MHz
    //
    // DB Module takes EPWM1A as input and generates:
    //   EPWM1A = High Side signal with rising edge delay
    //   EPWM1B = Inverted + falling edge delay (complementary LS signal)
    // =========================================================================

    EPWM_setDeadBandDelayMode(EPWM1_BASE, EPWM_DB_RED, true);
    // Enable Rising Edge Delay on EPWM1A (HS turn-on delay)

    EPWM_setDeadBandDelayMode(EPWM1_BASE, EPWM_DB_FED, true);
    // Enable Falling Edge Delay on EPWM1B (LS turn-on delay)

    EPWM_setRisingEdgeDelayCount(EPWM1_BASE, PWM_DEADBAND_COUNTS);
    // 20 counts = 100ns dead time before HS turns ON

    EPWM_setFallingEdgeDelayCount(EPWM1_BASE, PWM_DEADBAND_COUNTS);
    // 20 counts = 100ns dead time before LS turns ON

    EPWM_setDeadBandOutputSwapMode(EPWM1_BASE, EPWM_DB_OUTPUT_B, true);
    // Invert EPWM1B to make it complementary to EPWM1A

    EPWM_setDeadBandDelayPolarity(EPWM1_BASE,
                                   EPWM_DB_RED,
                                   EPWM_DB_POLARITY_ACTIVE_HIGH);
    EPWM_setDeadBandDelayPolarity(EPWM1_BASE,
                                   EPWM_DB_FED,
                                   EPWM_DB_POLARITY_ACTIVE_LOW);

    // =========================================================================
    // EVENT TRIGGER (ET) SUBMODULE
    // Generates ADC Start-of-Conversion (SOCA) trigger signal.
    //
    // Trigger at counter ZERO (bottom of triangle wave).
    // This ensures ADC samples exactly at the midpoint of the PWM period,
    // which is the optimal sampling instant for minimum switching ripple.
    //
    // SIMULINK MAPPING:
    //   This replaces the "Sample Time" or "ADC Trigger" block in Simulink.
    //   In Simulink, the ADC block triggers at fixed sample time = 10us.
    //   Here, the ePWM hardware generates this trigger automatically.
    // =========================================================================

    EPWM_setInterruptSource(EPWM1_BASE, EPWM_INT_TBCTR_ZERO);
    // Trigger event at counter = 0 (once per PWM cycle = 100kHz)

    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    // Enable SOCA (Start of Conversion A) output

    EPWM_setADCTriggerSource(EPWM1_BASE,
                             EPWM_SOC_A,
                             EPWM_SOC_TBCTR_ZERO);
    // SOCA fires when counter = 0

    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1U);
    // Fire SOCA every 1 counter-zero event (every PWM cycle = 100kHz)

    //
    // Re-enable TBCLK sync — all configured ePWM modules start together
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

//
// EPWM_setDutyCycle - Update duty cycle from control loop (float input)
//
// Input:  duty = normalized duty cycle (0.0 to 1.0)
// Action: Converts to CMPA counts, clamps to min/max, updates shadow register
//
// SIMULINK MAPPING:
//   This function is called every ISR with the Current PI controller output.
//   It maps to the "Duty Cycle to PWM" conversion block in Simulink.
//
void EPWM_setDutyCycle(float32_t duty)
{
    uint16_t cmpa;

    //
    // Convert normalized duty (0.0-1.0) to CMPA counts
    // CMPA = duty * TBPRD
    //
    cmpa = (uint16_t)(duty * (float32_t)PWM_TBPRD);

    //
    // Clamp to safe operating limits (5% - 95%)
    // Prevents transformer saturation and gate drive issues at extremes
    //
    if(cmpa < PWM_DUTY_MIN) cmpa = PWM_DUTY_MIN;
    if(cmpa > PWM_DUTY_MAX) cmpa = PWM_DUTY_MAX;

    EPWM_updateCMPA(cmpa);
}

//
// EPWM_updateCMPA - Write CMPA to shadow register
//
// Shadow register mechanism ensures the update takes effect only at
// counter zero — never mid-cycle. This is critical for stable control.
//
void EPWM_updateCMPA(uint16_t cmpa)
{
    EPWM_setCounterCompareValue(EPWM1_BASE,
                                EPWM_COUNTER_COMPARE_A,
                                cmpa);
}

//
// End of File
//
