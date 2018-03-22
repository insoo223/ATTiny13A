
// Target MCU: ATTiny13 

#define buzzPin 4

void setup()
{
  pinMode(buzzPin,OUTPUT);
}

void loop()
{
  const byte DURATION = 200;
  
  digitalWrite(buzzPin, HIGH);
  delay(DURATION);
  digitalWrite(buzzPin, LOW);
  delay(DURATION);
}
  
