# Smart University Prototype

This repository houses the prototypes and source code for the **Smart University Project**, an integrated system combining mobile applications, IoT sensors, and video streaming for campus management.

## 📂 Repository Structure

The project is organized into the following key components:

### 📱 `smart_school-main` (Mobile Application)
A **Flutter** application serving as the main interface for students and staff.
- **Path**: `/smart_school-main`
- **Tech Stack**: Flutter (Dart)
- **Features**: User authentication, dashboard, notifications.

### 🎥 `STREAMING_ESPCAM` (Video Streaming Server)
A dedicated **Node.js** server for handling video streams, designed to interface with ESP32-CAM modules.
- **Path**: `/STREAMING_ESPCAM`
- **Tech Stack**: Node.js, Express, WebSocket
- **Setup**:
  ```bash
  cd STREAMING_ESPCAM
  npm install
  npm start
  ```

### 📡 `SMART_FST` (IoT Firmware)
Arduino firmware code for sensor modules (Fire detection, Temperature, etc.).
- **Path**: `/SMART_FST_1` and `/SMART_FST_2`
- **Tech Stack**: C++ (Arduino)
- **Hardware**: ESP32 / Arduino compatible boards.

---

## 🚀 Getting Started

1.  **Clone the repository**:
    ```bash
    git clone https://github.com/azizguidara03-cpu/Smart_University_Prototype.git
    ```

2.  **Mobile App Setup**:
    - Ensure you have Flutter installed.
    - Navigate to `smart_school-main` and run:
      ```bash
      flutter pub get
      flutter run
      ```

3.  **Server Setup**:
    - Navigate to `STREAMING_ESPCAM` and install dependencies as shown above.

## 🛠 Contributing
Please ensure you do not commit `node_modules` or build artifacts. The repository includes a `.gitignore` to handle this automatically.
