#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "StatusCodes.h"

// LED pins: Yellow 25, Green 24, Red 26, Blue 12
// Arduino Mega 2560 pin mappings:
// Pin 24 = PA2 (Port A, bit 2)
// Pin 25 = PA3 (Port A, bit 3)
// Pin 26 = PA4 (Port A, bit 4)
// Pin 12 = PB6 (Port B, bit 6)

// Port A registers (for Green, Yellow, Red LEDs)
extern volatile unsigned char *my_DDRA;
extern volatile unsigned char *my_PORTA;

// Port B registers (for Blue LED)
extern volatile unsigned char *my_DDRB;
extern volatile unsigned char *my_PORTB;

void LEDInit(void);
void LEDControl(Status state);

#endif

