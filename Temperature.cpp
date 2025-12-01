#include "Temperature.h"

//defines the actual dht object
dht DHT;

int temperature = 0;
int humidity = 0;

// Use a separate timing variable so we don't interfere with button debounce
static unsigned long lastWaterCheckTime = 0;
static unsigned long currentTime = 0;
void ReadTempature() {

  // Get the current time
  currentTime = millis();

  // Read the water every few seconds
  if ((currentTime - lastWaterCheckTime) >= 3000UL) {
    lastWaterCheckTime = currentTime;
    int chk = DHT.read11(DHT11_PIN);
  
  //store the temp and humidity as ints
 temperature = (int) (round(DHT.temperature));
  humidity = (int) (round(DHT.humidity));
  }
}

void printTempandHumanity(int soil, int temp, int humanity)
{
  //create new temp strings to hold lines one and two
  char line1[17];
  char line2[17];

 //format lines 1 and 2 and then add them to each line string
  sprintf(line1, "Soil:%d%%", soil);

  sprintf(line2,"T:%dC H:%d%%", temp, humanity);

  //display the printed result via lcd

}