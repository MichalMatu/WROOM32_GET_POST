#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <Preferences.h>

Preferences preferences;

const char *ssid = "ESP32-AP";
const char *password = "password";
WebServer server(80);

int value = 1000; // The value to be updated
unsigned long previousMillis = 0;

void handleRoot()
{
  File file = SPIFFS.open("/index.html", "r");
  if (file)
  {
    String html = file.readString();
    file.close();
    html.replace("{value_placeholder}", String(value));
    server.send(200, "text/html", html);
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
    value = server.arg("value").toInt();
    server.send(200, "text/plain", String(value));

    // Save the value to preferences
    preferences.putInt("value", value);
  }
  else
  {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleGetValue()
{
  server.send(200, "text/plain", String(value));
}

void setup()
{
  Serial.begin(115200);

  preferences.begin("myApp", false);         // Specify a namespace for your preferences
  value = preferences.getInt("value", 1000); // Load the value from preferences or use the default value (1000 in this case)

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
  server.on("/getValue", handleGetValue);

  server.begin();
}

void loop()
{
  server.handleClient();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= value)
  {
    previousMillis = currentMillis;
    // print in console the current int value and its type
    Serial.print("value: ");
    Serial.print(value);
    Serial.print(" type: ");
  }
}
