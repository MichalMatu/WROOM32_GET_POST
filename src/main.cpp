#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>

Preferences preferences;
// WiFi AP credentials
char ap_ssid[32] = "ESP32-AP";
char ap_password[64] = "password";
// WiFi Station credentials
char sta_ssid[32] = "esp32";
char sta_password[64] = "0123456789";

AsyncWebServer server(80);

void setupWiFi()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  WiFi.begin(sta_ssid, sta_password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 8000)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.print("Connected to WiFi, IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Failed to connect to WiFi");
  }
}

void setupSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error occurred while mounting SPIFFS");
    // Handle the error here, such as returning from the function or taking appropriate action
    return;
  }
}

void setup()
{
  Serial.begin(115200);
  preferences.begin("myApp", false);

  setupSPIFFS();
  setupWiFi();

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.begin();
}

void loop()
{
  // Your code for other tasks, if any
}
