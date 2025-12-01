#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "Serial.h"
#include "StatusCodes.h"
//use the pins 2 and 36 for the startandstop button and the reset button respectively

#define StartStopButtonPin 2
#define STARTSTOP_DEBOUNCE_US 3000000  //17 sec debounce time
#define RESET_DEBOUNCE_US 50000UL      // 50 ms debounce time

///we declare the variables first, Then init them in the resective cpp file
// the extern then variable decloration tells the compiler that these variables are used in another file
//the cpp file.

extern unsigned long previousTime;
extern unsigned long currentTime;

//vars for the last reading of startstopbutton
extern int lastReading;
extern int stableState;
extern int reading;

//bool to keep track of when the button was pressed
extern volatile bool StartStopButtonEvent;

extern volatile unsigned char* my_DDRE;   // DDR for Port E for starting stopping program button, this is for pin 2
extern volatile unsigned char* my_PORTE;  // PORT for Port E.

extern volatile unsigned char* my_PINC;  //for the reset button for resetting the program, pin 36
extern volatile unsigned char* my_DDRC;
extern volatile unsigned char* my_PORTC;

//for the button start and stop
void StartStopButtonInit();
void StartStopISR();

//init reset button
void ResetButtonInit();

#endif
