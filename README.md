# ESP8266 Smart Matrix & Relay Controller

[![ESP8266](https://img.shields.io/badge/Board-ESP8266%20NodeMCU-blue.svg)](https://www.espressif.com/en/products/socs/esp8266)
[![Framework](https://img.shields.io/badge/Framework-Arduino%20IDE-orange.svg)](https://www.arduino.cc/)

🌐 **[English](#english-version)** | 🇮🇩 **[Bahasa Indonesia](#versi-indonesia)**

---

## English Version

An IoT project utilizing an ESP8266 (NodeMCU) to run a web server that controls a MAX7219 LED Matrix (using the `MD_Parola` library) and a 4-channel relay module. Users can change the scrolling text, adjust animation speed and effects, and toggle relays directly from a web browser.

### ✨ Features
* **Web-based Dashboard:** Responsive UI using Bootstrap 4.
* **LED Matrix Control:** * Update text in real-time.
  * Adjust animation speed.
  * Change animation effects (Scroll, Blink, Fade, etc.).
  * Toggle animation ON/OFF.
* **Relay Control:** 4 independent buttons to toggle 4 relay channels.
* **Network Status:** Dynamic display of connection status.

### 🛠️ Hardware Requirements
* NodeMCU ESP8266
* MAX7219 LED Matrix Module (FC-16 Hardware)
* 4-Channel Relay Module
* Jumper Wires

### 📍 Pin Configuration

| Component | NodeMCU Pin |
| :--- | :--- |
| **LED Matrix CLK** | D5 |
| **LED Matrix DATA/DIN** | D7 |
| **LED Matrix CS** | D8 |
| **Relay 1** | D1 |
| **Relay 2** | D2 |
| **Relay 3** | D3 |
| **Relay 4** | D4 |

### 🚀 How to Use
1. Install the required libraries in Arduino IDE: `MD_Parola`, `MD_MAX72XX`.
2. Open the code and modify the WiFi credentials:
   ```cpp
   const char *ssid = "your_SSID";
   const char *password = "your_PASSWORD";
