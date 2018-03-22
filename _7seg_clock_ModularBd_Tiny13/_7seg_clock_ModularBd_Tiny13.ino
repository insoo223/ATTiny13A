/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : _7seg_clock_ModularBd_Tiny13.ino
  Author  : Insoo Kim (insoo@hotmail.com)
  Date    : April 05, 2015    

  Description: Turn on/off LEDs responding RF remote control keys
  
  Ref:
  ** RC315Mhz_Decode.ino
  ** Test of RF decoder using SC2272-M4 chip
  **
  ** Written for and tested with Arduino Uno R3
  ** This example refered from the following link of Edward Comer.
  ** https://dzrmo.wordpress.com/2012/07/08/remote-control-pt2272-for-android/
  ** LICENSE: GNU General Public License, version 3 (GPL-3.0)
*****************************************************************/  
//Duration between numbers
#define DURATION 370
#define halfSec 370

#define BLANK 255

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

//PB# of ATtiny13
byte latchPin = 2; //2;
byte clockPin = 1;
byte dataPin = 0; 
byte debug = 4; 
//byte LED = 0; //4
byte startPin = 3;

boolean sysBegin=false; 
byte clockCnt;
byte secCnt;
byte minCnt;
byte hourCnt;
     

//---------------------------------
void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, OUTPUT);

//  pinMode(LED, OUTPUT);
  pinMode(startPin, INPUT);

} // setup
     
//---------------------------------
void loop()
{
  /*
  if (digitalRead(startPin) == LOW)
  {
    sysBegin = true;
//    digitalWrite(LED, HIGH);
//    delay(halfSec*2);
  }
  else
  {
//    digitalWrite(LED, LOW);
//    delay(halfSec);
  }
  //delay(halfSec/4);
  if (sysBegin)  
*/    goClock();
} // loop

//---------------------------------
void  goClock()
{
  byte num2show;
  
  clockCnt++;
  
  if (clockCnt % 2 == 0)   
  {
    secCnt++;
    //digitalWrite(debug, HIGH);
  }
  else
    //digitalWrite(debug, LOW);
  

  if (secCnt == 60)
  {
    minCnt++;
    secCnt = 0;
    //clockCnt = 0;
    //showCnt(minCnt);
  }

  //add one HOUR every 60 minutes
  if ( minCnt == 60 )
  {
    hourCnt++;
    minCnt = 0;
    //secCnt = 0;
    //clockCnt = 0;
  }
  
  //blink DP every second
  if (clockCnt % 2 == 0)   
  {
    if (hourCnt == 0)
      //num2show = minCnt;
      num2show = secCnt % 60;
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
}//goClock

//---------------------------------
void chkTemperature()
{
  int rawV;
  int C;

  // pin3 (PB4, ADC2)
  // if connected to pin1 (PB5, RESET), you'll be in trouble 
  rawV = analogRead(2);
  //C = rawV * 0.48828125;
  // To fit into 1K program space of ATtiny13
  // we sacrifice some precision by using * 1/2 in stead of * 0.48828125.
  // Experiments show its measurement of temperature is +/- 1 Celcius degree
  
  //C = rawV >> 1;
  C = lookupTable(rawV >> 1);
  //showTemp7seg(C, false);  
  //showTempDiceLED(C);
  //delay(halfSecond*2); // 1 seconds

/*
  showTempDiceLED(rawV/10);
  delay(halfSecond*2); // 1 seconds
  showTempDiceLED(rawV%10);
  delay(halfSecond*2); // 1 seconds
*/

  //showTempDiceLED(0);
  //showTemp7seg(BLANK, false);
  //delay(halfSecond*8); // 2 seconds
  
}//chkTemperature

//---------------------------------
byte lookupTable (int tmp)
{
  if (tmp <= 24) 
    return tmp - 1;  
  else if (tmp <= 34) 
    return tmp - 2;  
  else if (tmp <= 35) 
    return tmp - 3;  
  else if (tmp <= 42)
   if (tmp % 2 == 0) 
    return tmp - 2;  
   else
    return tmp - 3;  
  else if (tmp <= 51) 
    return tmp - 3;  
  else if (tmp <= 59)
   if (tmp % 2 == 0) 
    return tmp - 3;  
   else
    return tmp - 4;  
  else if (tmp <= 68) 
    return tmp - 4;  
}//lookupTable

//---------------------------------
void showTemp7seg(byte num, boolean DP)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  if (num != BLANK)
  {
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
  }
  else
  {
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[11]);
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[11]); 
  }
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//showTemp7seg

//---------------------------------
void show2digits7seg()
{
  byte i;
  for (i=0; i<99; i++)
  {
     // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[i % 10]);
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[i / 10]); 
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
  delay(500);
  }
 }//show2digits7seg
 
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

