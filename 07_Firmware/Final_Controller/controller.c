//#############################################################################
//
// FILE:    controller.c
//
// TITLE:   Cascaded PI Controller - Synchronous Buck Converter
//
// DESCRIPTION:
//   Implements discrete-time PI controllers for cascaded voltage/current
//   control of a synchronous buck converter.
//
// DISCRETE PI EQUATION:
//
//   error(k)      = reference(k) - feedback(k)
//   integrator(k) = integrator(k-1) + Ki * Ts * error(k)   ← Euler forward
//   output(k)     = Kp * error(k) + integrator(k)
//
//   Anti-windup: integrator is clamped to [outMin, outMax]
//   This prevents integrator from accumulating during saturation.
//
// SIMULINK MAPPING:
//
//   Simulink Discrete Integrator (Ki/z) → integrator += Ki * Ts * error
//   Simulink Gain (Kp)                  → Kp * error
//   Simulink Saturation block           → clamp(output, outMin, outMax)
//   Simulink Anti-windup clamp          → clamp(integrator, outMin, outMax)
//
// EXECUTION CONTEXT:
//   Called from ADC_ISR at 100kHz.
//   Must complete within one PWM period (10us).
//   Floating point operations are fast on F28379D (FPU32 hardware).
//
//#############################################################################

#include "controller.h"

//
// PI Controller Instances
//
PI_Controller voltagePI;
PI_Controller currentPI;

//
// Global Control Signals (Watch Window observable)
//
volatile float32_t g_vref    = VREF;    // 12V target
volatile float32_t g_iref    = 0.0f;    // Current reference from voltage loop
volatile float32_t g_duty    = 0.5f;    // Duty cycle (0.0 to 1.0)
volatile float32_t g_error_v = 0.0f;    // Voltage error
volatile float32_t g_error_i = 0.0f;    // Current error

//
// Controller_init - Initialize both PI controllers
//
// Sets gains, clears integrators, sets output limits.
// Must be called once before enabling interrupts.
//
void Controller_init(void)
{
    //
    // Voltage PI (Outer Loop)
    // Input:  Vref - Vout  (voltage error in Volts)
    // Output: Iref          (current reference in Amps)
    //
    // Bandwidth target: ~1kHz
    //   - Much slower than switching frequency (100kHz)
    //   - Slow enough to reject switching ripple
    //   - Fast enough to respond to load transients
    //
    voltagePI.kp          = VOLTAGE_KP;
    voltagePI.ki          = VOLTAGE_KI;
    voltagePI.integrator  = 0.0f;       // Clear integrator state
    voltagePI.outMax      = IREF_MAX;   // Max current reference = 2A
    voltagePI.outMin      = IREF_MIN;   // Min current reference = 0A
    voltagePI.output      = 0.0f;

    //
    // Current PI (Inner Loop)
    // Input:  Iref - Iout  (current error in Amps)
    // Output: Duty cycle   (normalized 0.0 to 1.0)
    //
    // Bandwidth target: ~10kHz
    //   - 10x faster than voltage loop (cascade stability rule)
    //   - Fast enough to track current reference from voltage loop
    //   - Output directly controls PWM duty cycle
    //
    currentPI.kp          = CURRENT_KP;
    currentPI.ki          = CURRENT_KI;
    currentPI.integrator  = 0.5f;       // Start at 50% duty (smooth startup)
    currentPI.outMax      = 0.95f;      // 95% max duty
    currentPI.outMin      = 0.05f;      // 5% min duty
    currentPI.output      = 0.5f;

    //
    // Initialize reference
    //
    g_vref = VREF;                      // 12V setpoint
}

//
// PI_update - Execute one step of the discrete PI controller
//
// Parameters:
//   pi        : pointer to PI_Controller instance
//   reference : desired setpoint (Vref or Iref)
//   feedback  : measured value (Vout or Iout)
//
// Returns:
//   Controller output (clamped to [outMin, outMax])
//
// EXECUTION TIME: ~10 FPU instructions, well within 10us ISR budget
//
float32_t PI_update(PI_Controller *pi, float32_t reference, float32_t feedback)
{
    float32_t error;
    float32_t output;

    //
    // Step 1: Compute Error
    // error = reference - feedback
    //
    // Voltage loop: error_v = Vref - Vout  (e.g., 12.0 - 11.8 = 0.2V)
    // Current loop: error_i = Iref - Iout  (e.g., 1.0 - 0.8 = 0.2A)
    //
    error = reference - feedback;

    //
    // Step 2: Update Integrator (Forward Euler)
    // integrator += Ki * Ts * error
    //
    // Ts = 10us (sampling period at 100kHz)
    // This accumulates the steady-state error over time → drives error to zero
    //
    pi->integrator += pi->ki * TS * error;

    //
    // Step 3: Anti-Windup — Clamp Integrator
    // Prevents integrator from growing unbounded during saturation.
    //
    // Without anti-windup: if output saturates (e.g., duty = 95%),
    // integrator keeps growing → large overshoot when leaving saturation.
    //
    // With anti-windup: integrator clamped → fast recovery from saturation.
    //
    // SIMULINK MAPPING: "Clamping" anti-windup on Discrete Integrator block
    //
    if(pi->integrator > pi->outMax)
        pi->integrator = pi->outMax;
    if(pi->integrator < pi->outMin)
        pi->integrator = pi->outMin;

    //
    // Step 4: Compute Output
    // output = Kp * error + integrator
    //
    // Proportional term: immediate response to error
    // Integral term:     eliminates steady-state error over time
    //
    output = (pi->kp * error) + pi->integrator;

    //
    // Step 5: Clamp Output
    // Ensures output stays within physical limits regardless of gains
    //
    if(output > pi->outMax)
        output = pi->outMax;
    if(output < pi->outMin)
        output = pi->outMin;

    //
    // Step 6: Store and return
    //
    pi->output = output;
    return output;
}

//
// End of File
//
