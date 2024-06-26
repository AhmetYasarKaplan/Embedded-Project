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
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  float duration = pulseIn(ECHO_PIN, HIGH);
  return (duration * 0.034) / 2;
}

// Function to set up the web server routes
void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = R"(
      <html>
      <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Park Sensor</title>
          <style>
          @import url("https://fonts.googleapis.com/css2?family=Nunito:ital,wght@0,200..1000;1,200..1000&display=swap");
          * {
            font-family: "Nunito", sans-serif;
            font-optical-sizing: auto;
            font-weight: regular;
            font-style: normal;
          }
          body:not(footer) {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            text-align: center;
            padding-top: 50px;
          }
          h1 {
            color: #333;
          }
          p {
            font-size: 24px;
            margin: 10px 0;
          }
          .container {
            display: flex;
            justify-content: center;
            align-items: center;
            margin-top: 50px;
          }
          .bulb-container {
            display: flex;
            flex-direction: column;
            align-items: center;
            margin: 0 20px;
            position: relative;
          }
          #bulb1,
          #bulb2,
          #bulb3 {
            width: 130px;
            height: 130px;
            border-radius: 50%;
            background-color: grey;
            border: 2px solid rgb(61, 61, 61);
            margin-bottom: 0px;
            position: relative;
          }
          button {
            margin-top: -10px;
            padding: 10px 10px;
            background-color: silver;
            font-size: large;
            border: 2px solid gray;
            cursor: pointer;
            border-bottom-left-radius: 15px;
            border-bottom-right-radius: 15px;
            color: silver;
          }
          .btn-end {
            margin-top: 0px;
            height: 11px;
            width: 18px;
            border-bottom-left-radius: 8px;
            border-bottom-right-radius: 8px;
            background-color: rgb(63, 63, 61);
          }
          #data {
            display: inline-block;
            padding: 20px;
            border-radius: 10px;
            background: #fff;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            margin-top: 50px;
          }
          .label {
            font-weight: bold;
            color: #555;
          }
          .value {
            font-size: 28px;
            color: #222;
          }
          footer {
            background-color: silver;
            position: absolute;
            bottom: 0;
            left: 0px;
            width: 100%;
            display: flex;
            align-items: center;
            justify-content: center;
          }
          footer p {
            font-size: 20px;
          }
        </style>
        <script>
          function updateData() {
            var xhr = new XMLHttpRequest();
            xhr.open('GET', '/data', true);
            xhr.onload = function() {
              if (xhr.status == 200) {
                var data = JSON.parse(xhr.responseText);
                document.getElementById('distance').innerText = data.distance + ' cm';
                document.getElementById('led1').innerText = data.led1 ? 'ON' : 'OFF';
                document.getElementById('led2').innerText = data.led2 ? 'ON' : 'OFF';
                document.getElementById('led3').innerText = data.led3 ? 'ON' : 'OFF';
               
                if(data.distance<5 && data.distance > 0){
                  document.getElementById('bulb1').style.backgroundColor = data.led1 ? 'red' : 'grey';
                  document.getElementById('bulb2').style.backgroundColor = data.led2 ? 'red' : 'grey';
                  document.getElementById('bulb3').style.backgroundColor = data.led3 ? 'red' : 'grey';
                }else{
                  document.getElementById('bulb1').style.backgroundColor = data.led1 ? 'yellow' : 'grey';
                  document.getElementById('bulb2').style.backgroundColor = data.led2 ? 'yellow' : 'grey';
                  document.getElementById('bulb3').style.backgroundColor = data.led3 ? 'yellow' : 'grey';
                }
              }
            };
            xhr.send();
          }
          setInterval(updateData, 200); // Update data every 200 ms
        </script>
      </head>
      <body>
        <h1>Park Sensor</h1>
        <div id="data">
          <p>
            <span class="label">Distance:</span>
            <span id="distance" class="value"></span>
          </p>
          <div class="container">
            <div class="bulb-container">
              <div id="bulb1"></div>
              <button>ON</button>
              <div class="btn-end"></div>
              <p>
                <span class="label">LED1:</span>
                <span id="led1" class="value"></span>
              </p>
            </div>
            <div class="bulb-container">
              <div id="bulb2"></div>
              <button>ON</button>
              <div class="btn-end"></div>
              <p>
                <span class="label">LED2:</span>
                <span id="led2" class="value"></span>
              </p>
            </div>
            <div class="bulb-container">
              <div id="bulb3"></div>
              <button>ON</button>
              <div class="btn-end"></div>
              <p>
                <span class="label">LED3:</span>
                <span id="led3" class="value"></span>
              </p>
            </div>
          </div>
        </div>
        <footer>
          <p>Contributors: Ahmet Yaşar Kaplan, Abdullah Öztoprak, Baran Parmak</p>
        </footer>
      </body>
      </html>
    )";
    request->send(200, "text/html", html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    int distance = getDistance();
    String json = "{";
    json += "\"distance\":" + String(distance) + ",";
    json += "\"led1\":" + String(digitalRead(LED1_PIN)) + ",";
    json += "\"led2\":" + String(digitalRead(LED2_PIN)) + ",";
    json += "\"led3\":" + String(digitalRead(LED3_PIN));
    json += "}";
    request->send(200, "application/json", json);
  });

  server.begin(); // Start the server
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

  setupServer();
}

void loop() {
  int distance = getDistance();

  if (0 < distance && distance < 5) {
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
