## PWM Pins for Mozzi Library on Arduino Mega 2560

| Function | Pin | Description |
|---|---|---|
| Audio Out | Digital Pin 11 | Standard Mozzi pin for audio output. |
| LO+ | Digital Pin 51 | MOSI pin for SPI communication. |
| MISO | Digital Pin 50 | MISO pin for SPI communication. |
| SCK | Digital Pin 52 | SCK pin for SPI communication. |
| SDA | Digital Pin 20 | SDA pin for I²C communication. |
| SCL | Digital Pin 52 | SCL pin for I²C communication. |

### Audio Out (PWM)
The `Audio Out` pin is used for generating audio signals using Mozzi, a library for real-time audio synthesis and processing on Arduino-compatible platforms. The standard Mozzi pin for audio output is Digital Pin 11 on the Arduino Mega 2560. This pin provides a PWM (Pulse-Width Modulation) signal that can be used to create a wide range of sounds.

### LO+ (Low-Output)
The `LO+` pin is used for various functions, including MOSI (Master Out Slave In) for SPI (Serial Peripheral Interface) communication, which allows for high-speed data transfer between the Arduino and other devices. Additionally, `LO+` can be used as an analog output pin for controlling external devices.

### MISO (Master In Slave Out)
The `MISO` pin is used for receiving data from SPI devices. It is typically connected to the MOSI pin of the slave device.

### SCK (Serial Clock)
The `SCK` pin provides the synchronization clock signal for SPI communication. It controls the timing of data transfers between the Arduino and SPI devices.

### SDA (Serial Data Line)
The `SDA` pin is used for sending data to I²C (Inter-Integrated Circuit) devices. It communicates at a slower speed than SPI but is more versatile and widely used.

### SCL (Serial Clock Line)
The `SCL` pin provides the clock signal for I²C communication. It synchronizes data transfers between the Arduino and I²C devices.

### Output (Sensor)
The `Output (Sensor)` pin is not specifically defined for PWM output in the context of Mozzi library. It is likely used for connecting external sensors or devices that require digital output signals.

### Fun Fact In the Master/Slave and SPI Pin Naming Conventions
The terms "Master" and "Slave" have been phased out in favor of "Controller" and "Peripheral" to avoid derogatory connotations. The corresponding SPI pin names have also been updated:

* **Master/Slave (OLD) → Controller/Peripheral (NEW)**
* **Master In Slave Out (MISO) → Controller In, Peripheral Out (CIPO)**
* **Master Out Slave In (MOSI) → Controller Out, Peripheral In (COPI)**
* **Slave Select pin (SS) → Chip Select Pin (CS)**
* [Ref: https://docs.arduino.cc/tutorials/generic/introduction-to-the-serial-peripheral-interface/]

These new naming conventions better reflect the actual roles of the devices involved in SPI communication.
