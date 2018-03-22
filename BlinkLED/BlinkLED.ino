
// Target MCU: ATTiny13
// internal OSC: 1.2Mhz
// Created: Mar 22, 2018
// Author: Insoo Kim ( insoo@hotmail.com ) 

#define LED 0
#define HALFSEC 490
#define DURATION (HALFSEC*6)
void setup()
{
  pinMode(LED,OUTPUT);
}

void loop()
{
  digitalWrite(LED, HIGH);
  delay(DURATION);
  digitalWrite(LED, LOW);
  delay(DURATION);
}
  
