/*
 Target MCU: ATtiny85
 HC-SR04 Ping distance sensor:
 VCC to arduino 5v 
 GND to arduino GND
 Echo to Arduino pin 7 
 Trig to Arduino pin 8
 
 This sketch originates from Virtualmix: http://goo.gl/kJ8Gl
 Has been modified by Winkle ink here: http://winkleink.blogspot.com.au/2012/05/arduino-hc-sr04-ultrasonic-distance.html
 And modified further by ScottC here: http://arduinobasics.blogspot.com.au/2012/11/arduinobasics-hc-sr04-ultrasonic-sensor.html
 on 10 Nov 2012.
 */

//#include "Module_74HC595_7SegTwoDigit.ino"
//Pin connected to ST_CP of 74HC595
int latchPin = 13;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

byte OnesDigit[11] = {
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
		0b00000001 //DP(10)
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
		0b11100110, //9                
		0b00000001 //DP(10)
	};//TensDigit[10]


#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 4 // Onboard LED

int maximumRange = 300; // Maximum range needed (default 200)
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

void setup() {
 //Serial.begin (9600);
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
}

void loop() {
/* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin, LOW);
 duration = pulseIn(echoPin, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance = duration/58.2;
 
 if (distance >= maximumRange || distance <= minimumRange){
 /* Send a negative number to computer and Turn LED ON 
 to indicate "out of range" */
 //Serial.println("-1");
 digitalWrite(LEDPin, HIGH); 
 }
 else {
 /* Send the distance to the computer using //Serial protocol, and
 turn LED OFF to indicate successful reading. */
 //Serial.println(distance);
 if (distance < 100)
 {
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[distance % 10]);
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[distance / 10]); 
    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
 }
 else if (distance < 200)
 {
   distance = distance - 100;
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[distance % 10] | OnesDigit[10] );
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[distance / 10]); 
    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
 }
 else if (distance < 300)
 {
   distance = distance - 200;
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    shiftOut(dataPin, clockPin, MSBFIRST, OnesDigit[distance % 10] | OnesDigit[10] );
    shiftOut(dataPin, clockPin, MSBFIRST, TensDigit[distance / 10] | OnesDigit[10] ); 
    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
 }
 
 digitalWrite(LEDPin, LOW); 
 }
 
 //Delay 50ms before next reading.
 delay(1000);
}
