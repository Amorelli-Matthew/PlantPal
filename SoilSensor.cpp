#include "SoilSensor.h"
#include "WaterSensor.h"  // Use the ADC functions from WaterSensor

bool isPumpOn = false;

// Read soil moisture sensor from A2 (ADC channel 2)
// Returns soil moisture percentage (0-100%)
int ReadSoilSensor(void) {
  unsigned int sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += adc_read(2);  // A2 = ADC channel 2
  }
  unsigned int avgValue = sum / 8;
  
  // Convert ADC value (0-1023) to percentage
  // Typical soil sensor: higher value = more moisture
  // Adjust these thresholds based on your sensor calibration
  int soilPercent = (avgValue * 100) / 1023;
  
  // Clamp to 0-100%
  if (soilPercent > 100) soilPercent = 100;
  if (soilPercent < 0) soilPercent = 0;
  
  return soilPercent;
}

bool isSoilDry()
{
  int soilPercent = ReadSoilSensor();
  // Consider soil dry if moisture is below 30%
  // Adjust threshold as needed
  return (soilPercent < 30);
}