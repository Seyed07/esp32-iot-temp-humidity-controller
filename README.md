# IoT Temperature & Humidity Control System (ESP32 | DHT11 | Servo | Bluetooth)

## Overview

This project implements a **wireless temperature and humidity control system** using an ESP32 microcontroller, a DHT11 sensor, servo motors (as actuators for a fan and heater), LEDs for status indication, and Bluetooth connectivity for remote monitoring and setpoint adjustment via a mobile device.

The system automatically maintains the ambient temperature within a specified range (deadband), and allows manual override of actuators through a mobile Bluetooth terminal.

---

## Features

- Real-time temperature & humidity monitoring (DHT11 sensor)
- Wireless data display and control via Bluetooth + mobile app
- User-adjustable temperature setpoint through phone
- Deadband logic (±5°C around setpoint) for stable control
- Servo-controlled fan and heater operation
- Visual status indication using LEDs
- Manual control of actuators via Bluetooth commands (e.g. left/right for vent/flap adjustment)
- Calibrated sensor readings for accuracy

---

## System Block Diagram

<p align="center">
  <img src="https://github.com/user-attachments/assets/d4345be8-ba46-4164-b360-69db06aa2da6" 
       alt="System Block Diagram" 
       style="max-width: 400px; width: 60%; height: auto; display: block; margin: auto;"/>
</p>


---

## Hardware Components

| Part                   | Quantity | Description                          |
|------------------------|----------|--------------------------------------|
| ESP32 Dev Board        | 1        | Main controller with Bluetooth       |
| DHT11 Sensor           | 1        | Temperature & humidity sensor        |
| Micro Servo Motors     | 2        | For fan & heater actuating           |
| LEDs (2 colors)        | 2        | Setpoint/status indication           |
| Push-button Switch     | 1        | Manual override (optional)           |
| Breadboard & wires     | -        | For prototyping connections          |
| Power supply           | 1        | USB or external (as needed)          |


## Workflow & Deadband Logic

- **Sensor Input**: DHT11 measures temperature/humidity and sends to ESP32.
- **Setpoint input**: User sets desired temperature (`setpoint`) from a Bluetooth terminal (Android phone).
- **Deadband**:  
  - If temperature < (setpoint - 5°C): **Heater ON, Fan OFF**
  - If (setpoint - 5°C) ≤ temperature ≤ (setpoint + 5°C): **Heater OFF, Fan OFF, Setpoint LED ON**
  - If temperature > (setpoint + 5°C): **Fan ON, Heater OFF**
- **Manual Override**: User can control vent/flap via servo using 'left'/'right' Bluetooth commands.


<p align="center">
  <img src="https://github.com/user-attachments/assets/c9e12412-b499-4a21-b12e-81776fb35ae3"
       alt="Deadband Logic Workflow Diagram"
       style="max-width:400px;width:60%;height:auto;display:block;margin:auto;">
</p>

---

## Getting Started

### 1. **Installation & Setup**

- Clone this repository:
  ```bash
  git clone https://github.com/Seyed07/esp32-iot-temp-humidity-controller
  ```
- Install [Arduino IDE](https://www.arduino.cc/en/software) and required libraries:
  - `ESP32 Board package`
  - `DHT sensor library`
  - `Servo library`
  - `BluetoothSerial (built-in for ESP32)`

### 2. **Wiring**

- Connect components as shown in the schematic diagram.
- Confirm ESP32 GPIO pin mapping in code matches wiring.

### 3. **Flashing Code**

- Edit `src/main.ino` as needed (specifically, set your setpoint, pin numbers, etc.).
- Use Arduino IDE to upload the code to the ESP32 board.

### 4. **Operation**

- Use a Bluetooth terminal app on your phone (e.g. "Serial Bluetooth Terminal") to connect to ESP32.
- Set the desired temperature by sending the setpoint value.
- Use 'left'/'right' commands to control vent/flap motors.
- Monitor live temperature/humidity and system status via app.

---

## Example Code Snippet

```cpp
#include <BluetoothSerial.h>
#include <DHT.h>
#include <Servo.h>

#define DHTPIN 4       // Example GPIO for DHT11
#define DHTTYPE DHT11

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);
Servo fanServo, heaterServo;

int setpoint = 25;
int deadband = 5;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Controller");
  dht.begin();
  // Attach servos to pins...
}

void loop() {
  float temp = dht.readTemperature();
  // Bluetooth setpoint receive code here...

  if(temp < setpoint - deadband) { /* Heater ON, Fan OFF */ }
  else if(temp > setpoint + deadband) { /* Fan ON, Heater OFF */ }
  else { /* Both OFF, LED ON */ }
  // Manual control...
}
```
See [`src/main.ino`](src/main.ino) for complete and commented code.

---
