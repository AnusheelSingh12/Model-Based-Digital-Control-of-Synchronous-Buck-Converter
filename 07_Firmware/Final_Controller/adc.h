//#############################################################################
//
// FILE:    adc.h
//
// TITLE:   ADC Module Header - Synchronous Buck Converter
//
//#############################################################################

#ifndef ADC_H
#define ADC_H

#include "driverlib.h"
#include "device.h"

//
// ADC Channel Assignments
// ADCA0 = Voltage feedback (Vout measurement via resistor divider)
// ADCB0 = Current feedback (Iout measurement via current sensor / shunt)
//
#define ADC_VOUT_BASE       ADCA_BASE           // Voltage ADC module
#define ADC_IOUT_BASE       ADCB_BASE           // Current ADC module
#define ADC_VOUT_CHANNEL    ADC_CH_ADCIN0       // ADCINA0 pin
#define ADC_IOUT_CHANNEL    ADC_CH_ADCIN0       // ADCINB0 pin
#define ADC_VOUT_SOC        ADC_SOC_NUMBER0     // SOC0 for voltage
#define ADC_IOUT_SOC        ADC_SOC_NUMBER0     // SOC0 for current
#define ADC_VOUT_RESULT     ADC_SOC_NUMBER0     // Result register 0
#define ADC_IOUT_RESULT     ADC_SOC_NUMBER0     // Result register 0

//
// Physical Scaling Constants
// ADC is 12-bit: counts range from 0 to 4095
// Reference voltage = 3.3V
//
// Voltage Divider for Vout (24V max → 3.3V ADC max):
//   R1 = 33k, R2 = 4.7k → Scale = (R1+R2)/R2 = 37.7/4.7 ≈ 8.02
//   Vout = ADC_count * (3.3/4095) * 8.02
//   Vout = ADC_count * 0.006458
//
// Current Sensor (e.g., ACS712 5A = 185mV/A, offset = 2.5V):
//   Iout = (ADC_count * (3.3/4095) - 2.5) / 0.185
//   Simplified: Iout = ADC_count * CURRENT_SCALE - CURRENT_OFFSET
//
#define ADC_VREF            3.3f                // ADC reference voltage
#define ADC_RESOLUTION      4095.0f             // 12-bit ADC

#define VOLTAGE_DIVIDER_RATIO   8.02f           // Hardware voltage divider
#define VOLTAGE_SCALE       (ADC_VREF / ADC_RESOLUTION * VOLTAGE_DIVIDER_RATIO)
// VOLTAGE_SCALE = 3.3/4095 * 8.02 = 0.006458 V/count

#define CURRENT_SENSOR_SENS 0.185f              // ACS712: 185mV per Amp
#define CURRENT_OFFSET      2.5f               // ACS712: 2.5V at zero current
#define CURRENT_SCALE       (ADC_VREF / ADC_RESOLUTION)
// Apply: Iout = (count * CURRENT_SCALE - CURRENT_OFFSET) / CURRENT_SENSOR_SENS

//
// Global ADC Results (written by ISR, read by controller)
//
extern volatile float32_t g_vOut;              // Measured output voltage (V)
extern volatile float32_t g_iOut;              // Measured output current (A)
extern volatile uint16_t  g_adcVoutRaw;        // Raw ADC count (debug)
extern volatile uint16_t  g_adcIoutRaw;        // Raw ADC count (debug)

//
// Function Prototypes
//
void ADC_configure(void);

#endif // ADC_H

//
// End of File
//
