/**************************************************************
  Target MCU & internal clock speed: ATtiny13 @ 9.6Mhz
  Name    : RC315Mhz_timedPowerCtrl_Tiny13.ino
  Author  : Insoo Kim (insoo@hotmail.com)
  Date    : Thu Aug 13, 2015    
  Update  : Mon Aug 31, 2015    
  Objective: Control electric power cycle
  Desc    : There are two mode of operation: 
      modeOP 1 for Fan, modeOP 2 for mosquito repellent vaporizer
      
    1) When system is on, the default modeOP is 1 or 2, 
        that is for control electric fan or mosquitor repellent vaporizer. 
       
    2) Set the modeOP  via RF remote control keys. 
       A button: 1 for Fan
       B button: 2 for mosquito repellent vaporizer 
       The on/off interval of modeOP 1 is 1 min each.
       The on/off intervals of modeOP 2 are various as follows:
        10 min On, 10 min Off for the first 2 hours
        10 min On, 20 min Off for the second 2 hours
        10 min On, 30 min Off for the next hours util safety cut-off hour limit (default:8 hours) 
       
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

// power off hours for safety
#define safetyPwrOff 8 //after 8 hrs of run-time, the system will be off.

//mode of operation: 1 - Fan, 2 - mosquito repellent vaporizer
byte modeOP = 2;

byte ONintervalMin;
byte OFFintervalMin;

//PB# of ATtiny13A
byte dataPin = 2; //Q8 of CD4021
byte clockPin = 1; // Clock of CD4021
byte latchPin = 0; //P/S ctrl of CD4021

byte relayPin = 3;

byte loopCnt=0;
byte minCnt=0, oneSixthiethHour=0;
byte hourCnt=0;

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

  timedPowerCtrl();
  readRFdata();

  if(dirty)
  {
    dirty = 0;              // clear interrupt occurance flag
    FourBits = switchVar1 & 0x0F;
    switch (FourBits)
    {
      //For fan control      
      case 2: // Button A: 0 0 1 0
      {
        digitalWrite(relayPin, HIGH);
        modeOP = 1;
        digitalWrite(relayPin, LOW);
        delay(200);
        break;
      }
      //For mosquito repellant vaporizer
      case 8: // Button B: 1 0 0 0
      {
        //30 min on-going off interval
        digitalWrite(relayPin, HIGH);
        modeOP = 2;
        digitalWrite(relayPin, LOW);
        delay(200);
        break;
      }
      case 1: // Button C: 0 0 0 1
      {
        //Turn on the system by resetting the minCnt
        hourCnt = 0;
        minCnt = 0;
        loopCnt = 0;
        break;
      }
      case 4: // Button D: 0 1 0 0
      {
        //Turn off the system by overflowing the minCnt
        hourCnt = safetyPwrOff+1;
        break;
      }
    } // switch (FourBits)
  } // if (dirty)

  delay(50);
} // loop

//------------------------------------------------
void timedPowerCtrl() 
{
  if (hourCnt < safetyPwrOff)
  {
    //every half sec loopCnt increse by one
    loopCnt++;  

    //For fan control    
    switch (modeOP)
    {
      case 1: //fan
        ONintervalMin = 1;
        OFFintervalMin = 1;
        break;
      case 2:  //mosquito
        if (hourCnt < 2)
        {
            ONintervalMin = 10;
            OFFintervalMin = 10;
        }
        else if (hourCnt < 4)
        {
            ONintervalMin = 10;
            OFFintervalMin = 20;
        }
        else 
        {
            ONintervalMin = 10;
            OFFintervalMin = 30;
        }
        break;
    }//switch (modeOP)
    
    //every min, minCnt & oneSixthiethHour increse by one
    //if ((loopCnt % 120) == 0) 
    //By experiment, we found modular by 100 gives more proper timing
    if ((loopCnt % 100) == 0) 
    {
      minCnt++;
      oneSixthiethHour++;
      if ( (oneSixthiethHour != 0) && (oneSixthiethHour % 60 == 0) )
      {
        hourCnt++;
        //oneSixthiethHour is reset every hour
        oneSixthiethHour = 0;
      }
      //loopCnt is reset every min
      loopCnt = 0;
    }//if ((loopCnt % 120) == 0) 
    
    //From minCnt is zero to ONintervalMin, 
    //the relay is turned On
    if (minCnt < ONintervalMin)
    {
      digitalWrite(relayPin, HIGH);
      readRFdata();
    }
    //From minCnt is ONintervalMin to twice ONintervalMin, 
    //the relay is turned Off
    else
    {
      digitalWrite(relayPin, LOW);
      if ( minCnt >= (ONintervalMin + OFFintervalMin) )
      {
        //minCnt is reset every twice ONintervalMin
        minCnt = 0;
      }
    }
    //wait half a second, 
    //so that the loopCnt increase by one at every half sec
    delay(DURATION);
  }// if (hourCnt < safetyPwrOff)
  else
  {
    //system off
    digitalWrite(relayPin, LOW);
    if (hourCnt == 250)
      hourCnt = safetyPwrOff;
  }//else, that is (minCnt >= safetyPwrOff)
}//timedPowerCtrl

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
