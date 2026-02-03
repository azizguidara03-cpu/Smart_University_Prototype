# Smart School Mobile App 📱

The **Smart School Mobile App** is the central interface for students and administrators to interact with the Smart University ecosystem. It provides real-time monitoring, attendance tracking, and alerts.

## 🌟 Key Features

*   **User Authentication**: Secure login for students and staff (via Supabase).
*   **Dashboard**: Overview of campus status (temperature, alerts).
*   **Real-time Streaming**: View live video feeds from campus security cameras (ESP32-CAM).
*   **Attendance Tracking**: Integration with RFID systems to track student presence.
*   **Notifications**: Instant alerts for fire, intrusion, or other emergencies.

## 🛠 Tech Stack

*   **Framework**: [Flutter](https://flutter.dev/) (Dart)
*   **Backend**: [Supabase](https://supabase.com/) (Auth, Database, Realtime)
*   **State Management**: `provider`
*   **Video Player**: `video_player` / `flick_video_player`

## 🚀 Getting Started

### Prerequisites

*   [Flutter SDK](https://docs.flutter.dev/get-started/install) installed.
*   An Android Emulator or physical device.

### Installation

1.  **Install dependencies**:
    ```bash
    flutter pub get
    ```

2.  **Run the app**:
    ```bash
    flutter run
    ```

## 📂 Project Structure

*   `lib/`: Source code for the application.
    *   `screens/`: UI pages (Login, Dashboard, Profile).
    *   `services/`: Supabase and API integration.
    *   `widgets/`: Reusable UI components.
