//#############################################################################
//
// FILE:    main.c
//
// TITLE:   Synchronous Buck Converter - Cascaded Digital Controller
//          TI LaunchXL-F28379D | CPU1 | Driverlib | C2000Ware
//
// DESCRIPTION:
//   This firmware implements a cascaded PI control architecture for a
//   synchronous buck converter with the following specs:
//
//     Vin  = 24V     Vout = 12V     Fsw  = 100kHz
//     L    = 50uH    C    = 12.5uF  Rload = 12 Ohm
//
// CASCADED CONTROL ARCHITECTURE:
//
//   Vref -->[+]-->[ Voltage PI ]-->[ Iref ]-->[+]-->[ Current PI ]-->[ Duty ]
//            [-]                               [-]
//            |                                 |
//           Vout (ADC)                        Iout (ADC)
//
//   Outer Loop (Voltage): Slower. Compares Vout to Vref, outputs Iref.
//   Inner Loop (Current): Faster. Compares Iout to Iref, outputs duty cycle.
//   Both loops execute inside the ADC ISR at 100kHz.
//
// SIMULINK MAPPING:
//   Simulink Voltage PI Block   → voltageController in controller.c
//   Simulink Current PI Block   → currentController in controller.c
//   Simulink PWM Generator      → epwm.c (EPWM1, up-down mode, CMPA)
//   Simulink ADC Block          → adc.c (ADCA/ADCB, ePWM triggered)
//   Simulink Scope/Output       → Watch Window (g_vOut, g_iOut, g_duty)
//
//#############################################################################

#include "driverlib.h"
#include "device.h"
#include "epwm.h"
#include "adc.h"
#include "controller.h"
#include "interrupts.h"

//
// Main
//
void main(void)
{
    //
    // Step 1: Device Initialization
    // Configures system clocks (200MHz SYSCLK), watchdog, and peripheral clocks.
    // Must always be the first call.
    //
    Device_init();

    //
    // Step 2: GPIO Initialization
    // Unlocks all GPIO pins and sets default states.
    // Required before any GPIO_setPinConfig() calls.
    //
    Device_initGPIO();

    //
    // Step 3: Interrupt Module Initialization
    // Resets the PIE (Peripheral Interrupt Expansion) controller.
    // Clears all CPU interrupt flags.
    // Disables all interrupts during configuration.
    //
    Interrupt_initModule();

    //
    // Step 4: PIE Vector Table Initialization
    // Loads default shell ISR addresses into all 96 PIE interrupt vectors.
    // Our actual ISR (ADC_ISR) will overwrite the relevant vector below.
    //
    Interrupt_initVectorTable();

    //
    // Step 5: Register ADC ISR into PIE Vector Table
    // INT_ADCA1 = PIE Group 1, Vector 1 = ADCA End-of-Conversion interrupt
    // This tells the CPU where to jump when ADCA conversion completes.
    //
    Interrupt_register(INT_ADCA1, &ADC_ISR);

    //
    // Step 6: ePWM Configuration
    // Configures EPWM1 for 100kHz switching, up-down count mode,
    // and ADC trigger generation at counter zero.
    // GPIO0 = EPWM1A (High Side), GPIO1 = EPWM1B (Low Side, complementary)
    //
    EPWM_configure();

    //
    // Step 7: ADC Configuration
    // Configures ADCA for voltage feedback (ADCINA0)
    // Configures ADCB for current feedback (ADCINB0)
    // Both triggered by EPWM1 SOCA at counter zero (100kHz sampling rate)
    //
    ADC_configure();

    //
    // Step 8: Controller Initialization
    // Zeros all PI state variables (integrators, previous errors)
    // Sets Vref = 12V target output voltage
    //
    Controller_init();

    //
    // Step 9: Enable ADC Interrupt
    // Enables ADCA EOC (End of Conversion) interrupt in PIE Group 1
    //
    Interrupt_enable(INT_ADCA1);

    //
    // Step 10: Enable Global Interrupts
    // EINT = Enable INTerrupts (clears INTM bit in ST1 register)
    // ERTM = Enable Real-Time Mode (allows Watch Window updates while running)
    //
    EINT;
    ERTM;

    //
    // Infinite background loop.
    // All control work happens in ADC_ISR at 100kHz.
    // CPU is otherwise idle.
    //
    for(;;)
    {
        NOP;
    }
}

//
// End of File
//
