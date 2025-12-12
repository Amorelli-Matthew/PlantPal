#ifndef SOILSENSOR_H
#define SOILSENSOR_H
#include "StatusCodes.h"
extern bool isPumpOn;

int ReadSoilSensor(void);
bool isSoilDry();
//void water plant method

#endif