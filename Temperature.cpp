#include "Temperature.h"
#include "WaterSensor.h"  // Use ADC functions from WaterSensor

int ReadTempature() {
  currentTime = millis();

  // Only update timing every 3 seconds (for consistency with original code)
  if ((currentTime - lastTempCheckTime) >= 3000UL) {
    lastTempCheckTime = currentTime;
    
    // Read humidity sensor from A4 (ADC channel 4)
    unsigned int sum = 0;
    for (int i = 0; i < 8; i++) {
      sum += adc_read(4);  // A4 = ADC channel 4
    }
    unsigned int avgValue = sum / 8;
    
    // Convert ADC value (0-1023) to humidity percentage (0-100%)
    // Adjust this conversion based on your sensor's characteristics
    // Typical range: 0-1023 maps to 0-100% humidity
    humidity = (avgValue * 100.0) / 1023.0;
    
    // Clamp humidity to valid range (0-100%)
    if (humidity > 100.0) humidity = 100.0;
    if (humidity < 0.0) humidity = 0.0;
    
    // Temperature still using stub value (20.0°C) until we have a sensor
    // You can add temperature reading here if you have a sensor
  }

  // Return success code
  return TEMP_READ_SUCCESS;
}

bool TempandHumanitySensorCheck() {
  // DHT library removed - always return true with stub values
  int status = ReadTempature();

  // Check if status indicates an error
  if (status != TEMP_READ_SUCCESS) {
    int tempIntTemp = (int) temperature;
    int TempIntHum = (int) humidity;

    bool tempInvalid = (tempIntTemp < 0 || tempIntTemp > 50);
    bool humidityInvalid = (TempIntHum < 20 || TempIntHum > 90);

    if (tempInvalid || humidityInvalid) {
      ProgramStatus = ERROR;
      ErrorCode = ERR_DHT_SENSOR_FAULT;
      return false;
    }
  }
  
  // With stub values, always return true
  return true;
}

float GetTemperature(void) {
  return temperature;
}

float GetHumidity(void) {
  return humidity;
}


