# 🚰 JalSaathi - Smart Irrigation System (AgriTech Hack 2025)

*JalSaathi* is a low-cost, IoT-based smart irrigation system designed to automate and optimize water usage in agriculture. Built using an ESP32 microcontroller and a set of simple yet powerful sensors, the system helps smallholder farmers reduce water wastage and increase efficiency — all for under ₹900.

🏅 2nd Runner Up – *AgriTech Hack 2025, hosted at **Microsoft Sovereign Office, Noida, in collaboration with **Azure Developer Community* and *Reskilll*.

---

## 🌟 Features

- 💧 Automatic irrigation based on soil moisture levels
- 🚨 Failsafe shutdown when water tank is empty (ultrasonic detection)
- 🌐 Real-time web dashboard with *bilingual support (Hindi + English)*
- 🛑 Manual Emergency Stop & Resume buttons via web interface
- 🔋 Power-efficient and field-deployable
- 💸 Built under ₹900, making it affordable for rural India

---

## 🛠 Hardware Components Used

| Component              | Description                                       |
|------------------------|---------------------------------------------------|
| ESP32                  | Main microcontroller with built-in Wi-Fi         |
| Soil Moisture Sensor   | Detects moisture level in soil                   |
| Ultrasonic Sensor HC-SR04 | Measures distance to monitor water tank level    |
| 5V Relay Module        | Switches the water pump based on ESP32 signals   |
| Submersible Water Pump | Waters the crops based on moisture logic         |
| Power Supply           | Can be run via USB or battery pack               |

---

## 💻 Software & Tech Stack

- *Programming Language*: C++ (Arduino)
- *IDE*: Arduino IDE
- *Web Framework*: ESPAsyncWebServer (served from ESP32)
- *Web UI*: HTML + CSS (Hindi & English)
- *Serial Monitoring*: via Serial Monitor (115200 baud)

---

## 🔁 Code Logic Summary

- Read soil moisture and water tank level every 2 seconds
- If soil is dry and tank has water, turn pump ON
- If soil is wet or tank is empty → pump OFF
- Emergency Stop manually disables pump via web interface
- Dashboard refreshes every 5 seconds to show:
  - Soil Moisture
  - Pump Status
  - Tank Level
  - Emergency Mode

---

## 🚀 Setup Instructions

1. Open the code in *Arduino IDE*
2. Install the following libraries:
   - WiFi.h
   - AsyncTCP.h
   - ESPAsyncWebServer.h
3. Set your Wi-Fi credentials in the code:
   ```cpp
   const char* ssid = "Your_WiFi_Name";
   const char* password = "Your_WiFi_Password";
4.Connect the ESP32 to your laptop and upload the code
5.Open Serial Monitor (115200 baud) to view debug logs
6.Access the web dashboard via the IP shown in the Serial Monitor
