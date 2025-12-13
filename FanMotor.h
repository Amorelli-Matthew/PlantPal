#ifndef FANMOTOR_H
#define FANMOTOR_H

#include <Arduino.h>
#include "StatusCodes.h"

// Fan motor pin: 31
// Using digitalWrite/pinMode (same as test code that worked)

void FanMotorInit(void);
void FanMotorControl(Status state);

#endif

