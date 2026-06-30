# Firmware Development

---

# Overview

This directory contains the embedded firmware developed and studied during the internship on the Texas Instruments TMS320F28379D (C2000) Digital Signal Processor.

The firmware development progressed from fundamental peripheral examples to the implementation of the control algorithms required for the synchronous buck converter.

Each folder represents a specific stage in the learning process and includes source code, example projects, and documentation.

---

# Development Platform

Hardware

- TI LaunchXL-F28379D
- XDS110 Debug Probe

Software

- Code Composer Studio (CCS)
- C2000Ware
- TI C2000 Driver Library

Programming Language

- C

---

# Repository Structure

```text
Firmware/

GPIO/
Interrupts/
CPU_Timers/
ePWM/
ADC/
Final_Controller/
Utilities/
```

---

# Learning Progression

The firmware development followed the sequence below:

GPIO

↓

Interrupts

↓

CPU Timers

↓

Enhanced PWM (ePWM)

↓

Analog-to-Digital Converter (ADC)

↓

Peripheral Integration

↓

Digital Controller Implementation

---

# Topics Covered

- GPIO Programming
- Interrupt Service Routines
- CPU Timers
- Enhanced PWM
- ADC Configuration
- Peripheral Initialization
- Debugging
- CCS Project Structure
- Driver Library Usage

---

# Development Workflow

```text
Theory

↓

TI Example Projects

↓

Code Understanding

↓

Hardware Verification

↓

Oscilloscope Validation

↓

Custom Firmware

↓

Final Controller
```

---

# Skills Developed

- Embedded C Programming
- Peripheral Configuration
- Real-Time Embedded Systems
- Hardware Debugging
- Register-Level Programming
- TI Driver Library
- Code Composer Studio
- Digital Power Electronics

---

# Next Stage

The firmware developed here is integrated with the digital controller for hardware deployment of the synchronous buck converter.