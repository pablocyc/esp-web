// Constants
#define pinLed      LED_BUILTIN
#define pinButton   D8
#define option_limit  5

// Variables
String option_message = "Opcion: ";
bool option_array[option_limit];
int option_position;
unsigned long current_time;
unsigned long double_click_time = 700;
bool press_active;

void setup () {
  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT);
  Serial.begin(9600);
  Serial.println("WebServer - nodemcu");
}

void loop () {
  ReadButton();
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
  if (value)
    Serial.println(String(pos) + ": ON");
  else
    Serial.println(String(pos) + ": OFF");
}

void showOptions () {
  option_position++;
  if (option_position > option_limit - 1)
    option_position = 0;
  Serial.println(option_message + String(option_position));
}