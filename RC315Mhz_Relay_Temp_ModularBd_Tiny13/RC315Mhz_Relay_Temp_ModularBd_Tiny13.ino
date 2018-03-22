/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : RC315Mhz_Relay_Temp_ModularBd_Tiny13.ino
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
#define halfSecond  500*4
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
byte dataPin = 2; //Q8 of CD4021
byte clockPin = 1; // Clock of CD4021
byte latchPin = 0; //P/S ctrl of CD4021

byte relay = 3;
//Pi(Parallel input)# of CD4021 
// RF receiver module(“XY-DJM-5V”) pin defines - wired to Arduino Digital pins
// Wire the “XY-DJM-5V” per this assignment
#define Rx_VT 8 // Pi 8 at CD4021 Trigger
#define Rx_D0 4 // Pi 4 
#define Rx_D1 3 // Pi 3
#define Rx_D2 2 // Pi 2
#define Rx_D3 1 // Pi 1  
// Pi 7, 6, 5 (residual pins of CD4021) is open to allow future usage.

// interrupt has occurred flag     
byte dirty = 0;                          
     
//Define variables to hold the data 
//for shift register.
//starting with a non-zero numbers can help
//troubleshoot
byte switchVar1 = 0x48;  //01001000

//---------------------------------
void setup()
{
  //define CD4021 pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);

  pinMode(relay, OUTPUT);
} // setup
     
//---------------------------------
void loop()
{
  byte FourBits=0;
  
  readRFdata();

  if(dirty)
  {
    dirty = 0;              // clear interrupt occurance flag
    FourBits = switchVar1 & 0x0F;
    switch (FourBits)
    {
      case 2: // Button A: 0 0 1 0
      {
        digitalWrite(relay, HIGH);
        break;
      }
      case 8: // Button B: 1 0 0 0
      {
        digitalWrite(relay, LOW);
        break;
      }
      case 1: // Button C: 0 0 0 1
      {
        chkTemperature();
        break;
      }
      case 4: // Button D: 0 1 0 0
      {
        break;
      }
    } // switch (FourBits)
  } // if (dirty)

  delay(50);
} // loop

//---------------------------------
void readRFdata()
{
  byte triggered=0;
  
  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(latchPin,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(latchPin,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
  switchVar1 = shiftIn(dataPin, clockPin);

  triggered = switchVar1 & 0x80;
  if(triggered)
    dirty = 1;                          // flag interrupt occurance
} //readRFdata
     
////// ----------------------------------------shiftIn function
///// just needs the location of the data pin and the clock pin
///// it returns a byte with each bit in the byte corresponding
///// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0

byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
//we will be holding the clock pin high 8 times (0,..,7) at the
//end of each time through the for loop

//at the begining of each loop when we set the clock low, it will
//be doing the necessary low to high drop to cause the shift
//register's DataPin to change state based on the value
//of the next bit in its serial information flow.
//The register transmits the information about the pins from pin 7 to pin 0
//so that is why our function counts down
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}//shiftIn

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
  showTemp7seg(C, false);  
  //showTempDiceLED(C);
  delay(halfSecond*2); // 1 seconds

/*
  showTempDiceLED(rawV/10);
  delay(halfSecond*2); // 1 seconds
  showTempDiceLED(rawV%10);
  delay(halfSecond*2); // 1 seconds
*/

  //showTempDiceLED(0);
  showTemp7seg(BLANK, false);
  delay(halfSecond*8); // 2 seconds
  
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

