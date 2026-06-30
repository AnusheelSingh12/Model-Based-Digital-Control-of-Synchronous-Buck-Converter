//#############################################################################
//
// FILE:    controller.h
//
// TITLE:   Cascaded PI Controller Header - Synchronous Buck Converter
//
// CONTROL ARCHITECTURE:
//
//          Vref
//           |
//          [+]
//           |  error_v
//    Vout--[-]
//           |
//    [ Voltage PI ] ──── Iref (current reference)
//           |
//          [+]
//           |  error_i
//    Iout--[-]
//           |
//    [ Current PI ] ──── duty (0.0 to 1.0)
//           |
//      [ EPWM1 CMPA ]
//
// SIMULINK MAPPING:
//   Voltage PI block → voltagePI struct + PI_update() call
//   Current PI block → currentPI struct + PI_update() call
//   Anti-windup clamp → PI_update() integrator clamp
//   Saturation block  → duty clamp in EPWM_setDutyCycle()
//
//#############################################################################

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "driverlib.h"
#include "device.h"

//
// System Reference Values
//
#define VREF                12.0f       // Target output voltage (V)
#define IREF_MAX            2.0f        // Max current reference from voltage loop (A)
#define IREF_MIN            0.0f        // Min current reference (no negative current)

//
// Voltage PI Gains
// Tuned for: Vout loop bandwidth ~1kHz (much slower than 100kHz switching)
// Start conservative — increase Kp until oscillation, then back off 50%
// These values are initial estimates; tune on hardware with scope
//
#define VOLTAGE_KP          0.05f       // Proportional gain
#define VOLTAGE_KI          0.001f      // Integral gain
#define VOLTAGE_IMAX        2.0f        // Integrator anti-windup clamp (A)
#define VOLTAGE_IMIN        0.0f        // Integrator min clamp

//
// Current PI Gains
// Tuned for: Iout loop bandwidth ~10kHz (10x faster than voltage loop)
// Inner loop must be significantly faster than outer loop for stability
//
#define CURRENT_KP          0.1f        // Proportional gain
#define CURRENT_KI          0.01f       // Integral gain
#define CURRENT_IMAX        0.95f       // Integrator max clamp (duty cycle max)
#define CURRENT_IMIN        0.05f       // Integrator min clamp (duty cycle min)

//
// Sampling Period
// Ts = 1/Fsw = 1/100kHz = 10us
// Used in discrete integrator: integral += error * Ki * Ts
//
#define TS                  0.00001f    // 10 microseconds

//
// PI Controller Structure
// One instance per control loop (voltage and current)
//
typedef struct
{
    float32_t kp;           // Proportional gain
    float32_t ki;           // Integral gain
    float32_t integrator;   // Integrator state (accumulates over time)
    float32_t outMax;       // Output upper clamp (anti-windup)
    float32_t outMin;       // Output lower clamp (anti-windup)
    float32_t output;       // Controller output
} PI_Controller;

//
// Global Controller Instances
//
extern PI_Controller voltagePI;     // Outer voltage loop
extern PI_Controller currentPI;     // Inner current loop

//
// Global Control Signals (visible in Watch Window)
//
extern volatile float32_t g_vref;       // Voltage reference (V)
extern volatile float32_t g_iref;       // Current reference from voltage loop (A)
extern volatile float32_t g_duty;       // Duty cycle output (0.0 to 1.0)
extern volatile float32_t g_error_v;    // Voltage error (debug)
extern volatile float32_t g_error_i;    // Current error (debug)

//
// Function Prototypes
//
void Controller_init(void);
float32_t PI_update(PI_Controller *pi, float32_t reference, float32_t feedback);

#endif // CONTROLLER_H

//
// End of File
//
