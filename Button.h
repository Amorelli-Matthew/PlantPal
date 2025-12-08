#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>
#include "Serial.h"
#include "StatusCodes.h"
//use the pins 2 and 36 for the startandstop button and the reset button respectively

#define StartStopButtonPin 2
#define STARTSTOP_DEBOUNCE_US 20000UL   // 20 ms debounce
#define RESET_DEBOUNCE_US     20000UL   // 20 ms debounce


//to keep track of time
extern volatile unsigned long startStopPreviousTime;
extern volatile unsigned long currentButtonTime;
extern volatile unsigned long resetPreviousTime;

//vars for the last reading of startstopbutton
extern volatile int lastReading;
extern volatile int stableState;
extern volatile int reading;

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
void readResetButton(void);

#endif
