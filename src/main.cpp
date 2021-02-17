#include <Arduino.h>


#define F_CPU 1000000
// -----------------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
// -----------------------------------------------------------------------------------
#define LED(n) DDRB = 0;PORTB=0;DDRB = led[n][1]; PORTB = led[n][0]; 

enum GameMode {Idle,Gaming,Winning,Losing};
int ledNumber=0;
//button down time for filtering the button input
int onTime=0;
const int onFilter=1000;
bool clicked=false;

//0= diffecalty select, idling, 1 gaming, 2 winning, 3 losing
GameMode mode=Idle;
//interval for current diffecalty
int intervalDiff=100;
int selectDiffecalty();

GameMode playGame(int interval);
void checkPressed();
bool pressed();
int calcDiffecalty(int diff);

const unsigned char led[13][2] =
{
  {8, 8 | 4},
  {8, 8 | 1},
  {8, 8 | 2},
  {1, 1 | 4},
  {1, 1 | 8},
  {1, 1 | 2},
  {2, 2 | 4},
  {2, 2 | 1},
  {2, 2 | 8},
  {4, 4 | 8},
  {4, 4 | 2},
  {4, 4 | 1},
  {0, 0} 
};

int selectDiffecalty()
{
  int diffecalty=0;
  int time =millis();
  static int lastFlash=0;
  if ((time-lastFlash)>125 || (time-lastFlash)<0)
  { 
    ledNumber++;
    lastFlash=time;
    if (ledNumber>11)
    {
      ledNumber=0;
    }
  }

  if (clicked)
  {
    diffecalty=ledNumber+1;
  }
  else
  {
    diffecalty=0;
  }
  return diffecalty;
}

//return interval for a diffecalty
int calcDiffecalty(int diffecalty)
{
  return 250/diffecalty*1.5;  
}

GameMode playGame(int interval)
{
  static int nextLed=1;
  static int currentLed=0;
  static int lastFlash=0;
  //flash
  int time =millis();
  if ((time-lastFlash)>interval || (time-lastFlash)<0)
  {
    lastFlash=time;
    if (ledNumber==currentLed)
    {
      ledNumber=nextLed;
    }
    else
    {
      ledNumber=currentLed;
    }
  }
  if (clicked)
  {
    if (ledNumber==currentLed)
    {
      //reset current & next
      nextLed=1;
      currentLed=0;
      return Losing;
    }
    else
    {
       currentLed=nextLed;
       nextLed++;
       ledNumber=currentLed;
       if (nextLed>=12)
       {
         nextLed=1;
         currentLed=0;
         return Winning;
       }
    }
    return Gaming;
  }
  
  
  
  
  return Gaming;
}

GameMode losing()
{
  static int lastFlash=0;
  static int blinkNow=6;
  static int lastLed=0;
  //flash
  int time =millis();
  if ((time-lastFlash)>20 || (time-lastFlash)<0)
  {
    lastFlash=time;
    if (blinkNow>0)
    {
      blinkNow--;
      if (ledNumber!=12)
      {
        lastLed=ledNumber;
        ledNumber=12;
      }      
      else
      {
        ledNumber=lastLed;
      }
      return Losing;
    }
    if (ledNumber==0)
    {
      blinkNow=6;
      return Idle;
    }
    ledNumber--;
  }
  return Losing;    
}

GameMode winning()
{
  static int lastFlash=0;
  static int blinkNow=20;
  static int cycle=5;
  static int blinkInterval=5;
  //flash
  int time =millis();
  if ((time-lastFlash)>blinkInterval || (time-lastFlash)<0)
  {
    lastFlash=time;
    if (blinkNow>0)
    {
      blinkNow--;
      if (ledNumber!=12)
      {
        ledNumber=12;
      }      
      else
      {
        ledNumber=11;
      }
      return Winning;
    }
    else
    {
      blinkInterval=2;
    }
    

    if (ledNumber==0)
    {
      cycle--;
      ledNumber=12;
    }
    ledNumber--;
    if (cycle==0)
    {
      blinkNow=20;
      cycle=5;
      blinkInterval=5;
      return Idle;
    }
    
  }
  return Winning;    
}



void nextMode()
{
  switch (mode)
  {
  case Idle:
  {
    int diffecalty=selectDiffecalty();
    if (diffecalty>0)
    {
      intervalDiff=calcDiffecalty(diffecalty);
      mode = Gaming;
    }
    break;
  }
  case Gaming:
  {
    mode=playGame(intervalDiff);
    break;
  }
  case Winning:
  {
    mode=winning();
    break;
  }
  case Losing:
  {
    mode=losing();
    break;
  } 
  default:
    break;
  }

}

void checkPressed()
{

  if (!(PINB & 16))
  {
    onTime++; 
    clicked=false;   
    return;
  }
  else if(onTime>onFilter)
  {
    clicked=true;
    onTime=0;
    return;
  }
  clicked=false;
  onTime=0;
  
}

bool pressed()
{
  if (onTime>onFilter)
  {
    return true;
  }
  return false;  
}

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  nextMode();
  
  
  LED(ledNumber);
  checkPressed();

}