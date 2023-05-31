#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

#include <Preferences.h>
Preferences preferences;
// WiFi AP credentials
char ap_ssid[32] = "ESP32-AP";
char ap_password[64] = "password";

// WiFi Station credentials
char sta_ssid[32] = "esp32";
char sta_password[64] = "0123456789";

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

void handleAPCredentials()
{
  // send ap ssid, password and ip to client
  server.send(200, "text/plain", String(ap_ssid) + " " + String(ap_password) + " " + WiFi.softAPIP().toString());
}

void handleSTACredentials()
{
  // send sta ssid, password and ip to client
  server.send(200, "text/plain", String(sta_ssid) + " " + String(sta_password) + " " + WiFi.localIP().toString());
}

void handleAPCredentialsSet()
{
  // get ap ssid and password from client
  if (server.hasArg("ap_ssid") && server.hasArg("ap_password"))
  {
    String new_ap_ssid = server.arg("ap_ssid");
    String new_ap_password = server.arg("ap_password");

    // save new ap ssid and password to preferences
    preferences.putString("ap_ssid", new_ap_ssid);
    preferences.putString("ap_password", new_ap_password);

    // Update the in-memory variables
    strncpy(ap_ssid, new_ap_ssid.c_str(), sizeof(ap_ssid));
    strncpy(ap_password, new_ap_password.c_str(), sizeof(ap_password));

    // send new ap ssid and password to client
    server.send(200, "text/plain", new_ap_ssid + " " + new_ap_password);
    // disconnect all clients to update ap ssid and password, then restart ap
    WiFi.softAPdisconnect(true);
    WiFi.softAP(new_ap_ssid.c_str(), new_ap_password.c_str());
  }
  else
  {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleSTACredentialsSet()
{
  // get sta ssid and password from client
  if (server.hasArg("sta_ssid") && server.hasArg("sta_password"))
  {
    String new_sta_ssid = server.arg("sta_ssid");
    String new_sta_password = server.arg("sta_password");

    // save new sta ssid and password to preferences
    preferences.putString("sta_ssid", new_sta_ssid);
    preferences.putString("sta_password", new_sta_password);

    // Update the in-memory variables
    strncpy(sta_ssid, new_sta_ssid.c_str(), sizeof(sta_ssid));
    strncpy(sta_password, new_sta_password.c_str(), sizeof(sta_password));

    // send new sta ssid and password to client
    server.send(200, "text/plain", new_sta_ssid + " " + new_sta_password);
    // disconnect from current wifi network
    WiFi.disconnect(true);
    // reconnect to wifi network with new sta ssid and password

    WiFi.mode(WIFI_AP_STA);            // Enable both AP and STA modes
    WiFi.softAP(ap_ssid, ap_password); // Start AP
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    WiFi.begin(new_sta_ssid.c_str(), new_sta_password.c_str());

    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 8000) // 2 seconds timeout
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
  else
  {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup()
{
  Serial.begin(115200);

  preferences.begin("myApp", false);
  value = preferences.getInt("value", 1000);

  String pref_ap_ssid = preferences.getString("ap_ssid", "");
  String pref_ap_password = preferences.getString("ap_password", "");

  if (pref_ap_ssid.length() > 0 && pref_ap_password.length() > 0)
  {
    pref_ap_ssid.toCharArray(ap_ssid, sizeof(ap_ssid));
    pref_ap_password.toCharArray(ap_password, sizeof(ap_password));
  }

  String pref_sta_ssid = preferences.getString("sta_ssid", "");
  String pref_sta_password = preferences.getString("sta_password", "");

  if (pref_sta_ssid.length() > 0 && pref_sta_password.length() > 0)
  {
    pref_sta_ssid.toCharArray(sta_ssid, sizeof(sta_ssid));
    pref_sta_password.toCharArray(sta_password, sizeof(sta_password));
  }

  WiFi.mode(WIFI_AP_STA);            // Enable both AP and STA modes
  WiFi.softAP(ap_ssid, ap_password); // Start AP
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  WiFi.begin(sta_ssid, sta_password); // Connect to WiFi network

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 2000) // 2 seconds timeout
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

  if (!SPIFFS.begin(true))
  {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.on("/getValue", handleGetValue);
  server.on("/AP_credentials", handleAPCredentials);
  server.on("/STA_credentials", handleSTACredentials);
  server.on("/AP_CREDENTIALS_SET", handleAPCredentialsSet);
  server.on("/STA_CREDENTIALS_SET", handleSTACredentialsSet);

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
