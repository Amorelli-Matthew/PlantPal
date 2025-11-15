#ifndef BUTTON_H
#define BUTTON_H
//for handling buttons and their logic

#include <Arduino.h>
#include "Serial.h"


///we declare the variables first, Then init themn in the resective cpp file

extern unsigned long previousTime;   // when the raw reading last flipped
extern unsigned long currentTime;

//vars for the last reading
extern int lastReading;
extern int stableState;
extern int reading;

extern volatile bool StartStopButtonEvent;    
extern volatile bool ProgramStatus; 

#ifdef __cplusplus
extern "C" {
#endif

#define StartStopButtonPin 2
#define STARTSTOP_DEBOUNCE_US 1500000

#define RESET_DEBOUNCE_US 50000UL  // 50 ms debounce time
#define PC1_BIT 1             // bit 1 = pin 36
         

extern volatile unsigned char *my_DDRE;  // DDR for Port E for starting stopping program button, this is for pin 2
extern volatile unsigned char *my_PORTE;  // PORT for Port E. 

extern volatile unsigned char*  my_PINC; //for the reset button for resetting the program, pin 36
extern volatile unsigned char*  my_DDRC;
extern volatile unsigned char*  my_PORTC;

//for the buttons start and stop
void StartStopButtonInit();
void StartStopISR();
void CheckifProgramIsOnOrOff();

//init reset button
void ResetButtonInit();

//logic for what the resetbutton does...ie resetting the program
void readResetButton();



#ifdef __cplusplus
}
#endif

#endif