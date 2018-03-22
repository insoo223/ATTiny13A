//BlinkLED0_2.ino
// Target MCU: ATTiny13 

#define DURATION 500
void setup()
{
  byte i;
  for (i=0; i<3; i++)
    pinMode(i,OUTPUT);
}

void loop()
{
  byte i;
  for (i=0; i<3; i++)
  {
    digitalWrite(i, HIGH);
    delay(DURATION);
    digitalWrite(i, LOW);
    delay(DURATION);
  }
}
  
