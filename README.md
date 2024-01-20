# MycoSonic MK1

## Introduction
Welcome to the MycoSonic MK1, an Arduino-based interface designed for transforming biological signals, specifically from mushrooms, into a unique auditory experience. This README provides an overview of its features, setup, and usage.

## Features
- **Signal Processing:** Advanced processing of input signals with a variety of filters and mappings.
- **Dynamic Waveforms:** Support for Sine, Triangle, Square, and Sawtooth waveforms.
- **Input Options:** Capability to receive data from biosensors or SD cards.
- **Audio Effects:** A range of audio effects including Phase Modulation, Tremolo, Bit Crusher, and more.
- **Customizable Scales and Timing:** Adjustable BPM, note duration, and support for multiple musical scales.
- **Output Flexibility:** Options for audio output, SD card data logging, and MIDI output.

## Hardware Requirements
- Arduino (ATmega2560) board
- Biosensor AD8232 (analog output to `A0`)
- SD card module (optional)
- Audio output module (compatible with Mozzi library)
- LCD and input buttons for menu navigation

## Setup
1. **Hardware Setup:** Connect the biosensor, SD card, and audiou output module to the Arduino.
2. **Install Libraries:** Ensure [libraries](MK1/platformio.ini) like Mozzi, SPI, and others necessary for your hardware are installed in the Arduino IDE.

## Usage
1. **Start:** Power on the Arduino to view the main menu on the LCD.
2. **Navigate:** Use the input buttons to navigate the menu.
3. **Configure:**
    - Select input source (Sensor/SD Card).
    - Adjust filtering and mapping settings.
    - Set waveform and audio effects.
4. **Operate:** After configuration, the device will process signals and output audio based on your settings.

## Advanced Features
- **Filter Options:** Moving average, low/high pass, notch, median, and Kalman filters.
- **Scale Mapping:** Supports various musical scales.
- **Effect Customization:** Enable/disable and arrange audio effects.

## Troubleshooting
- **No Audio Output:** Check audio module connections and configuration.
- **Signal Processing Issues:** Calibrate the sensor and adjust filter settings.
- **Menu Navigation Problems:** Ensure input buttons are functioning properly.

## Contributing
Contributions are welcome. Feel free to fork, improve, and submit pull requests.

## License
This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.

## Acknowledgements
Thanks to the open-source community and all further contributors.

---

Create your bio-symphonies with MycoSonic MK1! 🍄🎵
