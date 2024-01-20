# Audio Circuit Configuration

## Components and Connection Overview
This document details the configuration of an audio circuit using a 3.5mm audio jack output breakout board, a 10 µF polarized tantalum capacitor, and an appropriate bleeder resistor. The connections are as follows:

1. **3.5mm Audio Jack Output Breakout Board**: 
    - Tip connected to the positive side of the tantalum capacitor and the Mozzi audio out pin.
    - Ring1 also connected to the Tip (assumed to be for stereo output).
    - Sleeve connected to the negative side of the tantalum capacitor and the ground.

2. **10 µF Polarized Tantalum Capacitor**: 
    - Positive side connected to the Tip of the 3.5mm audio jack and the Mozzi audio out pin.
    - Negative side connected to the Sleeve of the 3.5mm audio jack (ground).

3. **Bleeder Resistor**: 
    - Connected in parallel to the tantalum capacitor (across the positive and negative terminals).
    - In our case a resistor with 100 ohms (circuit has operating voltage of 5v)

## Circuit Operation
The circuit utilizes the tantalum capacitor for its audio filtering properties, ensuring a stable and clean audio signal. The bleeder resistor, calculated as per the guidelines in the `BLEEDER.md` cheatsheet, ensures safe discharge of the capacitor when the circuit is powered off.

## Reference
For the calculation of the bleeder resistor, refer to the [BLEEDER.md](BLEEDER.md) cheatsheet. This document provides a comprehensive guide on selecting the right resistor value to ensure safety and optimal performance of the audio circuit.

## Safety Note
When assembling and testing the circuit, ensure all components are correctly rated and properly connected. Follow standard safety protocols while working with electrical components.
