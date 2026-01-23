# 📡 ESP32 Portable Wi-Fi Analyzer

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![Platform](https://img.shields.io/badge/platform-ESP32-orange)
![License](https://img.shields.io/badge/license-MIT-green)

A professional-grade, handheld network analysis tool developed using the **ESP32** microcontroller. This device helps engineers and technicians visualize the 2.4GHz Wi-Fi spectrum, analyze signal strength (RSSI), and detect security protocols in real-time.

Unlike standard scanners, this project implements a custom **State Machine Architecture** for stability and features a **Deep Scan** algorithm to detect weak signals without compromising UI responsiveness.

## ✨ Key Features

* **📊 Real-Time List Mode:** Displays SSID, RSSI (dBm), and Security status (Open/Secured) with a scrollable interface. Handles hidden networks intelligently.
* **📈 Spectrum Graph Mode:** Visualizes channel congestion (Ch 1-13) to identify interference and help optimize router placement.
* **🔍 Detail View:** Provides deep insights for a specific network including MAC Address (BSSID), exact Channel, and Encryption Type (WEP/WPA2/WPA3).
* **⚡ Smart UX:** Implements "Anti-Bounce" button logic and "Turbo Scroll" for smooth navigation on a small OLED screen.
* **💾 Efficient Resource Usage:** Optimized memory management using PROGMEM for graphical assets to ensure system stability.

## 🛠️ Hardware Required

| Component | Quantity | Description |
| :--- | :--- | :--- |
| **ESP32 Development Board** | 1 | Any ESP32 model (WROOM-32 recommended) |
| **0.96" OLED Display** | 1 | SSD1306 Driver, I2C Interface |
| **Tactile Buttons** | 4 | 6x6mm Push buttons |
| **Jumper Wires** | - | M-M and M-F wires |
| **Breadboard / PCB** | 1 | For prototyping or permanent assembly |

## 🔌 Pin Configuration

| Component Pin | ESP32 GPIO | Function |
| :--- | :--- | :--- |
| **OLED SDA** | GPIO 21 | I2C Data |
| **OLED SCL** | GPIO 22 | I2C Clock |
| **Button K1 (UP)** | GPIO 18 | Scroll Up / Navigation |
| **Button K2 (DOWN)** | GPIO 19 | Scroll Down / Navigation |
| **Button K3 (MODE)** | GPIO 5 | Switch to **Graph Mode** |
| **Button K4 (SELECT)** | GPIO 4 | Select / **Detail View** / Rescan |

*> Note: All buttons are configured with internal `INPUT_PULLUP` resistors. No external resistors are needed.*

## 📸 Gallery

Here represents the device operating in different modes:

| **List Mode** | **Graph Mode** | **Detail Mode** |
| :---: | :---: | :---: |
| <img src="https://github.com/KadircanPeker/ESP32-Handheld-WiFi-Analyzer/blob/main/mode_list.jpeg?raw=true" width="250"> | <img src="https://github.com/KadircanPeker/ESP32-Handheld-WiFi-Analyzer/blob/main/mode_graph.jpeg?raw=true" width="250"> | <img src="https://github.com/KadircanPeker/ESP32-Handheld-WiFi-Analyzer/blob/main/mode_detail.jpeg?raw=true" width="250"> |

## 🚀 Installation & Usage

1.  **Clone the Repo:** Download this repository or copy the code.
2.  **Libraries:** Install `Adafruit_GFX` and `Adafruit_SSD1306` via the Arduino IDE Library Manager.
3.  **Upload:** Select your board (e.g., "DOIT ESP32 DEVKIT V1") and upload the `.ino` file.
4.  **Controls:**
    * Use **K1/K2** to navigate the list.
    * Press **K4** to see details of the selected network.
    * Press **K3** to view the Channel Traffic Graph.

## 👨‍💻 Author

**Kadircan Peker**
*Electrical & Electronics Engineering Student*
*Focus: Embedded Systems & IoT*

---
*Disclaimer: This tool is intended for educational and network analysis purposes only.*
