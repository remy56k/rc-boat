#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "ESP8266_AP"; // SSID du point d'accès Wi-Fi
const char* password = "password"; // Mot de passe du point d'accès Wi-Fi
const char* webSocketHost = "192.168.4.1"; // IP du serveur WebSocket

WebSocketsClient webSocket;
MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(100);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi");

  webSocket.begin(webSocketHost);

  Wire.begin();
  mpu.initialize();
}

void loop() {
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  // Créer le message à envoyer au format JSON avec les valeurs lues
  String message = "{\"acceleration\": {\"x\": " + String(ax) + ", \"y\": " + String(ay) + ", \"z\": " + String(az) + "}, \"rotation\": {\"x\": " + String(gx) + ", \"y\": " + String(gy) + ", \"z\": " + String(gz) + "}}";

  // Envoyer le message au serveur WebSocket
  webSocket.sendTXT(message);

  webSocket.loop(); // Important pour gérer les événements du client WebSocket
  delay(100); // Attendre un court instant entre chaque envoi
}
