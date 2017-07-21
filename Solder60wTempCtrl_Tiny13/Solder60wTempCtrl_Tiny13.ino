/**************************************************************
 Target MCU: ATTiny13A
  Name    : Solder60wTempCtrl_Tiny13.ino
  Author  : Insoo Kim
  Date    : March 21, 2015    
  Update  : Mon Oct 19, 2015
  Desc    : 
    1) Solder iron termperature control by intermittent power switching
        initially gives 90 seconds of power, then on & off every 10 seconds.
    2) Safety cut-off time is set to 20 minutes after beginning system running.
  Notes   : 
    Clock source: Internal 9.6Mhz of ATTiny13A
    
****************************************************************/

//calibration value for 500ms at ATtiny13 @ 9.6Mhz mode
#define DURATION 370 

// power off minutes for safety
#define safetyPwrOff 20

// 90 seconds for init heat. Muliply by 2 for it loops every half a second.
//#define initHeatingMin (90*2) //for an iron of 60W
byte initHeatingMin=1; //1min

//#define initHeatingMin 3 //for an iron of 30W//
// 10 seconds for on-going heat
//#define ongoingONsec (10*2) //for an iron of 60W
//Muliply by 2 for it loops every half a second.
byte ongoingONsec=(5*2); //5sec
byte ongoingOFFsec=(10*2); //10sec

#define relaySWpin 0
//#define LEDpin 1

byte prevLoop=0, curLoop=0, lapse=0;
byte loopCnt=0;
byte minCnt=0, onGoingCnt=0;

boolean DONE_initHeat = false;

//-------- SETUP
void setup() {
  pinMode(relaySWpin, OUTPUT);
  //pinMode(LEDpin, OUTPUT);
}//setup

//-------- LOOP
void loop() 
{
  if (minCnt < safetyPwrOff)
  {
    loopCnt++;  
  
      //every min, minCnt increse by one
    if ((loopCnt % 120) == 0) 
    {
      minCnt++;
      //loopCnt is reset every min
      loopCnt = 0;
    }//if ((loopCnt % 120) == 0) 


    if (!DONE_initHeat)  
      //initial heating
      //if (lapse < initHeatingMin)
      if (minCnt < initHeatingMin)
      {
        digitalWrite(relaySWpin, HIGH);
        //digitalWrite(LEDpin, HIGH);
      }
      else
      {
        digitalWrite(relaySWpin, LOW);
        //digitalWrite(LEDpin, LOW);
        DONE_initHeat = true;
        loopCnt = 0;
        minCnt = 0;
        delay(ongoingONsec*DURATION);
      }
    else //(DONE_initHeat == true)  
      //On-going heating
      if (loopCnt < ongoingONsec)
      {
        digitalWrite(relaySWpin, HIGH);
        //digitalWrite(LEDpin, HIGH);
      }
      else
      {
        digitalWrite(relaySWpin, LOW);
        //digitalWrite(LEDpin, LOW);
        if ( loopCnt >= (ongoingONsec + ongoingOFFsec) )
        {
          loopCnt = 0;
        }
      }
      delay(DURATION);
  }// if (minCnt < safetyPwrOff)
  else
  {
    digitalWrite(relaySWpin, LOW);
    //digitalWrite(LEDpin, LOW);
  }
}//loop

