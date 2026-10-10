# Contactless Screening Kiosk – Automatic Gate Monitoring

**Industrial IoT and Industry 4.0 (22UECH64 / M64) – Assessment 2 Hardware Prototype**
Dr. N.G.P. Institute of Technology, Coimbatore – Department of ECE (2026-27, odd sem)

**Team 12 – Project 29**
- Kishore Kumar K (710724121028)
- Niranjan A (710724121037)

---

## Overview
An ESP32-based automatic gate kiosk. An HC-SR04 ultrasonic sensor detects a person within 30 cm and opens an SG90 servo gate, while a DHT11 sensor measures temperature and humidity. The ESP32 uploads the readings to a **ThingSpeak** channel over Wi-Fi so they can be monitored live on a dashboard.

> **Note:** the DHT11 measures *air* temperature (0–50 °C, ±2 °C). This prototype is a screening/monitoring demonstrator, not a medical thermometer. For real contactless body-temperature measurement, replace it with an infrared sensor such as the MLX90614.

## Hardware
| Component | Purpose |
|---|---|
| ESP32 development board (ESP32 Dev Module) | Controller with built-in Wi-Fi |
| DHT11 module | Temperature and humidity |
| HC-SR04 ultrasonic sensor | Person / distance detection |
| SG90 micro servo | Gate (90° = open, 0° = closed) |
| USB cable, jumper wires | Power and connections |

## Pin connections
| Device pin | ESP32 pin |
|---|---|
| DHT11 DATA | GPIO 26 |
| HC-SR04 TRIG | GPIO 5 |
| HC-SR04 ECHO | GPIO 18 |
| SG90 signal | GPIO 19 |
| DHT11 VCC | 3V3 |
| HC-SR04 VCC, SG90 +5V | VIN (5 V) |
| All GND | GND |

> The HC-SR04 ECHO output is 5 V; a 1 kΩ / 2 kΩ voltage divider to GPIO 18 is recommended to protect the ESP32.

## How it works
1. The ESP32 connects to Wi-Fi.
2. It measures distance with the HC-SR04 and reads the DHT11.
3. If a person is closer than **30 cm**, the servo moves to 90° (gate open) and returns to 0° (closed) afterwards.
4. Temperature, humidity, distance and gate status are printed on the Serial Monitor and uploaded to ThingSpeak.

## ThingSpeak channel
| Field | Data |
|---|---|
| Field 1 | Temperature (°C) |
| Field 2 | Humidity (%) |
| Field 3 | Distance (cm) |
| Field 4 | Gate status (0 = closed, 1 = open) |

Protocol: HTTP (REST) using the ThingSpeak Arduino library. Minimum update interval on the free plan: 15 s.

## Software setup
1. Install the **Arduino IDE** and add ESP32 board support (Espressif ESP32 package). Select **ESP32 Dev Module**.
2. Install these libraries from the Library Manager:
   - ThingSpeak
   - DHT sensor library (Adafruit) and Adafruit Unified Sensor
   - ESP32Servo
3. Open `contactless_tempreature_monitoring.ino` and replace the placeholders with your own values:
   ```cpp
   const char* WIFI_NAME     = "YOUR_WIFI_NAME";
   const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
   unsigned long CHANNEL_ID  = 0000000;            // your ThingSpeak channel ID
   const char* WRITE_API_KEY = "YOUR_WRITE_API_KEY";
   ```
4. Connect the ESP32 by USB, choose the COM port, click **Upload**.
5. Open the Serial Monitor at **115200 baud**. You should see temperature, humidity and distance values, "Gate OPEN / CLOSED" messages and "ThingSpeak upload SUCCESS!".

## Sample Serial Monitor output
```
Temp: 29.1 C | Humidity: 62.9 % | Distance: 13.9 cm
Gate OPEN
Temp: 29.1 C | Humidity: 63.5 % | Distance: -1.0 cm
Gate CLOSED
ThingSpeak upload SUCCESS!
```
(`-1.0 cm` means no echo was received, i.e. nothing within range.)

## Security
Never commit your real Wi-Fi password or ThingSpeak Write API key. The code in this repository uses placeholders only.

## Possible improvements
- Replace the DHT11 with an MLX90614 infrared sensor for true contactless body-temperature screening.
- Add a ThingSpeak React/e-mail alert for high temperature.
- Add an OLED display and buzzer for local feedback.
