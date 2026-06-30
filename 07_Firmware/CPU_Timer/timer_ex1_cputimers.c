//#############################################################################
//
// FILE:   timer_ex1_cputimers.c
//
// TITLE:  CPU Timers Example
//
//! \addtogroup driver_example_list
//! <h1> CPU Timers </h1>
//!
//! This example configures CPU Timer0, 1, and 2 and increments
//! a counter each time the timer asserts an interrupt.
//!
//! \b External \b Connections \n
//!  - None
//!
//! \b Watch \b Variables \n
//! - cpuTimer0IntCount
//! - cpuTimer1IntCount
//! - cpuTimer2IntCount
//!
//
//#############################################################################
//
// 
// $Copyright:
// Copyright (C) 2013-2024 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Globals
//
volatile uint16_t cpuTimer0IntCount = 0;

//
// Function Prototypes
//
__interrupt void cpuTimer0ISR(void);

//
// Main
//
void main(void)
{
    //
    // Initialize device
    //
    Device_init();

    //
    // Configure GPIO31 (LD10 Red LED)
    //
    Device_initGPIO();

    GPIO_setPinConfig(GPIO_31_GPIO31);
    GPIO_setPadConfig(31, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(31, GPIO_DIR_MODE_OUT);

    //
    // Start LED OFF
    //
    GPIO_writePin(31, 1);

    //
    // Initialize Interrupt System
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //
    // Register Timer0 ISR
    //
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    //
    // Configure CPU Timer0
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 200000000);

    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);

    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);

    CPUTimer_enableInterrupt(CPUTIMER0_BASE);

    //
    // Enable Timer0 Interrupt
    //
    Interrupt_enable(INT_TIMER0);

    //
    // Start Timer0
    //
    CPUTimer_startTimer(CPUTIMER0_BASE);

    //
    // Enable Global Interrupts
    //
    EINT;
    ERTM;

    //
    // Main Loop
    //
    while(1)
    {

    }
}

//
// Timer0 ISR
//
__interrupt void cpuTimer0ISR(void)
{
    cpuTimer0IntCount++;

    GPIO_togglePin(31);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}