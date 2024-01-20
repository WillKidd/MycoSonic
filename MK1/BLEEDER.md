
# Bleeder Resistor to Capacitor Cheatsheet

## Introduction
A bleeder resistor is a resistor connected across the terminals of a capacitor in an electrical circuit. Its primary function is to safely discharge the capacitor after power is turned off, preventing potential electric shock or damage to other components.

## Basic Formula
The value of the bleeder resistor can be calculated using the formula:

\[ R = rac{V^2}{P} \]

Where:
- \( R \) is the resistance of the bleeder resistor (in ohms, Ω)
- \( V \) is the voltage across the capacitor (in volts, V)
- \( P \) is the power dissipation capacity of the resistor (in watts, W)

## Considerations
- **Voltage Rating**: The resistor should have a voltage rating higher than the maximum voltage of the capacitor.
- **Power Rating**: Ensure the resistor can handle the power dissipation without overheating.
- **Discharge Time**: A higher resistance value will result in slower discharge of the capacitor.

## Example Calculation
Assume a capacitor charged to 50V and a desired power dissipation of 0.25W. Using the formula:

\[ R = rac{50^2}{0.25} = 10,000 Ω \]

Thus, a 10kΩ resistor would be suitable.

## Safety Note
Always ensure capacitors are fully discharged before working on a circuit. The use of bleeder resistors adds a layer of safety but does not replace the need for caution and proper handling of electrical components.

## Common Capacitor Values in Audio Circuits

The following table lists some common capacitor values used in audio circuits, along with their typical applications:

| Capacitor Value | Application                  | Description                                  |
|-----------------|------------------------------|----------------------------------------------|
| 0.1 µF          | Decoupling                   | Used for decoupling power supplies           |
| 1 µF            | Coupling                     | Commonly used for coupling audio signals     |
| 10 µF           | Bypassing                    | Used in power supply bypass applications     |
| 47 µF           | Power Supply Filtering       | Used for filtering in power supply circuits  |
| 100 µF          | Large-Scale Filtering        | For more extensive filtering requirements    |
| 220 µF          | Electrolytic, General-Purpose| Widely used in various audio applications    |
| 470 µF          | Power Supply, Large Capacity | For high-capacity power supply applications  |
| 1000 µF         | High-Capacity Filtering      | Used where high-capacity filtering is needed |

Note: Capacitor values are often chosen based on specific circuit requirements and may vary based on design and function.
