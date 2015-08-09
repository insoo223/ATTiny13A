/**************************************************************
 Target MCU: ATTiny13
  Name    : CLKinternal_NoMillis_SolderCtrl.ino --> SolderTempCtrl_Tiny13.ino
  Author  : Insoo Kim
  Date    : March 21, 2015    
  Notes   : Solder termperature control using power switching period
****************************************************************/

//calibration value for 500ms at ATtiny13 @ 9.6Mhz mode
#define DURATION 370 

// power off minutes for safety
#define safetyPwrOff 20

// 90 seconds for init heat
#define initHeatingINTERVAL (90*2)
// 10 seconds for on-going heat
#define onGoingHeatingINTERVAL (10*2)

#define relaySWpin 0
#define LEDpin 1

byte prevLoop=0, curLoop=0, lapse=0;
byte loopCnt=0;
byte minCnt=0, onGoingCnt=0;

boolean DONE_initHeat = false;

//-------- SETUP
void setup() {
  pinMode(relaySWpin, OUTPUT);
  pinMode(LEDpin, OUTPUT);
}//setup

//-------- LOOP
void loop() 
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
        digitalWrite(relaySWpin, HIGH);
        digitalWrite(LEDpin, HIGH);
      }
      else
      {
        digitalWrite(relaySWpin, LOW);
        digitalWrite(LEDpin, LOW);
        DONE_initHeat = true;
        loopCnt = 0;
        curLoop = 0;
        prevLoop = 0;
        minCnt = 1.5;
        delay(onGoingHeatingINTERVAL*DURATION);
      }
    else
      //On-going heating
      if (lapse < onGoingHeatingINTERVAL)
      {
        digitalWrite(relaySWpin, HIGH);
        digitalWrite(LEDpin, HIGH);
      }
      else
      {
        digitalWrite(relaySWpin, LOW);
        digitalWrite(LEDpin, LOW);
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
    digitalWrite(relaySWpin, LOW);
    digitalWrite(LEDpin, LOW);
  }
}//loop

