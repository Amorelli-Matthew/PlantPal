#include "Temperature.h"

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

bool TempandHumanitySensorCheck() {

  int status = ReadTempature();

  if (status != DHTLIB_OK) {

    bool tempInvalid = (temperature < 0 || temperature > 50);
    bool humidityInvalid = (humidity < 20 || humidity > 90);

    if (tempInvalid || humidityInvalid) {
      ProgramStatus = ERROR;
      ErrorCode = ERR_DHT_SENSOR_FAULT;

      return false;
    }
  }

  return true;
}


