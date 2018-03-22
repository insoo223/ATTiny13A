/*------------ TheImitationGameKeypad.ino ----------------------------
    Emulate code breaking algorithm shown at the movie "The Imitation Game".
    This can serve a good motivator why students need to learn coding, 
      for it demonstrates
      how fast the seed number is got using a well-written computer programming.
      
    Date: March 6, 2015
    Author: Insoo Kim (insoo@hotmail.com)
--------------------------------------------------------------------*/
#include <Keypad.h>

#define MAXCODE  3
#define YELLOW  13
#define RED        11
#define GREEN    9

#define b0  9
#define b1  10
#define b2  11
#define b3  12


//---- global variables for using 4 by 3 membrane keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//---- global variables for Nazzi war-time passcode decoding
int nazziCode=0;
boolean setNazziCode = false;
boolean found = false;
unsigned long start, finish;
unsigned long trialNum=0;
unsigned long trialNumChristopher=0;

//---- function prototypes
void initSys();
void makeEnigmaSeed();
void showSelProg(int);
char chkValidity(char);
void  notifySetNazziCodeCompleted();
int char2int(char);
void helpMeChristopher();
void helpMeYouAngel();
void helpMeYouAngel_OLD();
void congrat();
void notifyFalseTrial();
void showNumBinary(int,int,int);

//-----------------------------
void setup()
{
  //Serial.begin(9600);
  delay(100);
  
  for (int i=9; i<=13; i++)
    pinMode(i, OUTPUT);
  initSys();  
}//setup

//-----------------------------
void loop()
{
  boolean debug=false;
  int analogVal;
  //if the Nazzi code is not set, then mak enigma seed(=Nazzi code).
  if (!setNazziCode)
    makeEnigmaSeed();
  //otherwise, try to break the Nazzi code
  else
  {
    // if not yet found, then keep trying to find the pass code(=Nazzi code)
    if (!found)
    {
      analogVal = analogRead(A0);
      if (analogVal < 20)
      {
        //Serial.println("Christopher is helping to solve out...");
        //use computer to turn on and off with lightening speed to find the Nazzi code.
        start = millis();
        helpMeChristopher();
      }
      else
      {
        if (debug)
        {
          //Serial.print("analogRead(A0): ");
          //Serial.println(analogVal);
        }
        //try to find it out for yourself, feel how long it takes to break the code...haha
        helpMeYouAngel();
      }
    }//if (!found)
    else
    {
      initSys();
    }//else (== found)
  }//else (== setNazziCode)
  
}//loop

//-----------------------------
void initSys()
{
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);

  //initial message  
  //Serial.println("Imagine that you are an officer of Nazzi in the World-War II.");
  //Serial.println("And you are ready to set Nazzi enigma code of 3 digits.");
  //Serial.println("Please, push 3 digits(Number only!) using the key pad...");   


  nazziCode = 0;  
  //flag indicating Nazzi code has been set or not
  setNazziCode = false;
  //flag indicating Nazzi code has been found or not
  found = false;
}//initSys

//-----------------------------
void makeEnigmaSeed()
{
  boolean debug=false;
  char k[MAXCODE];
  char c;
  
  for (int i=0; i<MAXCODE; i++)
    k[i] = '?';
    
  for (int i=0; i<MAXCODE; i++)
  {
    if (k[i] == '?')
    {
      k[i] = keypad.waitForKey();
      k[i] = chkValidity(k[i]);
      if (debug) //Serial.println(k[i]);
      if (k[i] == '?')
      {
        i = i-1;
        //Serial.println("Invalid code! Please enter number only between 0 and 9");      
      }
      else
      {
        //Serial.print("k");
        //Serial.print(i);      
        //Serial.println(" has been set.");      
        showSelProg(i+1);
      }
      if ( i == (MAXCODE-1) )
      {
        if (debug) 
        {
          for (int i=0; i<MAXCODE; i++)
            ;
            //Serial.print(k[i]);
          //Serial.println();
        }
        setNazziCode = true;
  
        //Compose Nazzi code with each number of code
        //For some reason, pow function and ^ seems not work. So used hard coded number combination.
        if ( setNazziCode )
          nazziCode = char2int(k[0])*100 + char2int(k[1])*10 + char2int(k[2])*1;
          
        ////Serial.print("Nazzi code: ");  
        ////Serial.println(nazziCode);  
        notifySetNazziCodeCompleted();
      }
    }//if (k[i] == '?')
  }//for
  start = millis();
}//makeEnigmaSeed

//-----------------------------
void showSelProg(int num)
{
  switch (num)
  {
    case 1:
      digitalWrite(b0, HIGH);
      digitalWrite(b1, LOW);
      break;
    case 2:
      digitalWrite(b0, LOW);
      digitalWrite(b1, HIGH);
      break;
    case 3:
      digitalWrite(b0, HIGH);
      digitalWrite(b1, HIGH);
      break;
  }
}//showSelProg

//-----------------------------
char chkValidity(char c)
{
  if (c >='0' && c <= '9')
    return c;
  else
    return '?';
}//chkValidity

