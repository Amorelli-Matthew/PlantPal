#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

#include <Arduino.h>
#include "StatusCodes.h"

// Stepper motor pins: 41, 43, 40, 42
// Arduino Mega 2560 pin mappings:
// Pin 40 = PG1 (Port G, bit 1)
// Pin 41 = PG0 (Port G, bit 0)
// Pin 42 = PL7 (Port L, bit 7)
// Pin 43 = PL6 (Port L, bit 6)

// Port G and L registers
extern volatile unsigned char *my_DDRG;
extern volatile unsigned char *my_PORTG;
extern volatile unsigned char *my_DDRL;
extern volatile unsigned char *my_PORTL;

void StepperMotorInit(void);
void StepperMotorControl(Status state);
void StepperMotorStep(void);

// Enable/disable low power mode
void StepperMotorSetLowPower(bool enable); 
#endif

