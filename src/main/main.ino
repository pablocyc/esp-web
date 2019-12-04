// Libraries
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

// Constants
#ifndef STASSID
#define STASSID "Network_Cardozo"
#define STAPSK  "Card0z0Ch1l3"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
#define option_limit  5
#define numPixels     5
// PINOUT
#define pinPixel      D4
#define pinButton     D8
#define LED_RED       D5
#define LED_GREEN     D6
#define LED_BLUE      D7

// Variables
String option_message = "Opcion: ";
bool option_array[option_limit];
int option_position;
unsigned long current_time;
unsigned long double_click_time = 700;
bool press_active;
String message_output;

// Init
Adafruit_NeoPixel pixels(numPixels, pinPixel, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Libraries functions
void handleRoot () {
  String path = "/index.html";
  if (SPIFFS.exists(path)) {
    Serial.println("SPIFFS exist!");
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, "text/html");
    file.close();
  }
  else 
    Serial.println("no exist SPIFFS");
}

void handleCss () {
  String path = "/styles.css";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, "text/css");
    file.close();
  }
  else
    Serial.println("styles.css not exist!");
}

void handleJs () {
  String path = "/index.js";
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, "text/js");
    file.close();
  }
  else
    Serial.println("index.js not exist!");
}

void handleNotFound () {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void webSocketEvent (uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

      // send message to client
      webSocket.sendTXT(num, "Connected");
    }
    break;
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);

      if(payload[0] == '#') {
        uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

        analogWrite(LED_RED,    ((rgb >> 16) & 0xFF));
        analogWrite(LED_GREEN,  ((rgb >> 8) & 0xFF));
        analogWrite(LED_BLUE,   ((rgb >> 0) & 0xFF));
      }
      break;
  }

}


void setup () {
  pinMode(pinButton, INPUT);
  pixels.begin();
  Serial.begin(9600);
  pixels.clear();
  pixels.show();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // start webSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  server.on("/", handleRoot);
  server.on("/styles.css", handleCss);
  server.on("/index.js", handleJs);
  server.on("/digital", []() {
    server.send(200, "text/html", "<h1>this works as well...</h1>");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP WebServer - nodemcu");
  messagePrint(0, "APAGADO");

  //Initialize File System
  if(SPIFFS.begin())
    Serial.println("SPIFFS Initialize....ok");
  else
    Serial.println("SPIFFS Initialization...failed");
   
}

void loop () {
  ReadButton();
  webSocket.loop();
  server.handleClient();
  MDNS.update();
}

void ReadButton () {
  if (digitalRead(pinButton)) {
    while(digitalRead(pinButton));

    if (!press_active) {
      current_time = millis();
      press_active = true;
    }
    else {
      ledToggle(option_position);
      press_active = false;
    }
  }
  
  if (millis() > current_time + double_click_time) {
    if (press_active)
      showOptions();
    press_active = false;
  }
}

void ledToggle (int pos) {
  option_array[pos] = !option_array[pos];
  int value = option_array[pos];
  if (value) {
    messagePrint(pos, "ENCENDIDO");
    Serial.println(String(pos) + ": ON");
    pixels.setPixelColor(pos, pixels.Color(170, 0, 255));
    pixels.show();
  }
  else {
    messagePrint(pos, "APAGADO");
    Serial.println(String(pos) + ": OFF");
    pixels.setPixelColor(pos, pixels.Color(0, 0, 0));
    pixels.show();
  }
}

void messagePrint (int pos, String status) {
  message_output = "<h1>LEDs Status</h1>";
  message_output += "<ul>";
  message_output += "<li>LED " + String(pos) + ": " + status + "</li>";
  message_output += "</ul>";
}

void showOptions () {
  option_position++;
  if (option_position > option_limit - 1)
    option_position = 0;
  Serial.println(option_message + String(option_position));
}