
#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ===== Wi-Fi Credentials =====
const char* WIFI_NAME = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ===== ThingSpeak Credentials =====
unsigned long CHANNEL_ID = 0000000;  // Replace with your ThingSpeak Channel ID
const char* WRITE_API_KEY = "YOUR_WRITE_API_KEY";

// ===== ESP32 Pins =====
#define DHT_PIN 26
#define DHT_TYPE DHT11
#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 19

DHT dht(DHT_PIN, DHT_TYPE);
Servo gateServo;
WiFiClient client;

// ===== Gate Settings =====
const float DETECT_DISTANCE = 30.0;
const int CLOSED_ANGLE = 0;
const int OPEN_ANGLE = 90;

bool gateIsOpen = false;
unsigned long gateOpenedAt = 0;
unsigned long lastUpload = 0;
unsigned long lastRead = 0;

float temperatureC = 0;
float humidityPct = 0;
float distanceCm = -1;

// ===== Read Ultrasonic Distance =====
float readDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  return duration * 0.0343 / 2.0;
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  dht.begin();

  gateServo.setPeriodHertz(50);
  gateServo.attach(SERVO_PIN, 500, 2400);
  gateServo.write(CLOSED_ANGLE);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi...");

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed!");
  }

  ThingSpeak.begin(client);
}

void loop() {
  // Read sensors every 2 seconds
  if (millis() - lastRead >= 2000) {
    lastRead = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();
    float d = readDistanceCM();

    if (!isnan(t)) temperatureC = t;
    if (!isnan(h)) humidityPct = h;

    distanceCm = d;

    Serial.printf(
      "Temp: %.1f C | Humidity: %.1f %% | Distance: %.1f cm\n",
      temperatureC, humidityPct, distanceCm
    );

    // Open gate when an object is detected
    if (d > 0 && d <= DETECT_DISTANCE && !gateIsOpen) {
      gateServo.write(OPEN_ANGLE);
      gateIsOpen = true;
      gateOpenedAt = millis();
      Serial.println("Gate OPEN");
    }
  }

  // Close gate after 3 seconds
  if (gateIsOpen && millis() - gateOpenedAt >= 3000) {
    gateServo.write(CLOSED_ANGLE);
    gateIsOpen = false;
    Serial.println("Gate CLOSED");
  }

  // Upload data every 20 seconds
  if (millis() - lastUpload >= 20000) {
    lastUpload = millis();

    if (WiFi.status() == WL_CONNECTED) {
      ThingSpeak.setField(1, temperatureC);
      ThingSpeak.setField(2, humidityPct);
      ThingSpeak.setField(3, distanceCm);
      ThingSpeak.setField(4, gateIsOpen ? 1 : 0);

      int response = ThingSpeak.writeFields(
        CHANNEL_ID, WRITE_API_KEY
      );

      if (response == 200) {
        Serial.println("ThingSpeak upload SUCCESS!");
      } else {
        Serial.printf(
          "ThingSpeak upload FAILED. HTTP code: %d\n",
          response
        );
      }
    } else {
      Serial.println("Wi-Fi disconnected. Reconnecting...");
      WiFi.reconnect();
    }
  }
}