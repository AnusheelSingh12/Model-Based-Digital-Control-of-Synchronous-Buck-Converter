# MATLAB and Simulink

---

# Overview

Model-Based Design formed the foundation of this internship. Before developing the digital controller and deploying firmware to the TI C2000 DSP, it was essential to understand the MATLAB ecosystem, simulation workflows, and physical modelling tools.

This section documents the software tools used throughout the internship and the learning process involved in building simulation models prior to hardware implementation.

The workflow follows a Model-Based Design approach, where system modelling, controller development, verification, and testing are completed in simulation before deployment to embedded hardware.

---

# Software Environment

The following software was used throughout the project.

| Software | Purpose |
|----------|---------|
| MATLAB | Numerical computing and scripting |
| Simulink | Block-diagram based system modelling |
| Simscape Electrical | Physical modelling of the power converter |
| Embedded Coder | Automatic embedded code generation |
| C2000 Microcontroller Blockset | Integration with TI C2000 DSPs |
| Code Composer Studio | Firmware development and debugging |

---

# MATLAB

MATLAB was used for:

- Numerical computation
- Control system analysis
- Parameter calculations
- Model verification
- Data visualization
- Simulation analysis

Topics explored include:

- MATLAB workspace
- Variables and scripts
- Plotting
- Mathematical modelling
- Parameter initialization

---

# Simulink

Simulink served as the primary modelling environment.

Major concepts explored include:

- Block diagrams
- Signal routing
- Sources
- Sinks
- Mathematical operations
- Discrete systems
- Continuous systems
- Simulation configuration
- Solver settings

---

# Simscape Electrical

Simscape Electrical enabled the creation of a high-fidelity digital twin of the synchronous buck converter.

Key components studied include:

- DC Voltage Sources
- MOSFETs
- Inductors
- Capacitors
- Resistors
- Current Sensors
- Voltage Sensors
- Electrical Reference
- PWM Generator

These components were later integrated into the complete converter model.

---

# Model-Based Design Workflow

```text
Engineering Problem
        │
        ▼
MATLAB Analysis
        │
        ▼
Simulink Model
        │
        ▼
Simscape Electrical
        │
        ▼
Simulation
        │
        ▼
Controller Design
        │
        ▼
Embedded Code
        │
        ▼
DSP Deployment
```

---

# Skills Developed

During this phase of the internship, I gained experience in:

- MATLAB Programming
- Simulink Modelling
- Simscape Electrical
- Physical System Modelling
- Simulation Configuration
- Parameter Initialization
- Signal Processing
- Model-Based Design
- Simulation Verification

---

# Folder Organization

```text
MATLAB/
    Scripts/
    Examples/
    Images/

Simulink/
    Models/
    Examples/
    Images/

Simscape/
    Models/
    Components/
    Images/

Resources/
```

---

# Learning Outcomes

Completing this phase established the software foundation required for developing the digital twin, designing the cascade controller, and deploying embedded firmware to the TI C2000 DSP.

---

# Next Section

Continue to:

**03_Digital_Twin**

where the synchronous buck converter is modelled and validated using Simscape Electrical before controller implementation.