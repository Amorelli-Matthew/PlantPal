#ifndef FANMOTOR_H
#define FANMOTOR_H

#include <Arduino.h>
#include "StatusCodes.h"

// Fan motor pin: 32
// Arduino Mega 2560 pin mapping:
// Pin 32 = PC7 (Port C, bit 7)

// Port C registers
extern volatile unsigned char *my_DDRC;
extern volatile unsigned char *my_PORTC;

void FanMotorInit(void);
void FanMotorControl(Status state);

#endif

