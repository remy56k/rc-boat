#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "ESP8266_AP"; // SSID du point d'accès Wi-Fi
const char* password = "password"; // Mot de passe du point d'accès Wi-Fi

WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Received message: %s\n", num, payload);
      // Vous pouvez traiter les données reçues ici, par exemple, les stocker dans une variable ou les afficher
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.softAP(ssid, password);
  Serial.println("Server IP address:");
  Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("Server started");
}

void loop() {
  webSocket.loop();
}
