#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define RELAY_PIN 5        // GPIO for relay
#define SOIL_PIN 34        // Analog pin for soil sensor
#define TRIG_PIN 26        // Ultrasonic sensor TRIG
#define ECHO_PIN 27        // Ultrasonic sensor ECHO

const char* ssid = "moto g62 5G_7341";
const char* password = "e2bb47d4a778";

int moisture = 0;
bool pumpState = false;
long duration;
int distance = 0;
bool tankEmpty = false;
int dryThreshold = 3500;
int wetThreshold = 2500;
int emptyThreshold = 10;

bool emergencyStopEnabled = false;

AsyncWebServer server(80);

// HTML Page with buttons and dynamic values
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="refresh" content="5">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>JalSaathi - Smart Irrigation</title>
  <style>
    body {
      font-family: 'Segoe UI', sans-serif;
      background: linear-gradient(#a8e063, #56ab2f);
      color: #fff;
      text-align: center;
      padding: 20px;
    }
    .container {
      background-color: rgba(0, 0, 0, 0.3);
      border-radius: 20px;
      padding: 30px;
      max-width: 500px;
      margin: 30px auto;
      box-shadow: 0px 0px 15px rgba(0,0,0,0.3);
    }
    h1 {
      font-size: 28px;
    }
    .status {
      font-size: 22px;
      margin: 15px 0;
    }
    .label {
      font-weight: bold;
    }
    .footer {
      margin-top: 30px;
      font-size: 12px;
    }
    .button {
      padding: 10px 20px;
      font-size: 16px;
      margin: 10px;
      border: none;
      border-radius: 8px;
      cursor: pointer;
    }
    .stop {
      background-color: #e74c3c;
      color: white;
    }
    .resume {
      background-color: #2ecc71;
      color: white;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚰 JalSaathi - स्मार्ट सिंचाई प्रणाली</h1>
    <p class="status"><span class="label">Soil Moisture (मिट्टी की नमी):</span> %MOISTURE%</p>
    <p class="status"><span class="label">Pump Status (पंप स्थिति):</span> %PUMPSTATE%</p>
    <p class="status"><span class="label">Tank Status (पानी की टंकी स्थिति):</span> %TANKSTATUS%</p>
    <p class="status"><span class="label">Emergency Mode:</span> %EMERGENCY%</p>
    <form action="/stop" method="GET">
      <button class="button stop">Emergency Stop</button>
    </form>
    <form action="/resume" method="GET">
      <button class="button resume">Resume Auto</button>
    </form>
    <p class="footer">Page refreshes every 5 seconds • पृष्ठ हर 5 सेकंड में ताज़ा होता है</p>
  </div>
</body>
</html>
)rawliteral";

// Processor for dynamic content
String processor(const String& var) {
  if (var == "MOISTURE") {
    return String(moisture);
  } else if (var == "PUMPSTATE") {
    return pumpState ? "ON (चालू)" : "OFF (बंद)";
  } else if (var == "TANKSTATUS") {
    return tankEmpty ? "Empty (खाली)" : "Full (भरपूर)";
  } else if (var == "EMERGENCY") {
    return emergencyStopEnabled ? "ACTIVE (सक्रिय)" : "INACTIVE (निष्क्रिय)";
  }
  return String();
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // OFF initially
  pinMode(SOIL_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Web endpoints
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    emergencyStopEnabled = true;
    pumpState = false;
    digitalWrite(RELAY_PIN, HIGH);  // OFF
    request->redirect("/");
  });

  server.on("/resume", HTTP_GET, [](AsyncWebServerRequest *request){
    emergencyStopEnabled = false;
    request->redirect("/");
  });

  server.begin();
}

int getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return duration * 0.034 / 2;
}

void loop() {
  moisture = analogRead(SOIL_PIN);
  distance = getDistanceCM();
  tankEmpty = (distance > emptyThreshold);

  Serial.print("Soil Moisture Level: ");
  Serial.println(moisture);
  Serial.print("Water Tank Distance (cm): ");
  Serial.println(distance);

  if (emergencyStopEnabled) {
    pumpState = false;
    Serial.println("🚨 Emergency Stop Activated → Pump OFF");
  } else if (tankEmpty) {
    pumpState = false;
    Serial.println("Water tank empty → Pump OFF (Failsafe)");
  } else {
    if (moisture > dryThreshold) {
      pumpState = true;
    } else if (moisture < wetThreshold) {
      pumpState = false;
    }
  }

  digitalWrite(RELAY_PIN, pumpState ? LOW : HIGH);  // Active LOW Relay

  if (!emergencyStopEnabled) {
    if (pumpState) {
      Serial.println("Soil is dry → Pump ON");
    } else {
      Serial.println("Soil is wet → Pump OFF");
    }
  }

  delay(2000);
}
