#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>

// ########################################################################################
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// ########################################################################################
// WiFi AP/Station credentials
char ap_ssid[32] = "ESP32-AP";
char ap_password[64] = "password";
char sta_ssid[32] = "2.4G-vnet-5071C8";
char sta_password[64] = "vnet249216582";
// ########################################################################################
const int analogPin = 36;
int sensorValue = 0;

bool ledState = 0;
const int ledPin = 2;
// ########################################################################################
void setupSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error occurred while mounting SPIFFS");
    // Handle the error here, such as returning from the function or taking appropriate action
    return;
  }
}
// ########################################################################################
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
// ########################################################################################
// Send message to all connected clients
void notifyClients()
{
  ws.textAll(String(ledState));
}
// ########################################################################################
// Receive message from client and send it back
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0)
    {
      ledState = !ledState;
      notifyClients();
    }
  }
}
// ########################################################################################
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    notifyClients();
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}
// ########################################################################################
void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
// ########################################################################################
void setup()
{
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  // --------------------------------------------------------------------------
  setupSPIFFS();
  setupWiFi();

  // --------------------------------------------------------------------------
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  initWebSocket();
  server.begin();

  // --------------------------------------------------------------------------
}
// ########################################################################################
void loop()
{
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}
