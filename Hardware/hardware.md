# JARVIS Hardware Specification (Rev 1.0)

**Architecture:** Modular Carrier Board (Motherboard Style)  
**PCB Type:** Single-Sided FR4  
**Power System:** 5V Boosted LiPo (USB-C Rechargeable)

## Design
To ensure repairability and ease of assembly, the JARVIS Mainboard does not use surface-mount (SMD) chips for complex logic. Instead, it acts as a **Carrier Board** with Female Header Sockets. 
* **Modules (ESP32, Charging, Boost):** Plugged into headers (Replaceable).
* **Interface (USB, Buttons, LEDs):** Soldered directly (Through-Hole).

## Hardware Pinout Strategy

### Power Flow
`USB-C (5V)` -> `TP4056` -> `Battery (3.7V)` -> `Switch` -> `MT3608 Boost` -> `5V SYSTEM RAIL`

### User Interface Logic
The device uses dedicated buttons to switch the system `State Machine`.

| Hardware Input | Designated Function | Indicator LED |
| :--- | :--- | :--- |
| **Button 1** | **POMODORO MODE** (Timer Set/Start) | 🔴 Red LED |
| **Button 2** | **CAMERA MODE** (Wake ESP32-CAM) | 🟢 Green LED |
| **Button 3** | **RFID MODE** (Scan Tag/Unlock) | 🔵 Blue LED |
| **Rotary Encoder** | **SCROLL / SELECT** (Global) | N/A |

## Assembly Notes (Warning)
1.  **Voltage Calibration:** You **MUST** plug in the MT3608 Boost Module alone first, and turn the potentiometer screw until the output reads **5.0V**. If it reads 9V or 12V, it will destroy the ESP32s.
2.  **Socket Orientation:** Ensure the Female Headers for the ESP32-CAM are soldered correctly; the module has no reverse-voltage protection.