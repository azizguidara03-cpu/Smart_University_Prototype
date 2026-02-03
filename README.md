# Smart University Prototype 🎓

![Project Status](https://img.shields.io/badge/Status-Prototype-orange)
![License](https://img.shields.io/badge/License-MIT-blue)

The **Smart University Project** is a comprehensive IoT and software ecosystem designed to modernize campus management. It combines mobile applications, real-time video streaming, and intelligent sensor networks to create a safer, smarter learning environment.

## 📂 Repository Components

This repository is modularized into three main systems. Click the links below for detailed documentation on each:

### 📱 1. [Mobile Application](./smart_school-main/README.md)
*   **Path**: `/smart_school-main`
*   **Tech**: Flutter (Dart), Supabase.
*   **Role**: The frontend interface for students and staff to view dashboards, receive alerts, and track attendance.

### 🎥 2. [Streaming Server](./STREAMING_ESPCAM/README.md)
*   **Path**: `/STREAMING_ESPCAM`
*   **Tech**: Node.js, Express, WebSockets.
*   **Role**: A high-performance relay server that broadcasts live video from ESP32-CAM nodes to the mobile app.

### 📡 3. [IoT Firmware](./SMART_FST_1/README.md)
*   **Path**: `/SMART_FST_1` (ESP32) & `/SMART_FST_2` (ESP8266).
*   **Tech**: C++, Arduino.
*   **Role**: The brain of the physical classroom, managing fire detection, smart locks, lighting, and environmental control.

---

## 🚀 Quick Setup Guide

1.  **Clone the Repo**:
    ```bash
    git clone https://github.com/azizguidara03-cpu/Smart_University_Prototype.git
    ```
2.  **App**: Go to `smart_school-main`, run `flutter pub get` and `flutter run`.
3.  **Server**: Go to `STREAMING_ESPCAM`, run `npm install` and `npm start`.
4.  **Hardware**: Flash the `.ino` files in `SMART_FST_*` to your ESP devices.

## 🤝 Contributing
Please follow standard git practices. Do not commit sensitive keys or `node_modules`. A `.gitignore` is provided.
