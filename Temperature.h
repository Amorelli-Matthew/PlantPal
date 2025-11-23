#ifndef TEMPATURE_H
#define TEMPATURE_H
#define DHT11_PIN 7
#include <dht.h> //install the DHTLib library
#include "Serial.h"

//grab the dht.h lib from 
//https://www.circuitbasics.com/how-to-set-up-the-dht11-humidity-sensor-on-an-arduino/

extern dht DHT;

void READANDDISPLAYSENSOR();





#endif