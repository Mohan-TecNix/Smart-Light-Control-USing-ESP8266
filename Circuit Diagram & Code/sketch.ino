#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

ESP8266WebServer server(80);

#define PIR_PIN D5
#define LED_PIN D2

bool autoMode = true;
bool ledState = false;

void handleRoot() {

  String statusText;

  if (autoMode) {
    if (digitalRead(PIR_PIN))
      statusText = "AUTO MODE - Motion Detected";
    else
      statusText = "AUTO MODE - No Motion";
  } else {
    statusText = ledState ? "MANUAL MODE - LED ON"
                          : "MANUAL MODE - LED OFF";
  }

  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>Smart Light Control</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{
font-family:Arial;
text-align:center;
margin-top:40px;
}
button{
padding:15px 30px;
font-size:18px;
margin:10px;
}
</style>
</head>
<body>

<h1>Smart Light Control</h1>
)rawliteral";

  page += "<h2>" + statusText + "</h2>";

  page += "<a href='/mode'><button>Toggle AUTO/MANUAL</button></a><br>";

  if (!autoMode) {
    page += "<a href='/on'><button>LED ON</button></a>";
    page += "<a href='/off'><button>LED OFF</button></a>";
  }

  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleMode() {
  autoMode = !autoMode;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleOn() {
  ledState = true;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleOff() {
  ledState = false;
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void setup() {

  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/mode", handleMode);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();

  Serial.println("Server Started");
}

void loop() {

  server.handleClient();

  if (autoMode) {

    int motion = digitalRead(PIR_PIN);

    if (motion == HIGH) {
      digitalWrite(LED_PIN, HIGH);
    }
    else {
      digitalWrite(LED_PIN, LOW);
    }
  }
  else {

    if (ledState)
      digitalWrite(LED_PIN, HIGH);
    else
      digitalWrite(LED_PIN, LOW);
  }
}