#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "../config.h" 


// Pins for ultrasonic sensor
#define TRIGGER_PIN 5
#define ECHO_PIN 18

// Pins for LEDs and Buzzer
#define LED1_PIN 4
#define LED2_PIN 2
#define LED3_PIN 15
#define BUZZER_PIN 32

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

int getDistance() {
  // Implement the logic to measure the distance using the ultrasonic sensor
  // and return the distance in centimeters
  // For example:
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(TRIGGER_PIN, LOW);
  float duration = pulseIn(ECHO_PIN, HIGH);
  return (duration * 0.034) / 2;
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Initialize digital pins as outputs
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize ultrasonic sensor pins
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><meta http-equiv='refresh' content='1'></head><body>";
    html += "<h1>Park Sensor</h1>";
    html += "<p>Distance: " + String(getDistance()) + " cm</p>";
    html += "<p>LED1: " + String(digitalRead(LED1_PIN)) + "</p>";
    html += "<p>LED2: " + String(digitalRead(LED2_PIN)) + "</p>";
    html += "<p>LED3: " + String(digitalRead(LED3_PIN)) + "</p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Start server
  server.begin();
}

void loop() {
  int distance = getDistance();

  if(0 < distance &&  distance < 5){
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    tone(BUZZER_PIN, 1000);
    delay(200);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    delay(200);
  } else if (distance < 10) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    tone(BUZZER_PIN, 1000); // Play tone at 1000 Hz
    delay(150);
    noTone(BUZZER_PIN);
    delay(150);
  } else if (distance < 20) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    tone(BUZZER_PIN, 1000);
    delay(300);
    noTone(BUZZER_PIN); // Stop the tone
    delay(300);
  } else if (distance < 30) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    tone(BUZZER_PIN, 1000);
    delay(500);
    noTone(BUZZER_PIN); // Stop the tone
    delay(500);
  } else {
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    noTone(BUZZER_PIN); // Stop the tone
    delay(1000);
  }
}