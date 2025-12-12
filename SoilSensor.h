#ifndef SOILSENSOR_H
#define SOILSENSOR_H
#include "StatusCodes.h"
extern bool isPumpOn;

int ReadSoilSensor(void);
bool isSoilDry();  // Returns true if soil < 50% (triggers RUNNING)
bool isSoilCritical();  // Returns true if soil < 5% (triggers ERROR)
//void water plant method

#endif