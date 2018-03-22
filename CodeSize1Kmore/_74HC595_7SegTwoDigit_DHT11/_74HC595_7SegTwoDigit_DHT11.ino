//**************************************************************//
// Target MCU: ATtiny13 
// Size: 1678 byte, Cannot hold on ATTiny13
//  Name    : _74HC595_7SegTwoDigit_DHT11.ino                                
//  Author  : Insoo Kim
//  Ref: Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : March 16, 2015    
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : Driving two 7 segment LEDs
//          : Showing the current temparature and humidity read from DHT11
//****************************************************************

#include <DHT_SimpleLib.h>
#define dht_dpin 2 //no ; here. Set equal to channel sensor is on

dht DHT;

//Duration between numbers
#define DURATION 4000
//Pin connected to ST_CP of 74HC595
int latchPin = 0;
//Pin connected to SH_CP of 74HC595
int clockPin = 1;
////Pin connected to DS of 74HC595
int dataPin = 2;

byte OnesDigit[15] = {
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
		0b00000001, //DP (10)
		0b00000000 //Blank (11)
	};//OnesDigit[10]

byte TensDigit[15] = {
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
		0b00000001, //DP (10)
		0b00000000 //Blank (11)
	};//TensDigit[10]

void sensDHT11();
void showTwoDigit7seg(int, boolean);

void setup() {
  pinMode(dht_dpin, INPUT);
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {

  sensDHT11();
  
  showTwoDigit7seg((int)DHT.temperature, true);
  delay(DURATION);

  showTwoDigit7seg((int)DHT.humidity, false);
  delay(DURATION);
}//loop

//----------------------------------------
void showTwoDigit7seg(int num, boolean DP)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  if (DP)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[num % 10] | OnesDigit[10]);
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[num / 10] );
  }
  else
  {
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[num % 10]);
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[num / 10]); 
  }
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//showTwoDigit7seg

//----------------------------------------
void sensDHT11()
{
  DHT.read11(dht_dpin);
}//sensDHT11

