# Bill of Materials (BOM): 

## 1. Core Processing & Compute
| Component | Specification | Quantity | Application / Justification |
| :--- | :--- | :--- | :--- |
| **Single Board Computer (SBC)** | Raspberry Pi Zero 2 W | 1 | Quad-core processor running a custom Linux OS. Chosen for its minimal footprint, processing power for local data tasks, and native web networking. |

## 2. Display & Optical Interface
| Component | Specification | Quantity | Application / Justification |
| :--- | :--- | :--- | :--- |
| **Primary Display** | Waveshare 2.8-inch IPS LCD (480x640) | 1 | Capacitive touch interface. Oriented vertically (mini-phone aspect ratio). Utilizes SPI/DPI to ensure GPIO pins remain available for mechanical inputs. |
| **Optical Sensor Module** | Raspberry Pi Camera Module 3 (Sony IMX708) | 1 | 12-Megapixel with hardware Autofocus. Connects natively via CSI port. Crucial for close-range visual scanning and high-resolution local data logging. |

## 3. Tactile & Mechanical Inputs
| Component | Specification | Quantity | Application / Justification |
| :--- | :--- | :--- | :--- |
| **Rotary Encoder** | EC11 (with integrated push-button) | 1 | Primary scrolling and variable-modulation interface for the Finite State Machine (FSM). |
| **Tactile Push Buttons** | Omron Low-Profile Tactile Switches | 2 | Designated as Button A and Button B. Act as dynamic state-transition triggers and contextual action confirmations. |

## 4. Power Infrastructure
| Component | Specification | Quantity | Application / Justification |
| :--- | :--- | :--- | :--- |
| **Power Management IC (PMIC)** | IP5306 (or equivalent 5V boost circuit) | 1 | Handles safe battery charging and provides 5V step-up regulation to reliably power the SBC and connected peripherals. |
| **Power Cell** | 1000mAh Lithium-Polymer (LiPo) | 1 | Flat-pack form factor. Sized to fit precisely behind the display unit, providing sufficient capacity for portable, burst-mode processing. |