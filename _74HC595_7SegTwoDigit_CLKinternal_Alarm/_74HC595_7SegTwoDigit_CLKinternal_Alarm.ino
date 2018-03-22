//**************************************************************//
// Target MCU: ATTiny13
//  Name    : _74HC595_7SegTwoDigit_CLKinternal_Alarm.ino                                
//  Author  : Insoo Kim
//  Ref: Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : March 19, 2015    
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : Driving two 7 segment LEDs
//****************************************************************

byte alarmMin = 1;
boolean alarmEnable = false;

//Duration between numbers
//#define DURATION 2900  // for 170 pin bread b'd
//#define buzzPin 4   // for 170 pin bread b'd
//#define startPin 3  // for 170 pin bread b'd

#define DURATION 370 // for 2*8 perf b'd
#define buzzPin 3   // for 2*8 perf b'd
#define shortBuzz 3  // buzzing 3 times
#define longBuzz 10  // buzzing 10 times
#define startPin 4  // for 2*8 perf b'd
#define ledPin 0  // for 2*8 perf b'd

//Pin connected to ST_CP of 74HC595
#define latchPin 0
//Pin connected to SH_CP of 74HC595
#define clockPin 1
////Pin connected to DS of 74HC595
#define dataPin 2

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

//-------- FUNCTION PROTOTYPES
// Arduino Sketch C doesn't need to declare function prototypes
// But to conform with ANSI C, here i follow the standard C rules.
void showCnt(byte, boolean);
void chkStart();
void buzz(byte);
void alarm(byte);
void DelayNoBlock(unsigned long );

//-------- SETUP
void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(startPin, INPUT);
  pinMode(buzzPin, OUTPUT);
}//setup

boolean debug3s=false;
boolean debug1m=false;

//-------- LOOP
void loop() 
{
  byte num2show;

  chkStart();

  if (start)
  {
    clockCnt++;
    
    if (clockCnt % 2 == 0)   
      secCnt++;
/*      
    if (debug3s)
      if (secCnt == 3)
        //buzz(longBuzz); 
        //buzz(shortBuzz);
        chkAlarm(alarmMin);
*/

    if (secCnt == 60)
    {
      minCnt++;
/*
      if (debug1m)
        if (minCnt == 1) 
          //buzz(longBuzz); 
          buzz(shortBuzz);
*/
      secCnt = 0;
      clockCnt = 0;
    }
  
    //add one HOUR every 60 minutes
    if ( minCnt == 60 )
    {
      hourCnt++;
      minCnt = 0;
    }
    
    //====== Show laps time at two digits 7 segment
    //blink DP every second
    if (clockCnt % 2 == 0)   
    {
      if (hourCnt == 0)
        num2show = secCnt;
      else
        num2show = minCnt;
      //showCnt(num2show, false);
    }
    else
    {
      if (hourCnt == 0)
        num2show = minCnt;
      else
        num2show = hourCnt;
      //showCnt(num2show, true);
    }
    
    //========== check Alarm enable status
    if (alarmEnable == true)
    {
      digitalWrite(ledPin, HIGH);
      chkAlarm(alarmMin);
    }
    else
      digitalWrite(ledPin, LOW);

    delay(DURATION);        // delay in between reads for stability
    //DelayNoBlock(DURATION);
  }//if (start)
}//loop

//-----------------------------------
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

//-----------------------------------
void blinkLED(byte num)
{
  byte i;
  for (i=0; i<num; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(DURATION/2);
    digitalWrite(ledPin, LOW);
    delay(DURATION/2);
  }
}
//-----------------------------------
void chkStart()
{
  if (digitalRead(startPin) == LOW)
  {
    start = !start;
    blinkLED(2);
    //alarmEnable = true;
    if (start == true)
    {
      alarmEnable = true;
      //digitalWrite(ledPin, HIGH);
    }
    else
    {
      alarmEnable = false;
      //digitalWrite(ledPin, LOW);
    }

    clockCnt = 0;
    secCnt = 0;
    minCnt = 0;
    hourCnt = 0;
  }
}//chkStart

//-----------------------------------
void buzz(byte times)
{
  const byte buzzInterval = 200;
  byte i;
  for (i=0; i<times; i++)
  {
    digitalWrite(buzzPin, HIGH);
    delay(buzzInterval);
    digitalWrite(buzzPin, LOW);
    delay(buzzInterval);
  }
}//buzz

//-----------------------------------
void chkAlarm(byte num)
{
  if(num == minCnt)
  {
    buzz(shortBuzz);
    alarmEnable = false;
    digitalWrite(ledPin, LOW);
  }
}//chkAlarm
//-----------------------------------

void DelayNoBlock(unsigned long ms)
{
  unsigned long prevSec = 0; 
  unsigned long curSec = millis();
  
  while (curSec - prevSec < ms)
    curSec = millis();
}//DelayNoBlock

