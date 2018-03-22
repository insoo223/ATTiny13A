
// Target MCU: ATTiny13 

#define LED 0
#define SW 2
#define DURATION 370
void setup()
{
  pinMode(LED,OUTPUT);
  pinMode(SW,INPUT);
}

void loop()
{
  if (digitalRead(SW) == LOW)
  {
    digitalWrite(LED, HIGH);
    //delay(DURATION);
  }
  else
  {
    digitalWrite(LED, LOW);
    //delay(DURATION);
  }
}
  
