//#############################################################################
//
// FILE:    adc.c
//
// TITLE:   ADC Module - Synchronous Buck Converter
//
// DESCRIPTION:
//   Configures ADCA (voltage) and ADCB (current) for ePWM-triggered
//   sampling at 100kHz. Both ADCs are triggered by EPWM1 SOCA at
//   counter zero — the optimal sampling point for minimum ripple error.
//
// ADC TRIGGER CHAIN:
//
//   EPWM1 Counter = 0
//        ↓
//   SOCA signal generated
//        ↓
//   ADCA SOC0 triggered → samples ADCINA0 (Vout)
//   ADCB SOC0 triggered → samples ADCINB0 (Iout)
//        ↓
//   ADCA EOC (End of Conversion) → fires INT_ADCA1
//        ↓
//   ADC_ISR executes → reads results → runs control loop
//
// SIMULINK MAPPING:
//   Replaces the ADC block pair in Simulink.
//   In Simulink, ADC reads at fixed Ts = 10us (100kHz sample rate).
//   Here, ePWM trigger ensures hardware-synchronized sampling.
//
//#############################################################################

#include "adc.h"

//
// Global ADC measurement variables
// volatile: prevents compiler from optimizing these away
// Written in ISR, read by controller — must be global
//
volatile float32_t g_vOut       = 0.0f;
volatile float32_t g_iOut       = 0.0f;
volatile uint16_t  g_adcVoutRaw = 0U;
volatile uint16_t  g_adcIoutRaw = 0U;

//
// ADC_configure - Configure ADCA and ADCB for synchronous buck control
//
void ADC_configure(void)
{
    // =========================================================================
    // ADCA CONFIGURATION (Voltage Feedback)
    // =========================================================================

    //
    // Set ADCA resolution and signal mode
    // 12-bit resolution: 0-4095 counts across 0-3.3V
    // Single-ended: measures ADCINA0 with respect to VREFLO (GND)
    //
    ADC_setMode(ADC_VOUT_BASE,
                ADC_RESOLUTION_12BIT,
                ADC_MODE_SINGLE_ENDED);

    //
    // Set ADC clock prescaler
    // ADCCLK = SYSCLK / 4 = 200MHz / 4 = 50MHz
    // F28379D max ADCCLK = 50MHz
    //
    ADC_setPrescaler(ADC_VOUT_BASE, ADC_CLK_DIV_4_0);

    //
    // Set interrupt pulse position to end of conversion
    // INT pulse fires AFTER result is written to result register
    // Ensures ISR reads valid data
    //
    ADC_setInterruptPulseMode(ADC_VOUT_BASE, ADC_PULSE_END_OF_CONV);

    //
    // Power up ADCA and wait for it to stabilize
    // F28379D requires 1ms after power-up before first conversion
    //
    ADC_enableConverter(ADC_VOUT_BASE);
    DEVICE_DELAY_US(1000U);

    //
    // Configure SOC0 (Start of Conversion 0) for voltage measurement
    //
    // SOC = one conversion request. Each SOC has:
    //   - Input channel (which pin to sample)
    //   - Trigger source (what starts the conversion)
    //   - Sample window (how long to sample before converting)
    //
    ADC_setupSOC(ADC_VOUT_BASE,
                 ADC_VOUT_SOC,              // SOC number = SOC0
                 ADC_TRIGGER_EPWM1_SOCA,   // Triggered by EPWM1 SOCA
                 ADC_VOUT_CHANNEL,          // Sample ADCINA0
                 15U);                      // 15 ADCCLK cycles sample window
    // Sample window = 15 cycles / 50MHz = 300ns
    // Longer sample window → more accurate on high-impedance sources

    //
    // Configure ADCA Interrupt 1 (ADCINT1)
    // Fires when SOC0 conversion completes
    // This triggers ADC_ISR where we run the control loop
    //
    ADC_setInterruptSource(ADC_VOUT_BASE,
                           ADC_INT_NUMBER1,     // ADCINT1
                           ADC_VOUT_SOC);       // Fires after SOC0 completes

    ADC_enableInterrupt(ADC_VOUT_BASE, ADC_INT_NUMBER1);
    // Enable ADCINT1

    ADC_clearInterruptStatus(ADC_VOUT_BASE, ADC_INT_NUMBER1);
    // Clear any pending interrupt flag before starting

    // =========================================================================
    // ADCB CONFIGURATION (Current Feedback)
    // =========================================================================

    //
    // Mirror ADCA configuration for ADCB
    // Same resolution, clock, trigger source
    //
    ADC_setMode(ADC_IOUT_BASE,
                ADC_RESOLUTION_12BIT,
                ADC_MODE_SINGLE_ENDED);

    ADC_setPrescaler(ADC_IOUT_BASE, ADC_CLK_DIV_4_0);

    ADC_setInterruptPulseMode(ADC_IOUT_BASE, ADC_PULSE_END_OF_CONV);

    ADC_enableConverter(ADC_IOUT_BASE);
    DEVICE_DELAY_US(1000U);

    //
    // Configure ADCB SOC0 for current measurement
    // Same EPWM1 SOCA trigger as voltage — both sample simultaneously
    //
    ADC_setupSOC(ADC_IOUT_BASE,
                 ADC_IOUT_SOC,
                 ADC_TRIGGER_EPWM1_SOCA,   // Same trigger as voltage ADC
                 ADC_IOUT_CHANNEL,          // Sample ADCINB0
                 15U);
    // Both ADCs triggered at same instant → simultaneous V and I sampling
    // Critical for accurate power calculation and control

    //
    // No interrupt needed on ADCB — ADCA interrupt drives the ISR
    // ADCB result will be ready by the time ISR reads it (conversion is fast)
    //
}

//
// End of File
//
