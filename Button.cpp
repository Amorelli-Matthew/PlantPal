#include "Button.h"

volatile unsigned char *my_DDRE  = (unsigned char *)0x2D;  // DDR for Port E interupt pin, pin 2, aka for the starting stoping progran
volatile unsigned char *my_PORTE = (unsigned char *)0x2E;  // PORT for Port E

volatile unsigned char*  my_PINC  = (volatile unsigned char*)0x26; //for pin 36 aka the rest pun
volatile unsigned char*  my_DDRC  = (volatile unsigned char*)0x27;
volatile unsigned char*  my_PORTC = (volatile unsigned char*)0x28;

void StartStopButtonInit()
{
  //for the starting and stopping the system basically does pinMode(StartStopButton, INPUT_PULLUP);  
  
  *my_DDRE &= ~(1 << 4);
  *my_PORTE |= (1 << 4);
  
  attachInterrupt(digitalPinToInterrupt(StartStopButtonPin),StartStopISR,FALLING );
}

//init reset button
void RestButtonInit()
{
    // Configure pin 36 as input with pull-up like using pinMode
  *my_DDRC  &= ~(1 << PC1_BIT);  // input
  *my_PORTC |=  (1 << PC1_BIT);  // enable pull-up
}

void readResetButton()
{
  //so B says that mills is fine but Micros is better imo
    currentTime = micros();

  // Direct hardware read — no digitalRead()
  reading = ((*my_PINC & (1 << PC1_BIT)) ? HIGH : LOW);

  // Debounce
  if (reading != lastReading) {
    previousTime = currentTime;
    lastReading = reading;
  }

//delay to help debounce
  if ((currentTime - previousTime) >= RESET_DEBOUNCE_US && reading != stableState) {
    stableState = reading;

    if (stableState == LOW) {
      Serial.println("Button Pressed");
    }
  }
}

  
void CheckifProgramIsOnOrOff()
{
  
  //ifButtonPressed and the program is off, start the program
  if (StartStopButtonEvent) {
    StartStopButtonEvent = 0; //Reset StartStopButtonEvent back to zero
          
     //check if the system is on or off
     switch(ProgramStatus)
    { 
      case LOW:
      
       //starts the program
        ProgramStatus =1;    
        //  Serial.println("Program on!");
          break;
      
      case HIGH:
        ProgramStatus = 0;
      //  Serial.println("Program off!");
        break;

  }          
       
  }
  

}

void StartStopISR()
{
  currentTime = micros();

   // if it's been stable for DEBOUNCE_MS, accept the new state
  if ((currentTime - previousTime) >= (STARTSTOP_DEBOUNCE_US)) {
    previousTime = currentTime;   
     StartStopButtonEvent = !StartStopButtonEvent;
  }   

  }


  