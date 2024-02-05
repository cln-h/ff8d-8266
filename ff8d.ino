#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiManager.h>
#include <Servo.h>

const char* esp8266_hostname = "ff8d-8266";

ESP8266WebServer server(80);
Servo sm;

void triggerServo() {
  sm.attach(D3);
  sm.write(45);
  delay(1000);
  sm.write(0);
  delay(1000);
  sm.detach();
}

void handleBang() {
  triggerServo();

  server.send(200, "text/plain", "Triggered.");
}

void startConfigurationMode() {
  WiFiManager wifiManager;

  Serial.println("Attempting to start configuration portal.");
  wifiManager.startConfigPortal(esp8266_hostname);
}

void initialize() {
  WiFiManager wifiManager;

  // Attempt to connect to previously saved WiFi credentials, if any exist
  if (!wifiManager.autoConnect(esp8266_hostname)) {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(10000);
  }

  Serial.println("Connected to WiFi");

  if (MDNS.begin(esp8266_hostname)) {
    Serial.println("mDNS responder started");
  }

  delay(2000);

  server.on("/trigger", handleBang);

  server.begin();
  Serial.println("HTTP server started");
}

void setup() {
  Serial.begin(115200); // use common baud rate

  const int configBtnPin = D2;

  pinMode(configBtnPin, INPUT_PULLUP);
  if (digitalRead(configBtnPin) == LOW) {
    Serial.println("Button pressed. Entering configuration mode.");
    startConfigurationMode();
  } else {
    Serial.println("Button not pressed. Continuing setup");
    initialize();
  }
}

void loop() {
  server.handleClient();
}
