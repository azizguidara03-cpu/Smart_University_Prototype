#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Servo.h>
#include "DHT.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

// WiFi credentials
const char* ssid = "Orange-1FC9";
const char* password = "6BY5q5RYB2h";

// Supabase config
const char* supabaseURL = "https://wkvkynmbnqycxnkfdvip.supabase.co/rest/v1/";
const char* supabaseAPIKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Indrdmt5bm1ibnF5Y3hua2ZkdmlwIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDMyMzE4OTUsImV4cCI6MjA1ODgwNzg5NX0.kj95zrdFOJaBRZoJ4SBRcv2TDfC5rQeNliaCubsM6Sk";

// Pins
#define DHTPIN D1
#define DHTTYPE DHT11
#define RELAY_PIN D2
#define FAN_ACTUATOR_ID 3
#define LDR_PIN A0
#define LED_PIN D3
#define LED_ACTUATOR_ID 8
#define SERVO_PIN D4
#define WINDOW_ACTUATOR_ID 1

// Sensor IDs
#define TEMP_SENSOR_ID 2
#define LDR_SENSOR_ID 4

DHT dht(DHTPIN, DHTTYPE);
Servo servo;

void setup() {
  Serial.begin(115200);
  dht.begin();
  servo.attach(SERVO_PIN);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi Connected");
  servo.write(0);
}

void loop() {
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  
  if (now - lastUpdate >= 10000) { // Update every 10 seconds
    lastUpdate = now;
    
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      delay(1000);
      return;
    }

    // Read and post sensor data
    float temperature = dht.readTemperature();
    if (!isnan(temperature)) {
      postSensorReading(TEMP_SENSOR_ID, String(temperature, 2));
    }
    postSensorReading(LDR_SENSOR_ID, String(analogRead(LDR_PIN)));

    // Update actuators
    updateActuatorState(FAN_ACTUATOR_ID, checkFanState);
    updateActuatorState(LED_ACTUATOR_ID, checkLEDState);
    updateActuatorState(WINDOW_ACTUATOR_ID, checkWindowState);
  }
}

void postSensorReading(int sensorId, String value) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  String jsonData = "{\"sensor_id\":" + String(sensorId) + ",\"value\":" + value + "}";
  
  if (http.begin(client, String(supabaseURL) + "sensor_readings")) {
    http.addHeader("apikey", supabaseAPIKey);
    http.addHeader("Authorization", "Bearer " + String(supabaseAPIKey));
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Prefer", "return=minimal");
    
    int httpCode = http.POST(jsonData);
    if (httpCode != HTTP_CODE_OK && httpCode != HTTP_CODE_CREATED) {
      Serial.println("Sensor post failed: " + String(httpCode));
    }else{
       Serial.println("Sensor post OK: " + String(httpCode));
    }
    http.end();
  }
}

void updateActuatorState(int actuatorId, void (*action)(String, String)) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  if (http.begin(client, String(supabaseURL) + "actuators?actuator_id=eq." + String(actuatorId) + "&select=current_state,settings")) {
    http.addHeader("apikey", supabaseAPIKey);
    http.addHeader("Authorization", "Bearer " + String(supabaseAPIKey));
    
    if (http.GET() == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(512);
      if (!deserializeJson(doc, payload) && doc.is<JsonArray>() && doc.size() > 0) {
        JsonObject obj = doc[0];
        String state = obj["current_state"].as<String>();
        String message = obj["settings"]["message"] | "";
        action(state, message);
      }
    }
    http.end();
  }
}

void checkFanState(String state, String message) {
  digitalWrite(RELAY_PIN, state == "on" ? HIGH : LOW);
}

void checkLEDState(String state, String message) {
  digitalWrite(LED_PIN, state == "on" ? HIGH : LOW);
}

void checkWindowState(String state, String message) {
  servo.write(state == "on" ? 90 : 0);
}