#include "Temperature.h"

int ReadTempature() {

  currentTime = millis();

  // Only actually hit the sensor every 3 seconds
  if ((currentTime - lastTempCheckTime) >= 3000UL) {
    lastTempCheckTime = currentTime;

    int chk = DHT.read11(DHT11_PIN);

    if (chk == DHTLIB_OK) {
      // store the temp and humidity
      temperature = round(DHT.temperature);
      humidity    = round(DHT.humidity);
    }

    return chk;   // caller decides what to do with error/success
  }

  // No new sample; treat as "no error, keep old values"
  return DHTLIB_OK;
}

bool TempandHumanitySensorCheck() {

  int status = ReadTempature();

  if (status != DHTLIB_OK) {

    int tempIntTemp =(int) temperature;
    int TempIntHum = (int) humidity;

    bool tempInvalid = (tempIntTemp < 0 || tempIntTemp > 50);
    bool humidityInvalid = (TempIntHum < 20 || TempIntHum > 90);

    if (tempInvalid || humidityInvalid) {
      ProgramStatus = ERROR;
      ErrorCode = ERR_DHT_SENSOR_FAULT;

      return false;
    }
  }

  return true;
}


