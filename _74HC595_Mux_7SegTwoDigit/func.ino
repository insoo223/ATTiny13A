//------------------------------------------------
void alarmMin (uint8_t num)
{
    uint8_t prevMinCnt;
    uint8_t i;
    
    //reset WDT counter to give zero-reference to alarm duration
    curCnt125ms = 0;
    prevCnt125ms = 0;
    minCnt = 0;
    prevMinCnt=minCnt;
    while (!doneAlarm)
    {
        if (prevMinCnt != minCnt)
        {
            blinkLED(1);
            prevMinCnt=minCnt;
        }
        //check whether alarm is done
        if (minCnt >= num)
        {
            //show alarm minutes just done
            blinkLED(num);
            
            //bee-lee bee-lee bee-lee
            for (i=0; i<10; i++)
            {
                PORTB |= _BV(buzzPin); //on buzz
                _delay_ms(2);
                PORTB &= ~_BV(buzzPin); //off buzz
                _delay_ms(2);
            }
            
            doneAlarm = 1;
        }
    }
    //reset alarm duration
    ALMIN=0;
    //reset usr selected menu
    menuCnt = 0;
    //reset menu selection notification
    MENUSEL=0;
    //reset alarm finish notification
    doneAlarm=0;
    //reset wake up WDT interrupt sentinel
    firstPressed=0;
    //disable WDT and only wake up when the tactile button is pressed
    disableWDT();
    
}//alarmMin

//------------------------------------------------
void blinkLED(uint8_t num)
{
  /*
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0x01); 
      shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[num]);
      digitalWrite(latchPin, HIGH);
      */
  /*
    int i;
    
    for (i=0; i<num; i++)
    {
        PORTB = 1 << ledPin;
        _delay_ms(10);
        PORTB = 0 << ledPin;
        _delay_ms(10);
    }
    */
}//blinkLED

//------------------------------------------------
void disableWDT()
{
    // Disable watchdog timer interrupts
    WDTCR &= ~(1<<WDTIE);
}//disableWDT

//------------------------------------------------
void proceedTime()
{
    minCnt++;
    oneSixthiethHour++;
    if ( (oneSixthiethHour != 0) && (oneSixthiethHour == 60) )
    {
        hourCnt++;
        //oneSixthiethHour is reset every hour
        oneSixthiethHour = 0;
    }
}//proceedTime

void test7seg()
{
  for (byte onesColumn = 0; onesColumn < 10; onesColumn++) 
  {
    for (byte i=0; i<30; i++)
    {
      // take the latchPin low so 
      // the LEDs don't change while you're sending in bits:
      digitalWrite(latchPin, LOW);
      // shift out the bits:
      shiftOut(dataPin, clockPin, MSBFIRST, ~0x02); 
      shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[onesColumn]);
      //take the latch pin high so the LEDs will light up:
      digitalWrite(latchPin, HIGH);
      // pause before next value:
  
      digitalWrite(latchPin, LOW);
      shiftOut(dataPin, clockPin, MSBFIRST, 0x03); 
      shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[onesColumn]);
      digitalWrite(latchPin, HIGH);
      delay(10);
    }//for i
  }//for onesColumn
}
