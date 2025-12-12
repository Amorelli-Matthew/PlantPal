#include "Serial.h"
#include "WaterSensor.h"
#include "Button.h"
#include "Logging.h"
#include "Temperature.h"
#include "StatusCodes.h"
#include "SoilSensor.h"
#include "LEDControl.h"
#include "StepperMotor.h"
#include "LCDScreen.h"

Status ProgramStatus = DISABLED;
ErrorCodes ErrorCode = NONE;
unsigned long currentTime = 0;

static char timestamp[30];

// Timing variables
unsigned long MainspreviousTime = 0;
unsigned long lastLCDUpdateTime = 0;
const unsigned long SENSOR_DISPLAY_INTERVAL_MS = 60000UL;  // 1 minute
static Status lastDisplayedState = DISABLED;  // Track last displayed state

//prototypes
void CheckifProgramIsOnOrOff(void);
void ErrorHandling(ErrorCodes &ErrorCode);
void pumpOff(void);
void logEvent(const char *msg, const char* timestamp);

void setup(void) {
  // Initialize in this order:
  // 1) Serial operations
  // 2) LCD
  // 3) LEDs
  // 4) Stepper motor
  // 5) Buttons
  // 6) Water sensor / ADC
  // 7) Logging

  U0init(9600);
  
  // Initialize LCD
  lcd.begin(16, 2);
  delay(100);  // Give LCD time to initialize
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PlantPal v1.0");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(500);  // Show initial message briefly
  
  // Initialize LEDs
  LEDInit();
  
  // Initialize Stepper Motor
  StepperMotorInit();
  
  // Initialize Buttons
  StartStopButtonInit();

  // Initialize ADC for sensors
  adc_init();
  
  // Initialize RTC logging
  initLogging();
  
  // Start in DISABLED state
  ProgramStatus = DISABLED;
  ErrorCode = NONE;
  lastDisplayedState = DISABLED;
  
  // Display initial state on LCD
  UpdateLCD("System Disabled");
  
  // Log startup
  getTimeViaRTC(timestamp, sizeof(timestamp));
  logEvent("System initialized - DISABLED state", timestamp);
}

void loop(void) {
  currentTime = millis();
  
  // Handle Start/Stop button flag
  CheckifProgramIsOnOrOff();
  
  // Control LEDs based on state
  LEDControl(ProgramStatus);
  
  // Control stepper motor based on state
  StepperMotorControl(ProgramStatus);
  
  // Run stepper motor steps (non-blocking)
  StepperMotorStep();
  
  // State machine
  switch (ProgramStatus) {
    case DISABLED:
      // Turn off pump if it's on
      if(isPumpOn) {
        pumpOff();
      }
      
      // Display disabled message on LCD (only update if state changed)
      if (lastDisplayedState != DISABLED) {
        UpdateLCD("System Disabled");
        lastDisplayedState = DISABLED;
      }
      break;

    case IDLE:
      // Turn off pump if it's on
      if(isPumpOn) {
        pumpOff();
      }
      
      // Check sensors every minute and update LCD
      if ((currentTime - MainspreviousTime) >= SENSOR_DISPLAY_INTERVAL_MS) {
        MainspreviousTime = currentTime;
        
        // Read temperature and humidity
        ReadTempature();
        float tempC = GetTemperature();
        float humidity = GetHumidity();
        
        // Read soil moisture
        int soilPercent = ReadSoilSensor();
        
        // Update LCD with sensor values
        String statusStr = "IDLE";
        updateLCD(statusStr, tempC, soilPercent);
        
        // Log sensor readings
        getTimeViaRTC(timestamp, sizeof(timestamp));
        char sensorMsg[100];
        snprintf(sensorMsg, sizeof(sensorMsg), "IDLE: Temp=%.1fC Hum=%.0f%% Soil=%d%%", tempC, humidity, soilPercent);
        logEvent(sensorMsg, timestamp);
        
        // Check water level and temperature/humidity sensors
        if(waterlevelcheck() && TempandHumanitySensorCheck()) {
          // Check if soil is dry - if so, transition to RUNNING
          if (isSoilDry()) {
            ProgramStatus = RUNNING;
            ErrorCode = NONE;
            getTimeViaRTC(timestamp, sizeof(timestamp));
            logEvent("State change: IDLE -> RUNNING (soil dry)", timestamp);
          }
        }
      } else {
        // Update LCD periodically even if not time for sensor check
        if ((currentTime - lastLCDUpdateTime) >= 1000UL) {
          lastLCDUpdateTime = currentTime;
          ReadTempature();
          float tempC = GetTemperature();
          int soilPercent = ReadSoilSensor();
          String statusStr = "IDLE";
          updateLCD(statusStr, tempC, soilPercent);
        }
      }
      break;

    case RUNNING:
      // Note: Fan motor is broken, so we skip that functionality
      // But we still need to show RUNNING state
      
      // Check sensors every minute and update LCD
      if ((currentTime - MainspreviousTime) >= SENSOR_DISPLAY_INTERVAL_MS) {
        MainspreviousTime = currentTime;
        
        // Read temperature and humidity
        ReadTempature();
        float tempC = GetTemperature();
        float humidity = GetHumidity();
        
        // Read soil moisture
        int soilPercent = ReadSoilSensor();
        
        // Update LCD with sensor values
        String statusStr = "RUNNING";
        updateLCD(statusStr, tempC, soilPercent);
        
        // Log sensor readings
        getTimeViaRTC(timestamp, sizeof(timestamp));
        char sensorMsg[100];
        snprintf(sensorMsg, sizeof(sensorMsg), "RUNNING: Temp=%.1fC Hum=%.0f%% Soil=%d%%", tempC, humidity, soilPercent);
        logEvent(sensorMsg, timestamp);
        
        // Check water level and temperature/humidity sensors
        if(waterlevelcheck() && TempandHumanitySensorCheck()) {
          // Check if soil is still dry
          if (!isSoilDry()) {
            // Soil is now moist, return to IDLE
            ProgramStatus = IDLE;
            ErrorCode = NONE;
            getTimeViaRTC(timestamp, sizeof(timestamp));
            logEvent("State change: RUNNING -> IDLE (soil moist)", timestamp);
          }
        }
      } else {
        // Update LCD periodically even if not time for sensor check
        if ((currentTime - lastLCDUpdateTime) >= 1000UL) {
          lastLCDUpdateTime = currentTime;
          ReadTempature();
          float tempC = GetTemperature();
          int soilPercent = ReadSoilSensor();
          String statusStr = "RUNNING";
          updateLCD(statusStr, tempC, soilPercent);
        }
      }
      break;

    case ERROR:
      // Make sure pump is off
      if(isPumpOn) {
        pumpOff();
      }
      
      // Display error message on LCD
      ErrorHandling(ErrorCode);
      break;
  }
}

