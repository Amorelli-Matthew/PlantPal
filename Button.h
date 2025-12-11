#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "Serial.h"
#include "StatusCodes.h"

#define StartStopButtonPin    2
#define STARTSTOP_DEBOUNCE_US 1500000UL

#define RESET_DEBOUNCE_US     50000UL     // 50 ms debounce
#define PC1_BIT               1          // bit 1 = pin 36

extern volatile bool StartStopButtonEvent;
//extern volatile bool ProgramStatus;

extern volatile unsigned char *my_DDRE;
extern volatile unsigned char *my_PORTE;
extern volatile unsigned char* my_PINC;
extern volatile unsigned char* my_DDRC;
extern volatile unsigned char* my_PORTC;

// shared debounce state for reset button
extern unsigned long btnpreviousTime;
extern unsigned long btncurrentTime;
extern int lastReading;
extern int stableState;
extern int reading;

void StartStopButtonInit();
void StartStopISR();
void CheckifProgramIsOnOrOff();

void ResetButtonInit();
void readResetButton();



#endif
