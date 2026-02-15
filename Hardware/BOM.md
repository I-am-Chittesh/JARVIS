## BOM

### A. The Brains (Plug-in Modules)
| Component | Quantity | Footprint | Notes |
| :--- | :--- | :--- | :--- |
| **ESP32-WROOM-32 DevKit V1** | 1 | 2x15 Pin Headers | Primary Logic Unit. (Ensure 30-pin version). |
| **ESP32-CAM (AI Thinker)** | 1 | 1x16 Pin Header | Dedicated Vision Unit. |

### B. Power System (Hybrid THT/Module)
| Component | Quantity | Footprint | Notes |
| :--- | :--- | :--- | :--- |
| **USB-C Connector (6-Pin)** | 1 | THT (Type-C) | **Native PCB Component.** (Pins: VBUS, GND, CC1, CC2). |
| **TP4056 Charging Module** | 1 | Header Socket | Lithium Battery Charger (5V Input). |
| **MT3608 Step-Up Module** | 1 | Header Socket | **Crucial:** Must be calibrated to 5.0V output before use. |
| **LiPo Battery** | 1 | JST-PH 2.0 | 3.7V, 2000mAh (Flat Pack). |
| **Slide Switch (SPDT)** | 1 | THT (SS-12D00) | Master System ON/OFF (Cuts Battery Line). |
| **Resistor (5.1kΩ)** | 2 | THT (1/4W) | Pull-down resistors for USB-C CC lines (Required for C-to-C charging). |

### C. Sensors & Input
| Component | Quantity | Footprint | Notes |
| :--- | :--- | :--- | :--- |
| **INMP441 Microphone** | 1 | Header Socket | I2S Digital Audio Interface. |
| **RC522 RFID Reader** | 1 | Header Socket | SPI Interface (13.56MHz). |
| **Rotary Encoder (EC11)** | 1 | THT (5-pin) | Includes Push Button (Menu Select). |
| **Tactile Buttons** | 3 | THT (6x6mm) | Mode Selectors (Pomo, Cam, RFID). |

### D. Output & Indicators
| Component | Quantity | Footprint | Notes |
| :--- | :--- | :--- | :--- |
| **1.3" OLED Display** | 1 | Header Socket | I2C Interface (SSD1306 Driver). |
| **LED (3mm/5mm)** | 3 | THT | Status Indicators: Red (Focus), Green (Cam), Blue (Auth). |
| **Resistor (220Ω)** | 3 | THT (1/4W) | Current limiting for Status LEDs. |

