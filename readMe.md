

# IR Recorder Project v1

- version: 0.0.3

This project utilizes an **IR Receiver**, an **OLED Display**, and a **Keypad** with an STM32 microcontroller.

## Pin Assignments

| Component          | Arduino Pin| STM32 Pin     |
|---------------     |------------|------------   |
| **IR Receiver**    | D2         | PB0           |
| **IR LED**         | Unknown    | PB1           |
| **Keypad Analog**  | A0         | PA0           |
| **Keypad Row 1**   | A1         | PA1           |
| **Keypad Row 2**   | A2         | PA2           |
| **Keypad Row 3**   | A3         | PA3           |
| **I2C SDA (OLED)** | A4         | PB7 (Default) |
| **I2C SCL (OLED)** | A5         | PB6 (Default) |

## Features
- **IR Signal Recording & Display**
- **4x4 Keypad Handling**
- **OLED Output**

## Dependencies
- `IRremote.h`
- `Wire.h`
- `Adafruit_GFX.h`
- `Adafruit_SSD1306.h`

## Usage
1. Connect the components following the table above.
2. Upload the code to your STM32.
3. Open Serial Monitor to check IR signals and keypad inputs.

### Notes
- Adjust the I2C address (`0x3C` or `0x3D`) if the OLED display does not work.
- Ensure that the STM32 ADC uses **3.3V** reference voltage.
