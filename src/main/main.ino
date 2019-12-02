// Libraries
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Constants
#ifndef STASSID
#define STASSID "Network_Cardozo"
#define STAPSK  "Card0z0Ch1l3"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
#define pinPixel      D4
#define numPixels     5
#define pinButton     D8
#define option_limit  5

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

void handleRoot() {
  server.send(200, "text/html", message_output);
}

void handleNotFound() {
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

  server.on("/", handleRoot);

  server.on("/digital", []() {
    server.send(200, "text/html", "<h1>this works as well...</h1>");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP WebServer - nodemcu");
  messagePrint(0, "APAGADO");
}

void loop () {
  ReadButton();
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

void messagePrint(int pos, String status) {
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