// Pin definitions
const int currentPin = A2;
const int battery1Pin = A0;
const int battery2Pin = A1;

void setup() {
  Serial.begin(9600); // Initialize serial communication
}

float k1 = (5.0 / 1024.0) * 7.5;
float k2 = (5.0 / 1024.0) * 15.0;
float ck1 = (5.0 / 1024.0) * 10.0;

void loop() {
  Serial.print(analogRead(battery1Pin) * k1);
  Serial.print(",");
  Serial.print(analogRead(battery2Pin) * k2);
  Serial.print(",");
  Serial.println(analogRead(currentPin) * ck1);
  delay(200);
}
