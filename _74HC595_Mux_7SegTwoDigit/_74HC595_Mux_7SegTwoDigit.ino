//**************************************************************//
// Target MCU: ATTiny13
//  Name    : _74HC595_7SegTwoDigit.ino                                
//  Author  : Insoo Kim
//  Ref: Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : March 17, 2015    
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : Driving two 7 segment LEDs
//****************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#define swPin   PB4     // tactile switch
#define buzzPin PB3     // 5V buzzer

//clock edge selector in PCINT intrp routine
volatile uint8_t pinIntCnt=0;
//notifier: 1 - tactile button is pressed the 1st time
//          0 - not yet pressed
volatile uint8_t firstPressed=0;
//tactile button counter as user selected menu
volatile uint8_t menuCnt=0;
//alarm duration in minute
volatile uint8_t ALMIN=0;
//notifier: 1 - a valid menu is selected
//          0 - not yet pressed
volatile uint8_t MENUSEL=0;
//notifier: 1 - assigned alarm is accomplished
//          0 - not yet done
volatile uint8_t doneAlarm = 0;

//volatile int curCnt125ms=0, prevCnt125ms=0, lapseCnt125ms;
//counter: current WDT intrpt
volatile int curCnt125ms=0;
//counter: previous WDT intrpt
volatile int prevCnt125ms=0;
//counter: from previous to current WDT intrpt
volatile int lapseCnt125ms;

//minute passed from the beginning of the current alarm
uint8_t minCnt;
//counter to advance hour counter
uint8_t oneSixthiethHour;
//hours from the beginning of the system power-on
uint8_t hourCnt;

//Duration between numbers
#define DURATION 500
//Pin connected to ST_CP of 74HC595
byte latchPin = 2;
//Pin connected to SH_CP of 74HC595
byte clockPin = 1;
////Pin connected to DS of 74HC595
byte dataPin = 0;

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
/*
byte TensDigit[10] = {
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
void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

    // enable PC(Pin Change) interrupt
    GIMSK |= _BV(PCIE);  //Enable PC interrupt
    // Enable pin change interrupt for PB4
    PCMSK |= _BV(swPin); // PCINT4
    
    // enable global interrupt
    sei();
    
    // Use the Power Down sleep mode
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}//setup

void loop() 
{
  /*
        if (ALMIN == 0)
            sleep_mode();   // go to sleep and wait for interrupt...
        else
            alarmMin(ALMIN);
  */
  test7seg();
}//loop

