#ifndef FANMOTOR_H
#define FANMOTOR_H

#include <Arduino.h>
#include "StatusCodes.h"
#include "Serial.h"
#include "Logging.h"
#include "Serial.h"
#include "Logging.h"

// Fan motor pin: 31

void FanMotorInit(void);
void FanMotorControl(Status state);

#endif

