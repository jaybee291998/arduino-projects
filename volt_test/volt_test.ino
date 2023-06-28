#define volt_input A0
float volts;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(volt_input, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  float volts = map(analogRead(volt_input), 0, 1023, 0, 5.0);
  Serial.print("Volts: ");
  Serial.println(volts);
  delay(100);
}
