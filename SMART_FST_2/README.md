# Smart University Firmware (v2) 🌡️

This folder contains the **Secondary Firmware** designed for **ESP8266 (NodeMCU)** modules. It focuses on environmental monitoring and simple automation.

## 🧠 Hardware & Components

*   **Microcontroller**: ESP8266 (NodeMCU / Wemos D1)
*   **Sensors**:
    *   `DHT11`: Temperature & Humidity.
    *   `LDR`: Light intensity sensor.
*   **Actuators**:
    *   `Relay`: Fan control.
    *   `LED`: Lighting control.
    *   `Servo`: Window control.

## 💻 Code Overview

*   **`SMART_FST_2.ino`**:
    *   **Periodic Updates**: Reads sensors every 10 seconds.
    *   **HTTPS Secure**: Uses `WiFiClientSecure` for stable Supabase connection.
    *   **Key Functions**:
        *   `postSensorReading`: Uploads Temp/LDR data.
        *   `updateActuatorState`: Polls Supabase to sync Actuators (Fan, LED, Window).

## ⚠️ Configuration
**Note**: Ensure to update the Wi-Fi and API keys in the code before flashing.
