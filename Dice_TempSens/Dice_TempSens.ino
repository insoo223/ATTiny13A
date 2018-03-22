//**************************************************************//
//  Name    : _74HC595_7SegTwoDigit.ino    
// Target MCU: ATTiny13
// Code size > 1K, so that fail to load at ATTiny13
//  Author  : Insoo Kim
//  Ref: Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : March 17, 2015    
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : Driving two 7 segment LEDs
//          : Showing the current temparature read from LM335
//****************************************************************
//#include <math.h>

//Duration between numbers
#define DURATION 4000

//Pin connected to ST_CP of 74HC595
#define latchPin 4
//Pin connected to SH_CP of 74HC595
#define clockPin 3
//Pin connected to DS of 74HC595
#define dataPin 2

#define sensPin A0

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(sensPin, INPUT);
}

unsigned int currTemp;

void loop() {
  
  //currTemp = sensTemp(); 
  for (currTemp=0; currTemp<128; currTemp++)
  {
    dispTempOnLED(currTemp);
    delay(2000);
  }
  //dispTempOnLED(234);
  // pause before next value:
  delay(1000);
}//loop

byte sensTemp()
{
  byte rawvoltage;
  /*
  float millivolts;
  float kelvin;
  float celsius;
  byte byteCel;
  */
  rawvoltage = analogRead(sensPin)/10;
  /*
  millivolts = (rawvoltage/1023.0) * 5000;
  kelvin = (millivolts/10);
  
  celsius= kelvin - 273.15;
  //byteCel = (byte)round(celsius);
  byteCel = (byte)round(celsius);
  */
  return rawvoltage;

}//sensTemp

void  dispTempOnLED(byte num)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, num<<1);
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}
