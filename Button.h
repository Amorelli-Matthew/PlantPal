#ifndef BUTTON_H
#define BUTTON_H
//for handling buttons and their logic

#include <Arduino.h>
#include "Serial.h"
#ifdef __cplusplus
extern "C" {
#endif

#define StartStopButtonPin 2
#define STARTSTOP_DEBOUNCE_US 1500000

#define RESET_DEBOUNCE_US 50000UL  // 50 ms debounce
#define PC1_BIT 1             // bit 1 = pin 36

// debouncer state

extern volatile bool StartStopButtonEvent;    
extern volatile bool ProgramStatus;           

extern volatile unsigned char *my_DDRE;  // DDR for Port E for starting stopping program button
extern volatile unsigned char *my_PORTE;  // PORT for Port E. 
extern volatile unsigned char*  my_PINC;
extern volatile unsigned char*  my_DDRC;
extern volatile unsigned char*  my_PORTC;

unsigned long previousTime = 0;   // when the raw reading last flipped
unsigned long currentTime;
int lastReading  = HIGH;
int stableState  = HIGH;
int reading;

//for the buttons start and stop
void StartStopButtonInit();
void StartStopISR();
void CheckifProgramIsOnOrOff();


void ResetButtonInit();
void readResetButton();



#ifdef __cplusplus
}
#endif

#endif