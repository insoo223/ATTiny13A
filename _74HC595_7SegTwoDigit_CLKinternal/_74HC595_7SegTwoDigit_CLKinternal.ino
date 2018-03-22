//**************************************************************//
// Target MCU: ATTiny13
//  Name    : _74HC595_7SegTwoDigit_CLKinternal.ino                                
//  Author  : Insoo Kim
//  Ref: Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : March 18, 2015    
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : Driving two 7 segment LEDs
//****************************************************************

//Duration between numbers
#define DURATION 2900

byte startPin = 3;
//Pin connected to ST_CP of 74HC595
byte latchPin = 0;
//Pin connected to SH_CP of 74HC595
byte clockPin = 1;
////Pin connected to DS of 74HC595
byte dataPin = 2;

byte OnesDigit[12] = {
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
	};//OnesDigit[12]

byte TensDigit[12] = {
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
	};//TensDigit[12]

boolean start = false;

byte clockCnt;
byte secCnt;
byte minCnt;
byte hourCnt;

void showCnt(byte, boolean);
void chkStart();

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(startPin, INPUT);
}//setup

void loop() 
{
  byte num2show;

  chkStart();
  if (start)
  {
    clockCnt++;
    
    if (clockCnt % 2 == 0)   
      secCnt++;
  
    if (secCnt == 60)
    {
      minCnt++;
      secCnt = 0;
      clockCnt = 0;
      //showCnt(minCnt);
    }
  
    //add one HOUR every 60 minutes
    if ( minCnt == 60 )
    {
      hourCnt++;
      minCnt = 0;
    }
    
    //blink DP every second
    if (clockCnt % 2 == 0)   
    {
      if (hourCnt == 0)
        //num2show = minCnt;
        num2show = secCnt;
      else
        num2show = minCnt;
      showCnt(num2show, false);
      //DPoff(num2show);
    }
    else
    {
      if (hourCnt == 0)
        num2show = minCnt;
        //num2show = hourCnt;
      else
        num2show = hourCnt;
      showCnt(num2show, true);
      //DPon(num2show);
    }
    delay(DURATION);        // delay in between reads for stability
  }//if (start)
}//loop

//-----------------------------------------
void showCnt(byte num, boolean DP)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  if (!DP)
  {
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[num % 10]);
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[num / 10]); 
  }
  else
  {
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[num % 10] | OnesDigit[10]);
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[num / 10] ); 
  }
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//showCnt

void chkStart()
{
  if (digitalRead(startPin) == LOW)
  {
    start = !start;

    clockCnt = 0;
    secCnt = 0;
    minCnt = 0;
    hourCnt = 0;
  }
}//chkStart


