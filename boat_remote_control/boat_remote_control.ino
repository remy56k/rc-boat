/*
 * This ESP8266 NodeMCU code was developed by newbiely.com
 *
 * This ESP8266 NodeMCU code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/esp8266/esp8266-controls-car-via-web
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// https://github.com/Links2004/arduinoWebSockets/tree/master
#include <WebSocketsServer.h>
#include <Servo.h>
#include "index.h"

///////////////////// PARAMETERS //////////////////////

String WIFIMODE = "AP"; // Or AP or EXISTING

const char* ssid = "BATEAU-4";
const char* password = "12345678";

/////////// END, NOT TOUCH UNDER THIS LINE ////////////

Servo myservo;

#define CMD_STOP 0
#define CMD_FORWARD 1
#define CMD_BACKWARD 2
#define CMD_LEFT 4
#define CMD_RIGHT 8


#define A_PWM_PIN D1  // A MOTOR
#define A_DIR_PIN D3  // A MOTOR

#define B_PWM_PIN D2  // B MOTOR
#define B_DIR_PIN D4  // B MOTOR


#define GOUVERNAIL_PIN D5 // PIN SERVO GOUVERNAIL
int GOUVERNAIL_VALUE = 90; // PIN SERVO GOUVERNAIL

ESP8266WebServer server(80);                        // Web server on port 80
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81


  // Broadcast Wifi strength
  // String rssi = String(WiFi.RSSI());
  // webSocket.broadcastTXT(rssi);
  
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
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
      Serial.printf("[%u] Received text: %s\n", num, payload); // Debug all raw messages
      String angle = String((char*)payload);
      int command = angle.toInt();
      Serial.print("command: ");
      Serial.println(command);

      switch (command) {
        case CMD_STOP:
          Serial.println("Stop");
          CAR_stop();
          break;
        case CMD_FORWARD:
          Serial.println("Move Forward");
          CAR_moveForward();
          break;
        case CMD_BACKWARD:
          Serial.println("Move Backward");
          CAR_moveBackward();
          break;
        case CMD_LEFT:
          Serial.println("Turn Left");
          CAR_turnLeft();
          break;
        case CMD_RIGHT:
          Serial.println("Turn Right");
          CAR_turnRight();
          break;
        default:
          Serial.println("Unknown command");
      }

      break;
  }
}

void setup() {
  Serial.begin(9600);
  // A MOTOR
  pinMode(A_PWM_PIN, OUTPUT);
  pinMode(A_DIR_PIN, OUTPUT);
  // B MOTOR
  pinMode(B_PWM_PIN, OUTPUT);
  pinMode(B_DIR_PIN, OUTPUT);
  // GOUVERNAIL
  pinMode(GOUVERNAIL_PIN, OUTPUT);
  // Boot init
  Boot_init();

if (WIFIMODE == "EXISTING") {
// Connect to existing Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP8266 Web Server's IP address: ");
  Serial.println(WiFi.localIP());
} else {
  // Configure ESP8266 as an Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");

  // IP address assigned to the Access Point
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Serve a basic HTML page with JavaScript to create the WebSocket connection
  server.on("/", HTTP_GET, []() {
    Serial.println("Web Server: received a web page request");
    String html = HTML_CONTENT;  // Use the HTML content from the servo_html.h file
    server.send(200, "text/html", html);
  });

  server.begin();
  Serial.print("ESP8266 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  // SERVO
  myservo.attach(GOUVERNAIL_PIN);

}

void loop() {
  // Handle client requests
  server.handleClient();

  // Handle WebSocket events
  webSocket.loop();
}

void CAR_moveForward() {
  // A MOTOR
  digitalWrite(A_PWM_PIN, 500);
  digitalWrite(A_DIR_PIN, HIGH);
  // B MOTOR
  digitalWrite(B_PWM_PIN, 500);
  digitalWrite(B_DIR_PIN, HIGH);
}

void CAR_moveBackward() {
  // A MOTOR
  digitalWrite(A_PWM_PIN, 500);
  digitalWrite(A_DIR_PIN, LOW);
  // B MOTOR
  digitalWrite(B_PWM_PIN, 500);
  digitalWrite(B_DIR_PIN, LOW);
}

void CAR_turnLeft() {
  GOUVERNAIL_VALUE += 100;
  myservo.write(GOUVERNAIL_VALUE);
}

void CAR_turnRight() {
  GOUVERNAIL_VALUE -= 100;
  myservo.write(GOUVERNAIL_VALUE);
}

void CAR_stop() {
  GOUVERNAIL_VALUE = 90;
  myservo.write(90);
  digitalWrite(A_PWM_PIN, 0);
  digitalWrite(A_DIR_PIN, LOW);
  digitalWrite(B_PWM_PIN, 0);
  digitalWrite(B_DIR_PIN, LOW);
}

void Boot_init() {
  digitalWrite(A_PWM_PIN, 0);
  digitalWrite(A_DIR_PIN, LOW);
  digitalWrite(B_PWM_PIN, 0);
  digitalWrite(B_DIR_PIN, LOW);
}
