#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "Serial.h"
#include "StatusCodes.h"

#define StartStopButtonPin    18  // PD3 on Arduino Mega 2560
#define STARTSTOP_DEBOUNCE_US 1500000UL

extern volatile bool StartStopButtonEvent;
//extern volatile bool ProgramStatus;

extern volatile unsigned char *my_DDRD;
extern volatile unsigned char *my_PORTD;
extern volatile unsigned char *my_PIND;

void StartStopButtonInit();
void StartStopISR();
void CheckifProgramIsOnOrOff();

// Forward declaration for logging
void logEvent(const char *msg, const char* timestamp);



#endif
