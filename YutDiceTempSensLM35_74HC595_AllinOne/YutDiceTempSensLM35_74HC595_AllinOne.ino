/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : YutDiceTempSensLM35_74HC595_AllinOne.ino
  Author  : Insoo Kim
  Date    : March 26, 2015    

  Description: 
    1.To play Yut
       Press and hold the button while powering up,
       realease the button after any led lits up. 
    2. To measure temperature (Default)
      Don't press the button while powering up.
    3.To play dice       
       Can be played but not yet implemented to manipulate by end-user.
       Only set by Sketch

*****************************************************************/  

//********* Mode of functions
#define TEMP 0
#define YUT 1
#define DICE 2

//********* Temperature
//#define debugSerial
//#define debugIntOnly
//#define TargetBdUNO

//USB tiny programmer
//#define RefVtg 5030

//mV resolution is required to catch more precise temperature.
//so that, i measured power supply with mini digital meter.
//for 5 & 12V DC pwr supplier
#define RefVtg 5000

//for LG Dacom 070 handset
//#define RefVtg 4080 

//Pin connected to ST_CP of 74HC595
byte latchPin = 0;
//Pin connected to SH_CP of 74HC595
byte clockPin = 3;
//Pin connected to DS of 74HC595
byte dataPin = 2;
  
//switch pin
byte SWpin = 1;

/*
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
*/

//********* Yut & Dice
//#define halfSecond  500 // 370

#define maxCntDice 255
#define maxCntYut 17

byte cnt=0;
byte mode=0;
byte curState;

byte YutLED[6] = {
		0b00000000, //0 Back Doh
		0b00001000, //1 Doh
		0b10000100, //2 Gae
		0b00111000, //3 Girl
		0b11000110, //4 Yut
		0b11001110 // 5 Mo
	};//YutLED[6]
byte DiceLED[7] = {
		0b00000000, //0 Blank
		0b00001000, //1 
		0b10000100, //2 
		0b01001010, //3 
		0b11000110, //4 
		0b11001110, // 5 
		0b11110110 //6 
	};//DiceLED[7]

//----------function prototypes
void chkTemperature();
void showTempDiceLED(byte );
void countUpTo(byte );
//void playDice();
void playYut();
void dispYutDice(byte , byte *);

//---------------------------------
//this sets the ground pin to LOW and the input voltage pin to high
void setup()
{
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(SWpin, INPUT);
  
  if (digitalRead(SWpin) == HIGH)
    mode = TEMP;
  else
    mode = YUT;

  //analogReference(EXTERNAL);
}//setup

//---------------------------------
void loop()
{
  curState = digitalRead(SWpin);

  switch (mode)
  {
    case TEMP:
      chkTemperature();
      break;

    case DICE:
      //playDice();
      break;
      
    case YUT:
      playYut();  
      break;
  }//switch
  
}//loop

//---------------------------------
void chkTemperature()
{
  int rawV;
  float C;

  // pin3 (PB4, ADC2)
  // if connected to pin1 (PB5, RESET), you'll be in trouble 
  rawV = analogRead(2);
  C = rawV * 0.48828125;

  showTempDiceLED(C);
  delay(1000); // 1 seconds
  showTempDiceLED(0);
  delay(2000); // 2 seconds

  chkButton();
  
}//chkTemperature

//---------------------------------
void chkButton()
{
}

//---------------------------------
void showTempDiceLED(byte num)
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(latchPin, LOW);
  
  // shift out the bits:
  //Q0 of 74HC595 is not used in my dice circuit, so that shift left by one bit 
  shiftOut(dataPin, clockPin, MSBFIRST, num << 1);
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//showTempDiceLED

//---------------------------------
/*
void playDice()
{
  if (curState == LOW)
    countUpTo(maxCntDice);

  if (curState == LOW)
  {
    dispYutDice(cnt%6, DiceLED);
    delay(10);
  }
  else
  {
    dispYutDice(cnt%7, DiceLED);
  }
  delay(10);
}//playDice
*/
//---------------------------------
void playYut()
{

  if (curState == LOW)
    countUpTo(maxCntYut);

  if (curState == LOW)
  {
    dispYutDice(cnt%6, YutLED);
    delay(10);
  }
  else
  {
      switch (cnt % maxCntYut)
      {
        case 0: //back Doh
        case 14: //back Doh
          dispYutDice(0, YutLED);
          
          break;
        case 1: // Doh
        case 12: // Doh
        case 15: // Doh
          dispYutDice(1, YutLED);
          break;
        case 2: // Gae
        case 6: // Gae
        case 9: // Gae
        case 11: // Gae
        case 13: // Gae
        case 16: // Gae
          dispYutDice(2, YutLED);
          break;
        case 3: // Girl
        case 7: // Girl
        case 10: // Girl
          dispYutDice(3, YutLED);
          break;
        case 4: // Yout
        case 8: // Yout
          dispYutDice(4, YutLED);
          break; 
        case 5: // Mo
          dispYutDice(5, YutLED);
          break;
      }//switch
      //delay(halfSecond);
  }//else
  delay(10);
}//playYut

//---------------------------
void   countUpTo(byte maxNum)
{  
  if (cnt < maxNum)
    cnt++;
  else
    cnt = 0;
    
}//countUpTo

//---------------------------
void dispYutDice(byte n, byte *arr)
{
  digitalWrite(latchPin, LOW);

  // shift out the bits:
  shiftOut(dataPin, clockPin, MSBFIRST, arr[n]);
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);
}//disp        


