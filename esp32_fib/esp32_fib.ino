void setup() {
  // put your setup code here, to run once:
  byte n = 10;
  long result = 0;
  Serial.begin(115200);
  for(byte i = 0; i < 46; i++){
    Serial.print("f(");
    Serial.print(i);
    Serial.print(") = ");
    result = fib(i);
    Serial.println(result);
  }
}

long fib(byte n){
  if(n <= 1) return 1;
  return fib(n - 1) + fib(n - 2);
}

void loop() {
  // put your main code here, to run repeatedly:

}
