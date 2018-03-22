/*
  DigitalPortStatus
Target MCU: ATTiny13

 Reads a digital input on pin 2, prints the result to the serial monitor
 
 This example code is in the public domain.
 */

// digital pin 2 has a OutputPin555 attached to it. Give it a name:
int OutputPin555 = 2;

// the setup routine runs once when you press reset:
void setup() {
  // make the OutputPin555's pin an input:
  pinMode(OutputPin555, INPUT);
}

int Output555State=0;
int prevState=0;
unsigned long clockCnt = 0;
unsigned long minCnt = 0;
// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  Output555State = digitalRead(OutputPin555);
  if (prevState != Output555State)
    clockCnt++;
  if ( (clockCnt !=0) and (clockCnt % 120 == 0) )
  {
    minCnt++;
    clockCnt = 0;
  }
  prevState = Output555State;
  delay(1000);        // delay in between reads for stability
}
