#include "Button.h"

volatile unsigned char *my_DDRE  = (unsigned char*)0x2D;  // DDR for Port E interupt pin, pin 2, aka for the starting stoping the progran
volatile unsigned char *my_PORTE = (unsigned char*)0x2E;  // PORT for Port E

volatile unsigned char*  my_PINC  = (volatile unsigned char*)0x26; //for pin 36 aka the rest pin
volatile unsigned char*  my_DDRC  = (volatile unsigned char*)0x27;
volatile unsigned char*  my_PORTC = (volatile unsigned char*)0x28;

unsigned long previousTime = 0;
unsigned long currentTime = 0;

volatile bool StartStopButtonEvent;    
volatile bool ProgramStatus; 

//vars for the last reading
 int lastReading  = HIGH;
int stableState  = HIGH;
int reading;




void StartStopButtonInit()
{
  //for the starting and stopping the system basically does pinMode(StartStopButton, INPUT_PULLUP);  
  
  *my_DDRE &= ~(1 << 4);
  *my_PORTE |= (1 << 4); //enables pull up
  
  //attach the interupt
  attachInterrupt(digitalPinToInterrupt(StartStopButtonPin),StartStopISR,FALLING );
}

//init reset button
void ResetButtonInit()
{
    // Configure pin 36 as input with pull-up like using pinMode
  *my_DDRC  &= ~(1 << PC1_BIT);  // input
  *my_PORTC |=  (1 << PC1_BIT);  // enable pull-up
}

void readResetButton()
{
  //so Professor B says that mills doesnt work here but Micros works as it mesures microseconds imo
    currentTime = micros();

  // Direct hardware read from the reset button
  reading = ((*my_PINC & (1 << PC1_BIT)) ? HIGH : LOW);

  //checks to see if this buttends state is not the same as the last one
  //if so, store the previous timne as the current time and the lastReading as the current reading
  // used in Debouncing
  if (reading != lastReading) {
    previousTime = currentTime;
    lastReading = reading;
  }

  //check the difference in time is greater or equal to the time delay for debouncing and check if the reading is not the same as the stable reading
  if ((currentTime - previousTime) >= RESET_DEBOUNCE_US && reading != stableState) {
    stableState = reading;

    if (stableState == LOW) {
     //Button is pressed now impliment starting logic...
    }
  }
}

  
void CheckifProgramIsOnOrOff()
{
  
  //if Button is Pressed
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
      //turns off the program!
        ProgramStatus = 0;
      //  Serial.println("Program off!");
        break;

  }          
       
  }
  

}

void StartStopISR()
{
  //get the current time
  currentTime = micros();

  //check the difference in time and see if its been after 10 microseconds
  //if so, store the current time into the previous time variable and then invert the startStopButton button events state.
  if ((currentTime - previousTime) >= (STARTSTOP_DEBOUNCE_US)) {
    previousTime = currentTime;   
     StartStopButtonEvent = !StartStopButtonEvent;
  }   

  }


  