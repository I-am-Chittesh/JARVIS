Technical Documentation: Modular Cyber-Physical Gateway (SBC Architecture)
==========================================================================

```
Current Status: Pre-prototying
Version: v1.0.0 -beta
```

1\. System Overview
-------------------

This project details the development of a standalone, pocketable Single Board Computer (SBC) device designed to bridge physical tactile interactions with external operating systems and local computing processes. Moving away from rigid, single-purpose microcontroller setups, this device runs a lightweight Linux operating system capable of running custom-engineered applications.

The user interface relies entirely on a **Finite State Machine (FSM)**. A single set of physical hardware controls dynamically switches functional profiles depending on the active firmware mode. The display layer utilizes a streamlined graphical framework (such as LVGL) stylized with a high-contrast, dark aesthetic to prioritize immediate readability.

2\. Hardware Architecture & Bill of Materials (BOM)
---------------------------------------------------

The hardware components are selected to balance processing power for local vision and system processing tasks while strictly maintaining a pocketable form factor.

*   **Core Processing Unit (SBC):** Raspberry Pi Zero 2 W. Provides a quad-core processor capable of executing custom multi-threaded scripts and handling fast web/local networking within a compact footprint.
    
*   **Primary Display:** Waveshare 2.8-inch Capacitive Touch IPS LCD (480x640). Positioned vertically to offer a mini-phone aspect ratio, utilizing a high-speed SPI/DPI communication bus to keep general GPIO pins open.
    
*   **Optical Module:** Sony IMX708 (12-Megapixel Autofocus). Connected directly via the native CSI ribbon port, providing high-resolution image capture and crucial hardware autofocus for close-range data logging.
    
*   **Tactile Controls:**
    
    *   1x EC11 Rotary Encoder (with integrated push-button functionality).
        
    *   2x Low-Profile Tactile Push Buttons (designated as Button A and Button B).
        
*   **Power Infrastructure:** IP5306 Power Management IC paired with a 1000mAh flat Lithium-Polymer (LiPo) battery, enabling safe, integrated 5V boost regulation and USB-C recharging.
    

3\. Firmware Logic & State-Driven Interface
-------------------------------------------

The device eliminates fixed buttons for fixed tasks. The internal execution layer maps the physical inputs dynamically based on the current system environment.

### Default State: System Gateway Mode

*   **Function:** Acts as the base communication layer with the paired external host PC.
    
*   **Rotary Encoder (Turn):** Transmits volume modulation or scrolling vectors to the external host OS.
    
*   **Button A:** Transitions the system into _Functional State 1_.
    
*   **Button B:** Transitions the system into _Functional State 2_.
    

### Functional State 1: System Regulation (Custom App Compatible)

*   **Function:** Provides a dedicated, hardware-gated execution state designed to host custom productivity or system-locking automation apps.
    
*   **Rotary Encoder (Turn):** Adjusts numerical variables or countdown time arrays locally on the screen.
    
*   **Rotary Encoder (Press):** Toggles active execution routines (Start/Pause).
    
*   **Button A + B (Simultaneous Hold):** Triggers a hardware override signal to terminate current host restrictions and return to the Default State.
    

### Functional State 2: High-Resolution Data Logging (Custom App Compatible)

*   **Function:** Activates peripheral data acquisition, leveraging the onboard optical module to process, tag, and organize local inputs via custom scanning or recognition applications.
    
*   **Button B:** Triggers the camera shutter to capture optical frames.
    
*   **Rotary Encoder (Turn):** Cycles through local evaluation parameters or contextual lists populated on the LCD screen.
    
*   **Button A:** Commits the current data payload to the local storage array or remote cloud databases.
    

4\. Digital Proof of Concept (Wokwi Simulation Framework)
---------------------------------------------------------

To demonstrate "Reduction to Practice" for project evaluations or patent examinations, the operational logic of this FSM can be validated digitally within a web-based emulation environment like **Wokwi**.

While Wokwi handles microcontrollers rather than full Linux SBCs, it perfectly demonstrates the mechanical input behavior and state transitions.

### Simulation Protocol:

1.  **Logic Stand-In:** An ESP32 or Raspberry Pi Pico is placed in the workspace to simulate the GPIO pin mapping of the target SBC.
    
2.  **Peripheral Layout:** The simulator connects an ILI9341 2.8-inch TFT LCD panel (representing the mini-phone screen display), an EC11 rotary encoder, and two standalone tactile push buttons to the controller.
    
3.  **The Code Matrix:** A lightweight C++ script utilizes a clean execution loop to prove the mode switching:
    
    *   _Mode 0 (Hub):_ The display indicates a standby connection. Turning the encoder outputs text strings simulating generic system volume controls via the serial terminal.
        
    *   _Mode 1 (State 1 Active):_ Pressing Button A clears the screen to display the custom system timer UI. The encoder now explicitly updates the timer value instead of standard system controls.
        
    *   _Mode 2 (State 2 Active):_ Pressing Button B shifts the UI to an imaging preview frame, where pressing the encoder triggers a dedicated digital pin (lighting a virtual LED) to prove camera capture sync.