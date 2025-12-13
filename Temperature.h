#ifndef TEMPERATURE_H
#define TEMPERATURE_H

//Temperature TEMPERATURE_H
#define DHT11_PIN 8  // Humidity sensor pin (changed from 7 to 8)
#include "LCDScreen.h"
#include "StatusCodes.h"
#include <math.h>
#include <dht.h>
#include "Serial.h"

#define DHT11_PIN 8
#define TEMP_READ_INTERVAL_MS 3000UL
#define TEMP_READ_SUCCESS 0

static dht DHT;
static float temperature = 20.0;  // Default temperature in Celsius
static float humidity = 50.0;     // Default humidity percentage

//store the last tempatures checked time
static unsigned long lastTempCheckTime = 0;
extern unsigned long currentTime;
static bool hasValidSample = false;

void ReadTemperature();
bool TempandHumanitySensorCheck();
float GetTemperature(void);
float GetHumidity(void);

#endif