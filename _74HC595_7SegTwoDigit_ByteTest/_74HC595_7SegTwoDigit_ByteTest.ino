//**************************************************************//
//  Name    : _74HC595_7SegTwoDigit_ByteTest.ino                                
//  Target MCU: ATTiny13
//  Author  : Insoo Kim
//  Ref: Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : March 17, 2015    
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : Driving two 7 segment LEDs
//****************************************************************

//Duration between numbers
#define DURATION 2000
//Pin connected to ST_CP of 74HC595
byte latchPin = 0;
//Pin connected to SH_CP of 74HC595
byte clockPin = 1;
////Pin connected to DS of 74HC595
byte dataPin = 2;

byte OnesDigit[16] = {
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
		0b10011110 //9                
	};//TensDigit[10]

byte OutputPin555 = 2;
byte Output555State=0;
byte prevState=0;
unsigned int clockCnt = 0;
unsigned int minCnt = 0;
unsigned int hourCnt = 0;

//function prototypes
void showCnt(byte);
void DPon();
void DPoff();

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(OutputPin555, INPUT);
}//setup

void loop() 
{
  byte i;
  for (i=0; i<100; i++)
  {
    showCnt(i);
    delay(DURATION);        // delay in between reads for stability
  }
}//loop

void showCnt(byte num)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[num % 10]);
  shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[num / 10]); 
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//showCnt

void DPon()
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[minCnt % 10] | OnesDigit[10]);
  shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[minCnt / 10]); 
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//DPon

void DPoff()
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[minCnt % 10] | OnesDigit[11]);
  shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[minCnt / 10]); 
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//DPoff

