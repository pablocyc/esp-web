#define pinLed      LED_BUILTIN
#define pinButton   D3

void setup () {
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);
}

void loop () {
  if (!digitalRead(pinButton)) {
    digitalWrite(pinLed, !digitalRead(pinLed));
    while(!digitalRead(pinButton));
  }
}