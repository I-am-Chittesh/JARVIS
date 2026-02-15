# JARVIS

**Status:** Active Development (PCB Prototyping Phase)  
**Version:** v1.0.0-alpha

## 1. Overview
Project JARVIS is a dedicated, handheld hardware interface designed to bridge the physical world with the user’s digital ecosystem (**Physique.io** and **FlowState**). 

Unlike a smartphone, which is a source of distraction, JARVIS is a "Single-Purpose" tool designed to reduce friction. It allows the user to log data, authenticate work sessions, and capture visual context without ever unlocking a screen.

## 2. Core Functionality

### Voice Mode
* **Action:** Hold the dedicated voice button.
* **Function:** Captures high-fidelity audio commands (e.g., *"Logged 200g chicken breast"* or *"Add 'Fix API Bug' to Todo"*).
* **Backend:** Audio is synced via WiFi, processed by STT (Speech-to-Text), and parsed into the correct database (Notion/Physique.io).

### Pomodoro
* **Action:** Rotate the physical encoder to set a timer duration.
* **Function:** Initiates a "Deep Work" session. The device enters `FOCUS_MODE`, muting notifications and tracking the session in the FlowState database.

### Cam Module
* **Action:** Single-click to snap a photo; Long-press to record a 10s clip.
* **Function:** Uses a dedicated vision processor (ESP32-CAM) to capture workspace setups, meals, or project progress. Media is uploaded in the background.

### RFID Authentication
* **Action:** Tap an RFID card/tag.
* **Function:** * **Security:** Unlocks the device.
    * **Context Switching:** Triggers specific Spotify playlists (e.g., "Deep Focus" vs. "Gym Mode") via the Spotify API.

## 3. System Architecture
The device operates on a **Dual-MCU Architecture**:
* **Primary Brain:** ESP32-WROOM-32 (Handles WiFi, Audio, RFID, Logic).
* **Vision Brain:** ESP32-CAM (Handles Optics & SD Storage).

---
*Built with ❤️ by I_am_Chittesh*