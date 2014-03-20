const int pin = 8;

void setup() {
  pinMode(pin, OUTPUT);
}

void loop() {
  static int state = HIGH;
  
  if (state == LOW) {
    state = HIGH;
  } else {
    state = LOW;
  }
  
  digitalWrite(pin, state);
  delay(5000);
}
