//#############################################################################
//
// FILE:    epwm.h
//
// TITLE:   ePWM Module Header - Synchronous Buck Converter
//
//#############################################################################

#ifndef EPWM_H
#define EPWM_H

#include "driverlib.h"
#include "device.h"

//
// PWM Parameters
// Fsw = 100kHz, SYSCLK = 200MHz, Up-Down mode
// TBPRD = SYSCLK / (2 * Fsw) = 200e6 / (2 * 100e3) = 1000
//
#define PWM_TBPRD           1000U       // Timer period → 100kHz switching

//
// Duty Cycle Limits (Anti-Saturation)
// Min = 5%  → CMPA = 0.05 * 1000 =  50
// Max = 95% → CMPA = 0.95 * 1000 = 950
//
#define PWM_DUTY_MIN        50U         // 5% minimum duty cycle
#define PWM_DUTY_MAX        950U        // 95% maximum duty cycle
#define PWM_DUTY_INIT       500U        // 50% initial duty cycle (open loop start)

//
// Dead Band (Shoot-Through Protection)
// Prevents both high-side and low-side FETs conducting simultaneously.
// 100ns dead time at 200MHz = 20 clock cycles
//
#define PWM_DEADBAND_NS     100U        // Dead time in nanoseconds
#define PWM_DEADBAND_COUNTS 20U         // 100ns * 200MHz = 20 counts

//
// GPIO Pin Assignments
//
#define PWM_GPIO_HS         0U          // GPIO0 = EPWM1A = High Side FET
#define PWM_GPIO_LS         1U          // GPIO1 = EPWM1B = Low Side FET

//
// Function Prototypes
//
void EPWM_configure(void);
void EPWM_setDutyCycle(float32_t duty);
void EPWM_updateCMPA(uint16_t cmpa);

#endif // EPWM_H

//
// End of File
//