//-----------------------------
void  notifySetNazziCodeCompleted()
{
  for (int n=0; n<3; n++)
    {
      digitalWrite(b0, HIGH);
      digitalWrite(b1, HIGH);
      delay(200);
      digitalWrite(b0, LOW);
      digitalWrite(b1, LOW);
      delay(200);
    }//for

    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    //Serial.println("Nazzi code has been set.");
    //Serial.println("Now, It's the time to break the WWII Nazzi enigma!");
}//notifySetNazziCodeCompleted

//-----------------------------
int char2int(char c)
{
  return (c-'0');
}//char2int

//-----------------------------
void helpMeChristopher()
{
  boolean showTrial=false;
  int crackedCode;
  int p1, p2, p3;
  
  for (p1=0; p1<=9; p1++)
  {
    if (found)
      break;
    for (p2=0; p2<=9; p2++)
    {
      if (found)          
        break;
      for (p3=0; p3<=9; p3++)
      {
        if (!found)
        {
          trialNumChristopher++;  
          crackedCode = p1*100 + p2*10 + p3;
          if (showTrial)
          {
            //Serial.print(crackedCode);
            //Serial.print(" ");
            if (trialNumChristopher % 10 == 0)
              //Serial.println();
              ;
          }
          if (crackedCode == nazziCode)
          {
            finish = millis();
            //Serial.println();
            //Serial.print("After tryig ");
            //Serial.print(trialNumChristopher);          
            //Serial.print("times, Christopher cracked the Nazzi code.");
            //Serial.print("The Nazzi code found is ");
            //Serial.println(crackedCode);
            //Serial.print("It takes time ");
            //Serial.print(finish - start);
            //Serial.println(" [ms] ");
            //showNumBinary(p1, p2, p3);
            congrat();

            found = true;
            trialNumChristopher=0;
            break;
          }
        }//if (!found)
      }//for (p3=0; p3<=9; p3++)
    }//for (p2=0; p2<=9; p2++)
  }//for (p1=0; p1<=9; p1++)

}//helpMeChristopher

//-----------------------------
//You, an angel, are trying to find out the Nazzi code, to save thousands of people in WWII.
//Feel in person how long it'll take to find out the enignma code.
void helpMeYouAngel()
{
  boolean debug=false;
  char k[MAXCODE];
  int crackedCode;

  trialNum++;  
  for (int i=0; i<MAXCODE; i++)
    k[i] = '?';
    
  for (int i=0; i<MAXCODE; i++)
  {
    if (k[i] == '?')
    {
      k[i] = keypad.waitForKey();
      k[i] = chkValidity(k[i]);
      if (debug) //Serial.println(k[i]);
      if (k[i] == '?')
      {
        i = i-1;
        //Serial.println("Invalid code! Please enter number only between 0 and 9");      
      }
      else
      {
        //Serial.print("k");
        //Serial.print(i);      
        //Serial.println(" has been set.");      
        showSelProg(i+1);
      }
      
      //if it is the last digit of the code      
      if ( i == (MAXCODE-1) )
      {
        if (debug) 
        {
          for (int i=0; i<MAXCODE; i++)
            ;
            //Serial.print(k[i]);
          //Serial.println();
        }
  
        //Compose Nazzi code with each number of code
        //For some reason, pow function and ^ seems not work. So used hard coded number combination.
        crackedCode = char2int(k[0])*100 + char2int(k[1])*10 + char2int(k[2])*1;

        //if your cracked code is exactly match with the Nazzi code          
        if (crackedCode == nazziCode)
        {
          finish = millis();
  
          //Serial.print("After tryig ");
          //Serial.print(trialNum);          
          //Serial.print("times, you cracked the Nazzi code.");
            //Serial.print("The Nazzi code found is ");
          //Serial.println(crackedCode);
          //Serial.print("It takes time ");
          //Serial.print(finish - start);
          //Serial.println(" [ms] ");
          //flash out LEDs to celebrate your triumph
          congrat();

          found = true;
          trialNum = 0;       
        }//if (crackedCode == nazziCode)
        //NOT broke the code yet.
        else
        {
          notifyFalseTrial();
        }//else
      }//if ( i == (MAXCODE-1) )
    }//if (k[i] == '?')
  }//for
}//helpMeYouAngel

//-----------------------------
//Flashing LEDs to celebrate your findings
void congrat()
{
    for (int g=9; g<=13; g++)
    {
      digitalWrite(g, HIGH);
      delay(100);
      digitalWrite(g, LOW);
      delay(100);
    }
    for (int h=0; h<3; h++)
    {
      for (int g=9; g<=13; g++)
        digitalWrite(g, HIGH);
      delay(200);
      for (int g=9; g<=13; g++)
        digitalWrite(g, LOW);
      delay(200);
    }
    digitalWrite(GREEN, HIGH);
    delay(3000);
}//confgrat

//-----------------------------
void notifyFalseTrial()
{
    for (int n=0; n<3; n++)
    {
      digitalWrite(RED, HIGH);
      delay(200);
      digitalWrite(RED, LOW);
      delay(200);
    }//for
    digitalWrite(b0, LOW);
    digitalWrite(b1, LOW);
    
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
}//notifyFalseTrial

//-----------------------------
void showNumBinary(int n1, int n2, int n3)
{
  PORTB = n1 << 1;
  delay(1000);  
  PORTB = n2 << 1;
  delay(1000);  
  PORTB = n3 << 1;
  delay(1000);  
}
