# Smart University Firmware (v1) 📡

This folder contains the **Main Firmware** for the Smart University IoT nodes, designed for **ESP32**. It integrates multiple sensors and actuators to manage classroom environments and security.

## 🧠 Hardware & Components

*   **Microcontroller**: ESP32
*   **Sensors**:
    *   `MQ2`: Gas/Smoke detection.
    *   `HC-SR04`: Ultrasonic distance sensor.
    *   `PIR`: Motion detection.
    *   `RFID-RC522`: Student attendance tracking.
*   **Actuators**:
    *   `Servo Motors`: Door control, Smart Window.
    *   `RGB LED`: Status indication.
    *   `LCD (I2C)`: Information display ("Ahla bik").
    *   `Buzzer`: Alarm system.

## 💻 Code Overview

*   **`SMART_FST_1.ino`**: The core logic file.
    *   **Connectivity**: Connects to WiFi (`Orange-1FC9`) and syncs time via NTP.
    *   **Supabase Integration**:
        *   Sends sensor data to table `sensor_readings`.
        *   Checks `rfid_cards` table for authentication.
        *   Logs attendance to `daily_attendance`.
        *   Polls `actuators` table to remotely control devices (Lights, Buzzer).

## ⚠️ Configuration
**Note**: The source code contains hardcoded credentials. For production usage, please update:
*   `ssid` & `password`: Your WiFi credentials.
*   `supabaseAPIKey`: Your Supabase Project API Key.
