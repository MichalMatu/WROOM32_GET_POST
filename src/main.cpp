#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>

// #########################################################################################################################
AsyncWebServer server(80);
WebSocketsServer webSocket(81);
// #########################################################################################################################
// WiFi AP/Station credentials
char ap_ssid[32] = "ESP32-AP";
char ap_password[64] = "password";
char sta_ssid[32] = "esp32";
char sta_password[64] = "0123456789";
// #########################################################################################################################
const int analogPin = 36;
int sensorValue = 0;
// #########################################################################################################################
void setupWiFi()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  WiFi.begin(sta_ssid, sta_password);
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 4000)
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
// #########################################################################################################################
void setupSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error occurred while mounting SPIFFS");
    // Handle the error here, such as returning from the function or taking appropriate action
    return;
  }
}
// #########################################################################################################################
void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  // Handle WebSocket events here, if needed
}
// #########################################################################################################################
void setup()
{
  Serial.begin(115200);
  // --------------------------------------------------------------------------
  setupSPIFFS();
  setupWiFi();
  // --------------------------------------------------------------------------
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.begin();
  // --------------------------------------------------------------------------
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
}
// #########################################################################################################################
void loop()
{
  sensorValue = analogRead(analogPin); // Update the sensor value dynamically (replace with actual sensor reading)

  String sensorData = String(sensorValue); // Create a named String object

  webSocket.broadcastTXT(sensorData); // Broadcast the sensor data to all connected clients

  delay(1000);
}
