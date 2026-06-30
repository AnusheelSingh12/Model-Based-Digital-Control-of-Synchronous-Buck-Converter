# Digital Twin Development

---

# Overview

The first major technical milestone of the internship was the development of a high-fidelity digital twin of a synchronous buck converter using MATLAB Simulink and Simscape Electrical.

Before implementing any control algorithm or deploying firmware to the TI C2000 DSP, it was essential to accurately model the converter and validate its behaviour through simulation.

The digital twin served as the virtual prototype for the remainder of the project, allowing controller development and system verification before hardware implementation.

---

# Objectives

The objectives of this phase were to:

- Develop an accurate synchronous buck converter model.
- Study the operation of switching power converters.
- Validate the theoretical buck converter equations.
- Observe converter behaviour under different operating conditions.
- Build a simulation platform for controller development.

---

# Converter Specifications

| Parameter | Value |
|-----------|------:|
| Input Voltage | 24 V |
| Output Voltage | 12 V |
| Switching Frequency | 100 kHz |
| Inductor | 50 µH |
| Capacitor | 12.5 µF |
| Load Resistance | 12 Ω |

---

# Power Stage Components

The digital twin consists of:

- DC Voltage Source
- High-Side MOSFET
- Low-Side MOSFET
- Gate Driver Logic
- PWM Generator
- Inductor
- Output Capacitor
- Resistive Load
- Voltage Measurement
- Current Measurement
- Electrical Reference

To improve simulation realism, component non-idealities were also incorporated:

- MOSFET RDS(on)
- Inductor DCR
- Capacitor ESR

---

# Development Workflow

```text
Buck Converter Theory
        │
        ▼
Component Selection
        │
        ▼
Simscape Electrical Model
        │
        ▼
PWM Generation
        │
        ▼
Open-Loop Simulation
        │
        ▼
Duty Cycle Validation
        │
        ▼
Digital Twin Verification
```

---

# Working Principle

The converter operates by rapidly switching the high-side and low-side MOSFETs using complementary PWM signals.

The switching action transfers energy through the inductor while the output capacitor filters voltage ripple, producing a regulated DC output.

The converter behaviour was first validated in open-loop operation before introducing feedback control.

---

# Validation

The digital twin was validated using the theoretical buck converter equation:

\[
V_{out}=D \times V_{in}
\]

Example:

- Input Voltage = 24 V
- Duty Cycle = 50%

Expected Output:

```
12 V
```

Simulation results closely matched theoretical calculations, confirming the correctness of the model.

---

# Simulation Results

The following characteristics were analysed during simulation:

- Output Voltage
- Inductor Current
- PWM Waveforms
- Switching Behaviour
- Converter Start-up Response
- Duty Cycle Variation

These results formed the basis for the subsequent controller design.

---

# Skills Developed

During this phase I gained practical experience in:

- Simscape Electrical
- Power Electronics
- Buck Converter Design
- Digital Twin Development
- PWM Modelling
- Power Stage Analysis
- Converter Validation
- Model Verification

---

# Folder Organization

```text
Models/
    Converter Models

Images/
    Simulation Screenshots

Parameters/
    Component Values

Results/
    Simulation Outputs
```

---

# Key Learning Outcomes

The digital twin provided a reliable virtual representation of the physical converter, enabling safe experimentation, controller development, and system verification before hardware deployment.

This significantly reduced implementation risk and established the simulation environment used throughout the remainder of the internship.

---

# Next Section

Continue to:

**04_Cascade_Controller**

where the validated digital twin is extended with a cascaded voltage-current control architecture for closed-loop operation.