#define pinLed      LED_BUILTIN
#define pinButton   D3

void setup () {
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);
  Serial.begin(9600);
  Serial.println("WebServer - nodemcu");
}

void loop () {
  if (!digitalRead(pinButton)) {
    int value = digitalRead(pinLed);
    digitalWrite(pinLed, !value);
    Serial.print("LED: ");
    if (value)
      Serial.println("ON");
    else
      Serial.println("OFF");
    
    while(!digitalRead(pinButton));
  }
}