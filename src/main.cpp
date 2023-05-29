#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

const char *ssid = "ESP32-AP";
const char *password = "password";
WebServer server(80);

String value = "Initial Value"; // The value to be updated

void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  if (file)
  {
    server.streamFile(file, "text/html");
    file.close();
  }
  else
  {
    server.send(404, "text/plain", "File not found");
  }
}

void handleUpdate()
{
  if (server.hasArg("value"))
  {
    value = server.arg("value");
    server.send(200, "text/plain", value);
  }
  else
  {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup()
{
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);

  server.begin();
}

void loop()
{
  server.handleClient();
}
