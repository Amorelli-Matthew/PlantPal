#ifndef TEMPERATURE_H
#define TEMPERATURE_H
#define DHT11_PIN 8  // Humidity sensor pin (changed from 7 to 8)
#include "LCDScreen.h"
#include "StatusCodes.h"
#include <math.h>

// DHT library removed - temperature/humidity sensor not available
// Using stub values for compatibility

#define TEMP_READ_SUCCESS 0  // Success code (replaces DHTLIB_OK)

static float temperature = 20.0;  // Default temperature in Celsius
static float humidity = 50.0;     // Default humidity percentage

//store the last tempatures checked time
static unsigned long lastTempCheckTime = 0;
extern unsigned long currentTime;

int ReadTempature();
bool TempandHumanitySensorCheck();
float GetTemperature(void);
float GetHumidity(void);

#endif