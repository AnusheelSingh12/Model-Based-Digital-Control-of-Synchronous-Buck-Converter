//#############################################################################
//
// FILE:    interrupts.h
//
// TITLE:   Interrupt Service Routines Header - Synchronous Buck Converter
//
//#############################################################################

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "driverlib.h"
#include "device.h"

//
// Function Prototypes
//
__interrupt void ADC_ISR(void);

#endif // INTERRUPTS_H

//
// End of File
//
