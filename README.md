# Contactless-Temperature-Screening-Kiosk
An ESP32-based automatic gate kiosk. An HC-SR04 ultrasonic sensor detects a person within 30 cm and opens an SG90 servo gate, while a DHT11 sensor measures temperature and humidity. The ESP32 uploads the readings to a ThingSpeak channel over Wi-Fi so they can be monitored live on a dashboard.
