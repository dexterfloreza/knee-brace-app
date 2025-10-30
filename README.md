# Multi-Sensor Wearable Knee Brace System
This project is a mobile and microcontroller-based knee brace monitoring system. It collects sensor data (yaw, pitch, roll, flexion, pressure), processes it on an ESP32, and streams the data via BLE to a client application. The goal is to support rehabilitation, joint tracking, and clinical intervention through real-time alerts and historical analysis.

## System Architecture

The system follows a five-layer design:
1. **Physical Layer**: BNO055 IMU, ZD10-100 flex sensor, FSR402 pressure sensor, ESP32.
2. **Communication Layer**: BLE transmission via GATT service.
3. **Processing Layer**: Python BLE logger parses and saves CSV data.
4. **Data & UI Layer**: Home dashboard and session history interface.
5. **Application Layer**: (Planned) alerts, clinician exports, overuse monitoring.
