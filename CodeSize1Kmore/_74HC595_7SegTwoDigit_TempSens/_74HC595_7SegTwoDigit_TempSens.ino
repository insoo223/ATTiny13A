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

byte OnesDigit[11] = {
		0b01111110, //0
		0b00000110, //1
		0b10111100, //2
		0b10101110, //3
		0b11000110, //4
		0b11101010, //5
		0b11011010, //6
		0b00100110, //7
		0b11111110, //8
		0b11100110, //9                
		0b00000000 //Blank
	};//OnesDigit[10]

byte TensDigit[11] = {
		0b11111010, //0
		0b10010000, //1
		0b01111100, //2
		0b11011100, //3
		0b10010110, //4
		0b11001110, //5
		0b11100110, //6
		0b10011000, //7
		0b11111110, //8
		0b10011110, //9                
		0b00000000 //Blank
	};//TensDigit[10]

unsigned int sensTemp();

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

unsigned int currTemp;

void loop() {
  
  currTemp = sensTemp(); 

  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[currTemp % 10]);
  shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[currTemp / 10]); 
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);

  // pause before next value:
  delay(1000);
}//loop

unsigned int sensTemp()
{
  unsigned int rawvoltage;
  /*
  float millivolts;
  float kelvin;
  float celsius;
  byte byteCel;
  */
  rawvoltage = analogRead(A0);
  /*
  millivolts = (rawvoltage/1023.0) * 5000;
  kelvin = (millivolts/10);
  
  celsius= kelvin - 273.15;
  //byteCel = (byte)round(celsius);
  byteCel = (byte)round(celsius);
  */
  return rawvoltage/10;

}//sensTemp

