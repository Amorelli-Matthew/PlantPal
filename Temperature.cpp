#include "Temperature.h"
#include "StatusCodes.h"
//defines the actual dht object
dht DHT;

int temperature = 0;
int humidity = 0;

// Use a separate timing variable so we don't interfere with button debounce
 unsigned long lastTempCheckTime = 0;

int ReadTempature() {

  currentTime = millis();

  // Only actually hit the sensor every 3 seconds
  if ((currentTime - lastTempCheckTime) >= 3000UL) {
    lastTempCheckTime = currentTime;

    int chk = DHT.read11(DHT11_PIN);

    if (chk == DHTLIB_OK) {
      // store the temp and humidity as ints
      temperature = (int)round(DHT.temperature);
      humidity    = (int)round(DHT.humidity);
    }

    return chk;   // caller decides what to do with error/success
  }

  // No new sample; treat as "no error, keep old values"
  return DHTLIB_OK;
}


void printTempandHumanity(int soil, int temp, int humanity) {
  //create new temp strings to hold lines one and two
  char line1[17];
  char line2[17];

  //format lines 1 and 2 and then add them to each line string
  sprintf(line1, "Soil:%d%%", soil);

  sprintf(line2, "T:%dC H:%d%%", temp, humanity);

  //display the result via lcd
}