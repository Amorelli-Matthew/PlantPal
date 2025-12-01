#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#define DHT11_PIN 7
#include <dht.h> //install the DHTLib library
#include "LCDScreen.h"
#include <math.h>

//grab the dht.h lib from 
//https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/

extern dht DHT;
extern int temperature;
extern int humidity;

//store the last tempatures checked time
extern unsigned long lastTempCheckTime;
extern unsigned long currentTime;

int ReadTempature();
void printTempandHumanity(int s, int t, int d);
void TempandHumanitySensorCheck();

#endif