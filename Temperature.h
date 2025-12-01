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

// Use a separate timing variable so we don't interfere with button debounce
extern unsigned long lastTempCheckTime;
extern unsigned long currentTime;

int ReadTempature();
void printTempandHumanity(int s, int t, int d);


#endif