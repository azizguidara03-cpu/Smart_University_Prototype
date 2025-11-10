// === ESP32 Smart Faculty IoT Project ===
// Capteurs : MQ2, Ultrasonic, PIR + Servo, LCD, RGB LED, RFID, Supabase

#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>

// === Wi-Fi credentials ===
const char* ssid = "Orange-1FC9";
const char* password = "6BY5q5RYB2h";

// === Supabase ===
const String baseURL = "https://wkvkynmbnqycxnkfdvip.supabase.co/rest/v1/";
const String supabaseAPIKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Indrdmt5bm1ibnF5Y3hua2ZkdmlwIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDMyMzE4OTUsImV4cCI6MjA1ODgwNzg5NX0.kj95zrdFOJaBRZoJ4SBRcv2TDfC5rQeNliaCubsM6Sk"; 

LiquidCrystal_PCF8574 lcd(0x27);

// === Capteurs ===
const int mq2Pin = 34;
const int trigPin = 2;   // modifié
const int echoPin = 15;  // modifié
const int pirPin = 33;

// === Actionneurs ===
const int buzzerPin = 17;
const int redPin = 14;
const int greenPin = 27;
const int bluePin = 26;

Servo doorServo;
const int servoPin = 12;

Servo motionServo;
const int servo2Pin = 25;

// === RFID MFRC522 ===
#define SS_PIN 5
#define RST_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

// === États ===
String lastStateDoor = "";
String lastLCDState = "";
String lastLCDMessage = "";

bool motionActive = false;
unsigned long lastMotionTime = 0;
const unsigned long motionDelay = 5000;

unsigned long lastSensorSend = 0;
const unsigned long sensorInterval = 1000;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(mq2Pin, INPUT);
  pinMode(pirPin, INPUT);

  doorServo.attach(servoPin);
  doorServo.write(0);

  motionServo.attach(servo2Pin);
  motionServo.write(0);

  Wire.begin(21, 22); // SDA, SCL I2C
  lcd.begin(16, 2);
  lcd.clear(); lcd.setBacklight(1);
  lcd.setCursor(1, 1); lcd.print("Ahla bik");

  SPI.begin(); // SPI matériel
  mfrc522.PCD_Init();
  Serial.println("RFID ready");
  initTime();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi perdu, reconnexion...");
    WiFi.begin(ssid, password);
    delay(1000);
    return;
  }

  if (millis() - lastSensorSend > sensorInterval) {
    sendGasData();
    checkUltrasonic();
    lastSensorSend = millis();
  }

  handleActuator("12", lcdControl);
  handleActuator("4", lightControl);
  handleActuator("7", BuzzerControl);

  handleMotionControl();
  handleRFID();
}

void initTime() {
  configTime(3600, 0, "pool.ntp.org", "time.nist.gov"); // UTC+1 pour Tunis
  struct tm timeinfo;
  int retry = 0;
  while (!getLocalTime(&timeinfo) && retry < 10) {
    Serial.println("Échec de l'obtention de l'heure via NTP");
    delay(1000);
    retry++;
  }
  if (retry < 10) {
    Serial.println("Heure NTP synchronisée !");
  } else {
    Serial.println("Erreur critique : NTP échoué après 10 essais");
  }
}
// === GAS Sensor ===
void sendGasData() {
  int gasValue = analogRead(mq2Pin);
  int id = 8;
  Serial.println("Gas Value: " + String(gasValue));
  postToSupabase("sensor_readings", "{\"sensor_id\":" + String(id) + ",\"value\":" + String(gasValue) + "}");
}

// === Ultrasonic ===
void checkUltrasonic() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);

  // Get the duration of the pulse from the echo pin
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance (in cm)
  int distance = duration * 0.034 / 2;
  
  // Print the distance to the Serial Monitor for debugging
  Serial.println("Distance: " + String(distance));

  // Prepare data to send to Supabase
  String sensorId = "5"; // If your sensor ID is fixed as 5
  String body = "{\"sensor_id\":\"" + sensorId + "\",\"value\":" + String(distance) + "}";

  // Send the data to Supabase
  postToSupabase("sensor_readings", body);
}


// === PIR + Servo Intelligent ===
void handleMotionControl() {
  int motion = digitalRead(pirPin);
  if (motion == HIGH) {
    if (!motionActive) {
      Serial.println(">> Mouvement détecté !");
      motionServo.write(90);
      motionActive = true;
    }
    lastMotionTime = millis();
  }

  if (motionActive && millis() - lastMotionTime > motionDelay) {
    Serial.println(">> Plus de mouvement, fermeture.");
    motionServo.write(0);
    motionActive = false;
  }
}

// === RGB LED ===
void setRGB(bool r, bool g, bool b) {
  digitalWrite(redPin, r);
  digitalWrite(greenPin, g);
  digitalWrite(bluePin, b);
}

