# Healthcare Monitoring System Prototype

## Overview
This project is a prototype for a **Healthcare Monitoring System** using an **Arduino-based microcontroller**. The system monitors vital health parameters, including:

- **Heart Rate & SpO2 (Oxygen Saturation)** using **MAX30100 Pulse Oximeter**.
- **Body Temperature** using **DS18B20 Temperature Sensor**.
- **ECG (Electrocardiogram) Signal** using an **ECG module**.
- **OLED Display** for real-time visualization of readings.

## Components Used
- **Arduino Board (ESP32 / Arduino Uno)**
- **MAX30100 Pulse Oximeter**
- **DS18B20 Temperature Sensor**
- **ECG Sensor (AD8232 or similar)**
- **OLED Display (128x64, I2C)**
- **Jumper Wires & Breadboard**

## Installation & Setup
1. **Install Required Libraries:**
   - `MAX30100_PulseOximeter.h`
   - `U8g2lib.h` (for OLED Display)
   - `OneWire.h` and `DallasTemperature.h` (for DS18B20)

2. **Connect the Components:**
   - **Pulse Oximeter (MAX30100)** to **I2C Pins (SDA, SCL)**.
   - **Temperature Sensor (DS18B20)** to **Pin 4**.
   - **ECG Sensor** to **Analog Pin A0/A1 (ESP32: Pin 34)**.
   - **OLED Display** to **I2C (SDA, SCL)**.

3. **Upload the Code:**
   - Open `V1_0.ino` in the **Arduino IDE**.
   - Select the correct **Board & Port**.
   - Click **Upload**.

## Usage
- Once powered on, the device will begin monitoring **Heart Rate, SpO2, ECG, and Temperature**.
- The **OLED display** will show real-time health data.
- Open **Serial Monitor (115200 baud)** to view detailed logs.

## Code Explanation
- **Setup Function (`setup()`)**
  - Initializes **OLED Display, Pulse Oximeter, and Temperature Sensor**.
  - Registers a callback for **heartbeat detection**.
- **Loop Function (`loop()`)**
  - Continuously reads and displays data from sensors.
  - Logs readings to the **Serial Monitor** every second.

## Example Output (Serial Monitor)
```
Pulse, SpO2, ECG & Temperature Monitoring System
Beat detected!
Heart Rate: 75 bpm, SpO2: 98%
Temperature: 36.5°C
ECG Signal: 512
```

## Future Enhancements
- **Bluetooth/Wi-Fi Connectivity** for remote health monitoring.
- **Data Logging & Analysis** using cloud integration.
- **Mobile App Interface** for real-time monitoring.

## License
This project is confidential property of UnidemyGlobal under the  **MIT License**.

---


**Author:** Unidemy Global 
**GitHub Repository:** (https://github.com/Unidemy-Global/EmbededSystems)
