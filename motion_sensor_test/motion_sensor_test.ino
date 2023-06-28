#define motion_pin 23

void setup() {
  Serial.begin(115200);
  pinMode(motion_pin, INPUT_PULLUP);
  // put your setup code here, to run once:
  
}

void loop() {
//   put your main code here, to run repeatedly:
  if(digitalRead(motion_pin) == HIGH){
    Serial.println("Motion Detected");
    delay(1000);
  }
//  Serial.println(digitalRead(motion_pin));
//  delay(100);
}