// === RFID ===
void handleRFID() {
  

  // Vérifie s’il y a une carte à lire
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    // Aucun tag détecté
    return;
  }
  Serial.println("[INFO] Appel de handleRFID()...");
  // Lire l’UID de la carte
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uid += "0"; // Ajoute un 0 devant si nécessaire
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase(); // Met en majuscule
  Serial.println("[INFO] UID détecté : " + uid);

  // === Étape 1 : Vérification dans Supabase ===
  HTTPClient http;
  String url = baseURL + "rfid_cards?rfid_uid=eq." + uid + "&select=student_id";
  http.begin(url);
  http.addHeader("apikey", supabaseAPIKey);
  http.addHeader("Authorization", "Bearer " + supabaseAPIKey);

  int httpCode = http.GET();
  Serial.println("[HTTP] Code de réponse : " + String(httpCode));

  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("[HTTP] Réponse JSON : " + payload);

    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error && doc.size() > 0) {
      int studentId = doc[0]["student_id"];
      Serial.println("[INFO] Étudiant trouvé : ID = " + String(studentId));

      // Étape 2 : Obtenir la date du jour
      struct tm timeinfo;
      if (getLocalTime(&timeinfo)) {
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);

        // Étape 3 : Enregistrement de la présence
        String jsonData = "{\"student_id\":" + String(studentId) +
                          ",\"attendance_date\":\"" + String(dateStr) + "\"}";

        postToSupabase("daily_attendance", jsonData);
        Serial.println("[INFO] Donnée envoyée : " + jsonData);

        doorServo.write(90);  // Ouvre la porte
        delay(5000);
        doorServo.write(0);    // Ferme la porte

      } else {
        Serial.println("[ERREUR] Impossible d'obtenir l'heure via NTP !");
      }

    } else {
      Serial.println("[ERREUR] UID non reconnu dans la base !");
    }

  } else {
    Serial.println("[ERREUR] Échec de la requête Supabase. Code HTTP : " + String(httpCode));
  }

  http.end();

  // Fin de la communication avec la carte
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}








// === Gestion Actuateurs via Supabase ===
void handleActuator(String id, void (*action)(String, String)) {
  HTTPClient http;
  String url = baseURL + "actuators?actuator_id=eq." + id + "&select=current_state,settings&order=updated_at.desc&limit=1";

  http.begin(url);
  http.addHeader("apikey", supabaseAPIKey);
  http.addHeader("Authorization", "Bearer " + supabaseAPIKey);

  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error && doc.is<JsonArray>()) {
      JsonObject lastObj = doc[0];
      String state = lastObj["current_state"] | "off";
      String message = "";
      if (lastObj.containsKey("settings") && lastObj["settings"].is<JsonObject>()) {
        message = lastObj["settings"]["message"] | "";
      }
      action(state, message);
    } else {
      Serial.println("Erreur JSON");
    }
  } else {
    Serial.println("Erreur HTTP : " + String(httpCode));
  }
  http.end();
}



// === Contrôle Lumière RGB ===
void lightControl(String state, String _) {
  int repetitions = 0;
  if (state == "on") {
    while(repetitions<=2){
      setRGB(true, false, false);  // Rouge
      delay(250);
      setRGB(false, true, false);  // Vert
      delay(250);
      setRGB(false, false, true);  // Bleu
      delay(250);
      setRGB(false, false, false);
      delay(250);
      repetitions++;
      }
  } else if (state == "off") {
    setRGB(false, false, false); // Éteindre toutes les couleurs
  }
}



// === Contrôle LCD ===
void lcdControl(String state, String message) {
  if (state == "on") {
    lcd.setBacklight(1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message.length() > 0 ? message : "LCD actif");
  } else {
    lcd.clear();
    lcd.setBacklight(0);
  }
}

// === Contrôle Buzzer ===
int melody[] = {
  1000, 1000, 1200, 1000, 1200, 1000, // Repetitive beeps
  1500, 0,    1500, 0,    1500, 0,    // Pause and beep
  1000, 1200, 1000, 1200, 1500, 1000, // Rapid cycle
  2000, 0,    2000, 0,    2000        // Loud final burst
};

int noteDurations[] = {
  250, 250, 500, 500, 500, 1000,
  250, 250, 500, 500, 500, 1000,
  250, 250, 500, 500, 500, 500, 1000,
  250, 250, 500, 500, 500, 1000
};


void BuzzerControl(String state, String _) {
  if (state == "on") {
    for (int i = 0; i < sizeof(melody) / sizeof(int); i++) {
      int noteDuration = noteDurations[i];

      if (melody[i] != 0) {
        tone(buzzerPin, melody[i], noteDuration);
      }

      // wait for the note to finish + a small gap between notes
      delay(noteDuration * 1.3); 

      // stop the tone to avoid overlap
      noTone(buzzerPin);
    }
  } else {
    noTone(buzzerPin);
  }
}



// === Envoi JSON à Supabase ===
void postToSupabase(String table, String jsonPayload) {
  HTTPClient http;
  String url = baseURL + table;

  http.begin(url);
  http.addHeader("apikey", supabaseAPIKey);
  http.addHeader("Authorization", "Bearer " + supabaseAPIKey);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(jsonPayload);
  Serial.println("POST [" + table + "] Code: " + String(httpCode));
  if (httpCode >= 200 && httpCode < 300) {
    Serial.println("✅ Donnée acceptée !");
  } else {
    Serial.println("❌ Réponse : " + http.getString());
  }

  http.end();
}