void ErrorHandling(ErrorCodes &ErrorCode) {
  // get the time in order to record the error
  getTimeViaRTC(timestamp, sizeof(timestamp));

  switch (ErrorCode) {
    // 1xx: Sensor & Data Errors
    case ERR_SOIL_ADC_RANGE:
      UpdateLCD("ERROR: Soil", "Sensor out of range");
      logEvent("101: Soil ADC out of range; check wiring", timestamp);
      break;

    case ERR_SOIL_ADC_SATURATED:
      UpdateLCD("ERROR: Soil", "ADC saturated");
      logEvent("102: Soil ADC saturated; stuck at min/max", timestamp);
      break;

    case ERR_DHT_TIMEOUT:
      UpdateLCD("ERROR: DHT", "Read timeout");
      logEvent("111: DHT read timeout; will retry", timestamp);
      break;

    case ERR_DHT_SENSOR_FAULT:
      UpdateLCD("ERROR: DHT", "Hardware fault");
      logEvent("112: DHT hardware fault; check or replace sensor", timestamp);
      break;

    // 2xx: Pump / Motor Errors
    case ERR_PUMP_TIMEOUT:
      UpdateLCD("ERROR: Pump", "Timeout/Low water?");
      logEvent("201: Pump timeout; check pump and water level", timestamp);
      break;

    case ERR_PUMP_NO_CURRENT:
      UpdateLCD("ERROR: Pump", "No current draw");
      logEvent("202: Pump no current; wiring/pump may be disconnected", timestamp);
      break;

    case ERR_PUMP_OVERCURRENT:
      UpdateLCD("ERROR: Pump", "Overcurrent!");
      logEvent("203: Pump overcurrent; possible jam; pump turned OFF", timestamp);
      break;

    // 3xx: Environmental Condition Errors
    case ERR_LOW_WATER:
      UpdateLCD("ERROR: Low Water", "Refill reservoir");
      logEvent("301: Low water; pump disabled until refill", timestamp);
      break;

    case ERR_SOIL_NOT_RECOVERING:
      UpdateLCD("ERROR: Soil", "Not improving");
      logEvent("302: Soil not recovering; check plant/soil", timestamp);
      break;

    // 4xx: System / Logic Errors
    case ERR_RTC_FAILURE:
      UpdateLCD("ERROR: RTC", "Comm failure");
      logEvent("401: RTC failure; timestamps invalid; watering disabled", timestamp);
      break;

    case ERR_INIT_FAILURE:
      UpdateLCD("ERROR: System", "Init failed");
      logEvent("402: Init failed; hardware or config problem", timestamp);
      break;

    case ERR_UNEXPECTED_STATE:
      UpdateLCD("ERROR: Logic", "Unexpected state");
      logEvent("403: Unexpected state; logic bug; system in ERROR", timestamp);
      break;

    case ERR_UNEXPECTED_RESTART:
      UpdateLCD("ERROR", "Unexpected restart");
      logEvent("404: Unexpected restart; power loss", timestamp);
      break;

    default:
      UpdateLCD("ERROR", "Unknown code");
      logEvent("000: Unknown error code", timestamp);
      break;
  }
}
void pumpOff() {
  //method to turn pump off
  isPumpOn = false;
}

void logEvent(const char *msg, const char* timestamp) {
    char buffer[200];  // enough for timestamp + message
    snprintf(buffer, sizeof(buffer), "[%s] %s", timestamp, msg);

    println(buffer);   // ALWAYS one line
}
