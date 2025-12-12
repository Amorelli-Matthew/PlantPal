#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "StatusCodes.h"

// LED pins: Yellow 11, Green 10, Red 12, Blue 13
// Arduino Mega 2560 pin mappings:
// Pin 10 = PB4 (Port B, bit 4)
// Pin 11 = PB5 (Port B, bit 5)
// Pin 12 = PB6 (Port B, bit 6)
// Pin 13 = PB7 (Port B, bit 7)

// Port B registers
extern volatile unsigned char *my_DDRB;
extern volatile unsigned char *my_PORTB;

void LEDInit(void);
void LEDControl(Status state);

#endif

