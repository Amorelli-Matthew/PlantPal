#include "Temperature.h"

void ReadTemperature() {
  unsigned long now = millis();
  if ((now - lastTempCheckTime) < TEMP_READ_INTERVAL_MS) return;
  lastTempCheckTime = now;

  // SIM NOTE: use read11 for the DHT11 in your simulator
  int chk = DHT.read11(DHT11_PIN);

  if (chk == TEMP_READ_SUCCESS) {
    temperature     = roundf(DHT.temperature);
    humidity        = roundf(DHT.humidity);
    hasValidSample  = true;
  } else {
    print("DHT read failed, code=");
    println(chk);
  }
}

bool TempandHumanitySensorCheck() {
  ReadTemperature();

  if (!hasValidSample) return false; // don't judge until we have real data

  int t = (int)temperature;
  int h = (int)humidity;

  bool tempInvalid = (t < 0 || t > 50);
  bool humInvalid  = (h < 20 || h > 90);

  if (tempInvalid || humInvalid) {
   println("ERROR HERE");

    // If water level low, then set ProgramStatus to ERROR and set the error status to low water
    ProgramStatus = ERROR;
    ErrorCode = ERR_LOW_WATER;

    return false;
  }

  return true;
}

float GetTemperature(void)
{
  return temperature;
}

float GetHumidity(void)
{
  return humidity;
}
