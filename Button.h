#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "Serial.h"
#include "StatusCodes.h"

#define StartStopButtonPin    18  // PD3 on Arduino Mega 2560 (interrupt-capable)
#define ResetButtonPin        14  // PJ1 on Arduino Mega 2560 (polling method - not interrupt-capable)
#define STARTSTOP_DEBOUNCE_US 1500000UL

extern volatile bool StartStopButtonEvent;

extern volatile unsigned char *my_DDRD;
extern volatile unsigned char *my_PORTD;
extern volatile unsigned char *my_PIND;
extern volatile unsigned char *my_DDRJ;
extern volatile unsigned char *my_PORTJ;
extern volatile unsigned char *my_PINJ;

void StartStopButtonInit();
void ResetButtonInit();
void StartStopISR();
void CheckifProgramIsOnOrOff();
void CheckResetButton();  // Uses polling instead of interrupts

// Forward declaration for logging
void logEvent(const char *msg, const char* timestamp);



#endif
