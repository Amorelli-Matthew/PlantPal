#include "Temperature.h"

//defines the actual dht object
dht DHT;

// Use a separate timing variable so we don't interfere with button debounce
static unsigned long lastWaterCheckTime = 0;
static unsigned long currentTime = 0;
void READANDDISPLAYSENSOR() {

  // Get the current time
  currentTime = millis();

  // Read the water every few seconds
  if ((currentTime - lastWaterCheckTime) >= 3000UL) {
    lastWaterCheckTime = currentTime;
    int chk = DHT.read11(DHT11_PIN);
    print("Temperature = ");
    println(DHT.temperature, 2);

    print("Humidity = ");
    println(DHT.humidity, 2);
  }
}