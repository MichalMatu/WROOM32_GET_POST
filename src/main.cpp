#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "ESP32-AP";
const char *password = "password";
WebServer server(80);

String value = "Initial Value"; // The value to be updated

void handleRoot()
{
  server.send(200, "text/html", "<html><body><div id='value'>" + value + "</div><button onclick='updateValue()'>Update Value</button><script>function updateValue() { var newValue = prompt('Enter new value:'); if (newValue !== null) { sendData(newValue); } } function sendData(value) { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { document.getElementById('value').innerHTML = this.responseText; } }; xhttp.open('POST', '/update', true); xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded'); xhttp.send('value=' + value); }</script></body></html>");
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

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);

  server.begin();
}

void loop()
{
  server.handleClient();
}
