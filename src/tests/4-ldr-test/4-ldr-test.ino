
void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(34));
  
  delay(100);
}
