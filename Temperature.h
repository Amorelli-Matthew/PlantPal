#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#define DHT11_PIN 7
#include "LCDScreen.h"
#include "StatusCodes.h"
#include <dht.h> //install the DHTLib library
#include <math.h>

//grab the dht.h lib from 
//https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/

static dht DHT;
static int temperature = 0;
static int humidity = 0;

//store the last tempatures checked time
static unsigned long lastTempCheckTime = 0;
extern unsigned long currentTime;

int ReadTempature();
bool TempandHumanitySensorCheck();

#endif