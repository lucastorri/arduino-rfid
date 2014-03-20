int analogPin = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int v = analogRead(analogPin); // 0 <-> 1023
  Serial.println(v);
}
