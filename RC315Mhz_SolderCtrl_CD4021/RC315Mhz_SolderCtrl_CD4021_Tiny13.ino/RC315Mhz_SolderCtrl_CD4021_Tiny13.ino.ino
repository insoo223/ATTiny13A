/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : RC315Mhz_SolderCtrl_CD4021_Tiny13.ino
  Author  : Insoo Kim (insoo@hotmail.com)
  Date    : April 02, 2015    
  Update  : Mon Aug 10, 2015
  Desc    : 
    1) When system is on, the 1st power cycle engaged to relayPin for 90 sec,
        then the relayPin will on and off by user selectable on-going interval
        via RF remote control keys. 
       Default on-going interval is 10 sec.
       
    2) Set the on-going interval of relayPin on/off via RF remote control keys. 
       A button: 20 sec
       B button: 10 sec 
    3) Turn on or off the system by resetting or overflowing the safety counter.
       C button: system on again by clearing the safety counter
       D button: system off by overflowing the safety counter
    
  Notes   : 
    Clock source: Internal 9.6Mhz of ATTiny13A

  Ref:
  ** RC315Mhz_Decode.ino
  ** Test of RF decoder using SC2272-M4 chip
  **
  ** Written for and tested with Arduino Uno R3
  ** This example refered from the following link of Edward Comer.
  ** https://dzrmo.wordpress.com/2012/07/08/remote-control-pt2272-for-android/
  ** LICENSE: GNU General Public License, version 3 (GPL-3.0)
*****************************************************************/  
//calibration value for 500ms at ATtiny13A @ 9.6Mhz mode
#define DURATION 370 
// power off minutes for safety
#define safetyPwrOff 20

// 90 seconds for init heat. SHOULD put double of your wished-number
#define initHeatingINTERVAL (90*2)

byte intvalSec = 10;
// 10 seconds for on-going heat
byte onGoingHeatingINTERVAL;

//PB# of ATtiny13A
byte dataPin = 2; //Q8 of CD4021
byte clockPin = 1; // Clock of CD4021
byte latchPin = 0; //P/S ctrl of CD4021

byte relayPin = 3;

//for solder iron temperature control
byte prevLoop=0, curLoop=0, lapse=0;
byte loopCnt=0;
byte minCnt=0, onGoingCnt=0;

boolean DONE_initHeat = false;

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

//------------------------------------------------
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
     
//------------------------------------------------
void setup()
{
  //define CD4021 pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);

  pinMode(relayPin, OUTPUT);
} // setup
     
//------------------------------------------------
void loop()
{
  byte FourBits=0;
  
  onGoingHeatingINTERVAL = intvalSec*2;  
  solderCtrl();
  
  readRFdata();

  if(dirty)
  {
    dirty = 0;              // clear interrupt occurance flag
    FourBits = switchVar1 & 0x0F;
    switch (FourBits)
    {
      case 2: // Button A: 0 0 1 0
      {
        //20 sec on-going interval
        digitalWrite(relayPin, HIGH);
        intvalSec = 20;
        delay(200);
        digitalWrite(relayPin, LOW);
        break;
      }
      case 8: // Button B: 1 0 0 0
      {
        //10 sec on-going interval
        digitalWrite(relayPin, HIGH);
        intvalSec = 10;
        delay(200);
        digitalWrite(relayPin, LOW);
        break;
      }
      case 1: // Button C: 0 0 0 1
      {
        //Turn on the system by resetting the minCnt
        //digitalWrite(relayPin, HIGH);
        DONE_initHeat = false;
        minCnt = 0;
        loopCnt = 0;
        //delay(200);
        //digitalWrite(relayPin, LOW);
        break;
      }
      case 4: // Button D: 0 1 0 0
      {
        //Turn off the system by overflowing the minCnt
        //digitalWrite(relayPin, HIGH);
        minCnt = safetyPwrOff+1;
        //delay(200);
        //digitalWrite(relayPin, LOW);
        break;
      }
    } // switch (FourBits)
  } // if (dirty)

  delay(50);
} // loop

//------------------------------------------------
void solderCtrl() 
{
  if (minCnt < safetyPwrOff)
  {
    loopCnt++;  
  
    curLoop = loopCnt;
    lapse = curLoop - prevLoop;
  
    if (!DONE_initHeat)  
      //initial heating
      if (lapse < initHeatingINTERVAL)
      {
        digitalWrite(relayPin, HIGH);
        //digitalWrite(LEDpin, HIGH);
      }
      else
      {
        digitalWrite(relayPin, LOW);
        //digitalWrite(LEDpin, LOW);
        DONE_initHeat = true;
        loopCnt = 0;
        curLoop = 0;
        prevLoop = 0;
        minCnt = 2; // round of 1.5 min
        delay(onGoingHeatingINTERVAL*DURATION);
      }
    else
      //On-going heating
      if (lapse < onGoingHeatingINTERVAL)
      {
        digitalWrite(relayPin, HIGH);
        //digitalWrite(LEDpin, HIGH);
      }
      else
      {
        digitalWrite(relayPin, LOW);
        //digitalWrite(LEDpin, LOW);
        delay(onGoingHeatingINTERVAL*DURATION);
        loopCnt = 0;
        curLoop = 0;
        prevLoop = 0;
        onGoingCnt++;
        if (onGoingCnt == 3)
        {
          minCnt ++;
          onGoingCnt = 0;
        }
      }
      delay(DURATION);
  }// if (minCnt < safetyPwrOff)
  else
  {
    digitalWrite(relayPin, LOW);
    //digitalWrite(LEDpin, LOW);
  }
}//solderCtrl

//------------------------------------------------
////// -----shiftIn function
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
