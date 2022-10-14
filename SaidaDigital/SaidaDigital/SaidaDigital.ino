#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

const char *ssid = "REPLACE_WITH_YOUR_SSID";
const char *password = "REPLACE_WITH_YOUR_PASSWORD";

AsyncWebServer server(80);

const int ledPin = 2;

String ledState;

// Initialize LitleFS
void initFS()
{
  if (!LittleFS.begin())
  {
    Serial.println("Falha na inicializacao do LitleFS");
  }
  Serial.println("LitleFS iniciado com sucesso");
}

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Ligando WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String processor(const String &var)
{
  if (var == "STATE")
  {
    if (digitalRead(ledPin))
    {
      ledState = "OFF";
    }
    else
    {
      ledState = "ON";
    }
    return ledState;
  }
  return String();
}

void setup()
{
  Serial.begin(115200);
  initWiFi();
  initFS();

  pinMode(2, OUTPUT);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html", false, processor); });

  server.serveStatic("/", LittleFS, "/");

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(ledPin, LOW);
    request->send(LittleFS, "/index.html", "text/html", false, processor); });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    digitalWrite(ledPin, HIGH);
    request->send(LittleFS, "/index.html", "text/html", false, processor); });

  server.begin();
}

void loop()
{
