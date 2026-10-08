#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ===== PIN SETTING =====
#define SOIL_PIN 34
#define RELAY_PIN 26
#define PIR_PIN 27
#define BUZZER_PIN 25

// ===== WIFI =====
const char* ssid = "vivo 1938";
const char* password = "123456789";

WebServer server(80);

// ===== VARIABLE =====
int soilValue = 0;
int motion = 0;

// ===== CONTROL MANUAL =====
bool manualPump = false;

// ===== HALAMAN WEB =====
void handleRoot() {
  soilValue = analogRead(SOIL_PIN);

  String html = "<h1>Smart Tanaman</h1>";
  html += "<p>Nilai Soil: " + String(soilValue) + "</p>";

  html += "<p><a href='/on'><button>Pompa ON</button></a></p>";
  html += "<p><a href='/off'><button>Pompa OFF</button></a></p>";

  server.send(200, "text/html", html);
}

void handleOn() {
  manualPump = true;
  digitalWrite(RELAY_PIN, LOW);
  server.send(200, "text/html", "Pompa ON");
}

void handleOff() {
  manualPump = false;
  digitalWrite(RELAY_PIN, HIGH);
  server.send(200, "text/html", "Pompa OFF");
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);

  // ===== WIFI CONNECT =====
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  // ===== WEB ROUTES =====
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();

  Serial.println("Sistem Monitoring Tanaman Siap!");
}

void loop() {

  server.handleClient(); // WAJIB untuk web

  // =============================
  // 🌱 BACA SENSOR TANAH
  // =============================
  soilValue = analogRead(SOIL_PIN);

  Serial.print("Nilai Soil: ");
  Serial.println(soilValue);

  // =============================
  // 💧 LOGIKA PENYIRAMAN
  // =============================
  if (!manualPump) { // AUTO hanya jika tidak manual
    if (soilValue > 3000) {
      Serial.println("Status: KERING → SIRAM");
      digitalWrite(RELAY_PIN, LOW);
    } 
    else if (soilValue > 1500) {
      Serial.println("Status: LEMBAB");
      digitalWrite(RELAY_PIN, HIGH);
    } 
    else {
      Serial.println("Status: BASAH");
      digitalWrite(RELAY_PIN, HIGH);
    }
  }

  // =============================
  // 🐛 DETEKSI HAMA
  // =============================
  motion = digitalRead(PIR_PIN);

  if (motion == HIGH) {
    Serial.println("⚠️ HAMA TERDETEKSI!");
    tone(BUZZER_PIN, 1000);
  } else {
    noTone(BUZZER_PIN);
  }

  Serial.println("----------------------");

  delay(500); // dipercepat biar web tidak lag
}