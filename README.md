# Model-Based Design and HIL Deployment of a Cascade Digital Controller for a Synchronous Buck Converter on TI C2000 DSP

## Summer Research Internship

**Institution:** Indian Institute of Technology Guwahati

**Duration:** May 2026 – July 2026

---

# Overview

This repository documents my Summer Research Internship carried out at the Department of Electronics and Electrical Engineering, Indian Institute of Technology Guwahati.

The internship focused on the complete workflow involved in designing, simulating, implementing, and validating a digitally controlled synchronous buck converter using the Texas Instruments C2000 real-time digital signal processor platform.

Unlike repositories that present only the final implementation, this repository documents the complete engineering process, beginning with mathematical modeling and simulation, progressing through controller design, embedded firmware development, peripheral configuration, laboratory validation, and finally hardware deployment.

The project combines concepts from:

* Power Electronics
* Control Systems
* Embedded Systems
* Digital Signal Processing
* MATLAB & Simulink
* Simscape Electrical
* Real-Time Embedded Programming
* Hardware-in-the-Loop Development

---

# Project Objectives

The primary objectives of the internship were:

* Develop a high-fidelity digital twin of a synchronous buck converter.
* Design a cascaded digital controller consisting of outer voltage and inner current control loops.
* Learn the Texas Instruments C2000 development ecosystem.
* Implement embedded firmware using Code Composer Studio.
* Configure and validate DSP peripherals including GPIO, Timers, PWM, Interrupts, and ADC.
* Validate controller performance using laboratory instruments.
* Document the complete engineering workflow.

---

# Engineering Workflow

```text
Problem Definition
        │
        ▼
MATLAB Fundamentals
        │
        ▼
Simulink Fundamentals
        │
        ▼
Simscape Electrical
        │
        ▼
Digital Twin Development
        │
        ▼
Cascade PI Controller Design
        │
        ▼
Closed Loop Simulation
        │
        ▼
TI C2000 DSP Learning
        │
        ▼
GPIO
Interrupts
CPU Timers
PWM
ADC
        │
        ▼
Hardware Validation
        │
        ▼
Final System Integration
```

---

# Repository Structure

```text
IITG-DSP-Buck-Converter-Control/

├── 01_Internship_Overview/
│
├── 02_MATLAB_and_Simulink/
│
├── 03_Digital_Twin/
│
├── 04_Cascade_Controller/
│
├── 05_TI_C2000_DSP/
│   ├── GPIO/
│   ├── Interrupts/
│   ├── CPU_Timers/
│   ├── ePWM/
│   ├── ADC/
│
├── 06_Hardware_Setup/
│   ├── LaunchXL_F28379D/
│   ├── Oscilloscope/
│   ├── Function_Generator/
│
├── 07_Firmware/
│
├── 08_Simulation_Results/
│
├── 09_Final_Report/
│
├── Images/
│
├── Videos/
│
└── README.md
```

---

# Project Phases

## Phase 1 — Digital Twin Development

The first stage of the project focused on constructing a high-fidelity Simscape Electrical model of the synchronous buck converter.

Topics include:

* Converter topology
* MOSFET modelling
* Passive component modelling
* PWM generation
* Open-loop validation
* Converter analysis
* Duty-cycle experiments

---

## Phase 2 — Cascade Digital Controller

The second stage involved designing a cascaded digital controller.

Topics include:

* Voltage control loop
* Current control loop
* PI controller design
* Closed-loop regulation
* Controller tuning
* Performance analysis

---

## Phase 3 — TI C2000 DSP Development

The third stage focused on learning and implementing embedded firmware using the Texas Instruments C2000 platform.

Topics include:

* Code Composer Studio
* Project configuration
* GPIO
* Interrupts
* CPU Timers
* Enhanced PWM
* ADC
* Debugging
* Flash programming

---

## Phase 4 — Hardware Validation

The final stage focuses on validating both the firmware and control algorithms using laboratory equipment.

Topics include:

* Oscilloscope measurements
* Function generator experiments
* PWM verification
* ADC verification
* Peripheral testing
* System integration

---

# Software and Development Tools

MATLAB

Simulink

Simscape Electrical

Code Composer Studio

C2000Ware

Texas Instruments C2000 Driver Library

---

# Hardware Platform

Texas Instruments LaunchXL-F28379D

Texas Instruments C2000 DSP

Rigol Digital Oscilloscope

Function Generator

Host Computer

---

# Skills Developed

Throughout this internship I gained practical experience in:

* Model-Based Design
* Power Electronics
* Control Systems
* Digital Signal Processing
* MATLAB Programming
* Simulink Modelling
* Simscape Electrical
* Embedded C Programming
* TI C2000 Development
* Code Composer Studio
* GPIO Configuration
* Interrupt Programming
* CPU Timers
* PWM Generation
* ADC Configuration
* Oscilloscope Measurements
* Function Generator Operation
* Hardware Debugging
* Engineering Documentation

---

# Repository Navigation

| Section             | Description                                         |
| ------------------- | --------------------------------------------------- |
| Internship Overview | Internship objectives, timeline, and research scope |
| MATLAB and Simulink | Learning material and simulation fundamentals       |
| Digital Twin        | Simscape converter modelling                        |
| Cascade Controller  | Controller design and closed-loop simulation        |
| TI C2000 DSP        | Embedded firmware development                       |
| Hardware Setup      | Laboratory setup and instrumentation                |
| Firmware            | Embedded source code                                |
| Simulation Results  | Waveforms and experimental validation               |
| Final Report        | Internship report and documentation                 |

---

# Current Status

The repository is actively maintained and will continue to be updated throughout the internship.

Future updates include:

* Complete firmware
* Hardware validation
* Oscilloscope measurements
* ADC experiments
* Closed-loop implementation
* Final internship report
* Demonstration videos

---

# Acknowledgements

I would like to express my sincere gratitude to my internship supervisor, research mentors, and the School of Electronics and Electrical Engineering at the Indian Institute of Technology Guwahati for providing the opportunity to work on this project and gain practical experience in model-based design, embedded control systems, and real-time digital signal processing.

---

# Author

Anusheel Singh

Electronics and Communication Engineering

Areas of Interest

* Embedded Systems
* Digital Control Systems
* Power Electronics
* DSP
* Hardware Integration
* Robotics
* Automotive Electronics

---

This repository is intended to serve as a complete technical record of my internship, documenting both the engineering process and the final implementation.
