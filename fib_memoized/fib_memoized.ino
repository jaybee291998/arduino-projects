#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte SIZE = 46;
long mem[SIZE];
void setup()
{
  Serial.begin(115200);
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  long result;
  lcd.backlight();
  for(int i = 0; i < 46; i++){
    result = fib_m(i);
    printResult(i, result);
  }
  printMem();
  delay(10000);
  for(int i = 0; i < 46; i++){
    result = fib(i);
    printResult(i, result);
  }
}

long fib_m(byte n){
  if(n >= SIZE) return -1;
  if(n <= 1) return 1;
  if(mem[n] != 0) return mem[n];
  long nth_term = fib_m(n - 1) + fib_m(n - 2);  // calculate the nth fibonacci number
  mem[n] = nth_term;  // save the calculated nth_term
  return nth_term;
}



void printMem(){
  for(byte i = 0; i < SIZE; i++){
    Serial.print("mem[");
    Serial.print(i);
    Serial.print(") = ");
    Serial.println(mem[i]);
  }
}

void printResult(byte n, long result){
  lcd.clear();
  lcd.print("fib(");
  lcd.print(n);
  lcd.print(") = ");
  lcd.setCursor(0,1);
  lcd.print(result);
}

long fib(byte n){
  if(n <= 1) return 1;
  return fib(n - 1) + fib(n - 2);
}

void loop()
{
  // Do nothing here...
}
