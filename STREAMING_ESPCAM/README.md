# Video Streaming Server 🎥

A dedicated Node.js server designed to relay low-latency video streams from ESP32-CAM modules to the Smart School Mobile App and Web Dashboard.

## ⚙️ Architecture

This server acts as a **WebSocket Relay**:
1.  **Ingest**: Accepts raw MJPEG video frames from the **ESP32-CAM** via WebSocket.
2.  **Broadcast**: Relays these frames instantly to all connected clients (Mobile App / Web Browser).

## 🔌 API Endpoints

*   **Ingest (Input)**: `ws://<SERVER_IP>:8080/esp32`
    *   Used by the ESP32-CAM to send video data.
*   **Stream (Output)**: `ws://<SERVER_IP>:3000/stream`
    *   Used by the Flutter App or Web Client to watch the video.
*   **Web View**: `http://<SERVER_IP>:3000/`
    *   Simple browser-based viewer.

## 🚀 Setup & Run

1.  **Install Dependencies**:
    ```bash
    npm install
    ```

2.  **Start the Server**:
    ```bash
    npm start
    ```
    *   *Note*: Ensure your firewall allows traffic on ports `3000` and `8080`.

## 📦 Dependencies

*   `express`: Web server framework.
*   `ws`: WebSocket library for real-time communication.
