#include "Serial.h"
#include "WaterSensor.h"
#include "Button.h"
#include "Logging.h"
#include "Temperature.h"
#include "StatusCodes.h"
#include "SoilSensor.h"
#include "LEDControl.h"
#include "StepperMotor.h"
#include "FanMotor.h"
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

// Press button quickly 3 times to cycle: DISABLED -> IDLE -> RUNNING -> ERROR -> DISABLED
#define ENABLE_STATE_TEST_MODE true  // Set to true to enable test mode
static unsigned long lastButtonPressTime = 0;
static int buttonPressCount = 0;
static const unsigned long BUTTON_PRESS_WINDOW_MS = 2000;  // 2 seconds window for multiple presses

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
  
  // Initialize LCD with proper delays
  initLCDContrast();
  
 // Initialize LCD with proper sequence
  lcd.begin(16, 2);

  
  // Set display properties FIRST (before clearing)
  lcd.display();  // Ensure display is on
  lcd.noCursor();  // Turn off cursor
  lcd.noBlink();   // Turn off blink
  
  // Set initial contrast to 80
  setLCDContrast(80);
  
  // Clear display
  lcd.clear();
  
  // Ensure display is still on after clear
  lcd.display();
   
  // // Show startup message with explicit cursor positioning
  lcd.setCursor(0, 0);
 lcd.print("PlantPal v1.0");
 lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  
  
  // Initialize LEDs
  LEDInit();
 
  // Set initial LED state (DISABLED = Yellow LED)
  LEDControl(ProgramStatus);

  
  // Initialize Stepper Motor
  StepperMotorInit();
  
  // Initialize Fan Motor
  FanMotorInit();
  
  // Initialize Buttons
  StartStopButtonInit();
  ResetButtonInit();

  // Initialize ADC for sensors
  adc_init();
  
  // Initialize RTC logging
  initLogging();
  
  // Start in DISABLED state
  ProgramStatus = DISABLED;
  ErrorCode = NONE;
  lastDisplayedState = DISABLED;
  
  // Log startup
  getTimeViaRTC(timestamp, sizeof(timestamp));
  logEvent("System initialized - DISABLED state", timestamp);

}

void loop(void) {
  currentTime = millis();
  
  // Handle Reset button
  CheckResetButton();
  
  // Handle Start/Stop button flag
  CheckifProgramIsOnOrOff();
  
  // Control LEDs based on state
  LEDControl(ProgramStatus);
  
  // Control stepper motor based on state
  static Status lastStepperState = DISABLED;
  if (ProgramStatus != lastStepperState) {
  
    // State changed - log stepper motor activity
    getTimeViaRTC(timestamp, sizeof(timestamp));
    if (ProgramStatus == DISABLED) {
      logEvent("Stepper motor: STOPPED (Disabled state)", timestamp);
    } else {
      logEvent("Stepper motor: STARTED", timestamp);
    }
    lastStepperState = ProgramStatus;
  }
  StepperMotorControl(ProgramStatus);
  
  // Control fan motor based on state
  static Status lastFanState = DISABLED;
  if (ProgramStatus != lastFanState) {
  
    // State changed - log fan motor activity
    getTimeViaRTC(timestamp, sizeof(timestamp));
    if (ProgramStatus == RUNNING) {
      logEvent("Fan motor: ACTIVATED (Running state)", timestamp);
    } else {
      logEvent("Fan motor: STOPPED", timestamp);
    }
    lastFanState = ProgramStatus;
  }
  FanMotorControl(ProgramStatus);
  
  // Run stepper motor steps (non-blocking)
  StepperMotorStep();
  
  // State machine
  switch (ProgramStatus) {
    case DISABLED:
      // Turn off pump if it's on
      if(isPumpOn) {
        pumpOff();
      }
      
      // Initialize LCD when entering DISABLED state
      if (lastDisplayedState != DISABLED) {
        // Re-initialize LCD to ensure it works
        lcd.begin(16, 2);
       
       lcd.display();  // Turn display on first
        lcd.clear();
       
       // Set contrast to 80 for DISABLED state
        setLCDContrast(80);
       lastDisplayedState = DISABLED;
      }
      
      // Always show "System Disabled" message with sensor data
      lcd.display();  // Turn display on
      setLCDContrast(80);  // Use contrast 80 for DISABLED state
      
      // Refresh LCD less frequently to reduce glitchiness (every 2 seconds instead of 500ms)
      static unsigned long lastLCDRefreshDisabled = 0;
      if ((currentTime - lastLCDRefreshDisabled) > 2000) {  // Refresh every 2 seconds
        lastLCDRefreshDisabled = currentTime;
        // Read sensors and update LCD with data
        ReadTemperature();
        float tempC = GetTemperature();
        int soilPercent = ReadSoilSensor();
        String statusStr = "DISABLED";
        updateLCD(statusStr, tempC, soilPercent);
      }
      break;

    case IDLE:
      // Turn off pump if it's on
      if(isPumpOn) {
        pumpOff();
      }
      
      // Enable stepper motor in IDLE state (use normal speed, not low power)
      // Low power mode was causing issues, so use normal speed
      StepperMotorSetLowPower(false);
      
      // Initialize LCD when entering IDLE state
      if (lastDisplayedState != IDLE) {
        
        // Re-initialize LCD to ensure it works
        lcd.begin(16, 2);
       lcd.display();  // Turn display on first
        lcd.clear();

       // Set contrast - adjust based on voltage and visibility
        setLCDContrast(50);
       lastDisplayedState = IDLE;
      }
      
      // Always show "System IDLE" message with sensor data - refresh continuously to prevent fading
      lcd.display();  // Turn display on
      setLCDContrast(50);  // Ensure contrast is set
      
      // Refresh LCD every 1 second to prevent flicker from frequent clears
      static unsigned long lastLCDRefresh = 0;
      if ((currentTime - lastLCDRefresh) > 1000) {  // Refresh every 1 second
        lastLCDRefresh = currentTime;
        // Read sensors and update LCD with data
        ReadTemperature();
        float tempC = GetTemperature();
        int soilPercent = ReadSoilSensor();
        String statusStr = "IDLE";
        updateLCD(statusStr, tempC, soilPercent);
      }
      
      // Check soil sensor and state transitions every 5 seconds
      static unsigned long lastSoilCheck = 0;
      if ((currentTime - lastSoilCheck) > 5000) 
      {lastSoilCheck = currentTime;
        
        // Check water level and temperature/humidity sensors
        if(waterlevelcheck() && TempandHumanitySensorCheck()) {
          // Read soil moisture
          int soilPercent = ReadSoilSensor();
          
          // Check soil moisture levels and transition states accordingly
          if (isSoilCritical()) {
            // Soil is critical (< 5%) - transition to ERROR
            ProgramStatus = ERROR;
            ErrorCode = ERR_SOIL_NOT_RECOVERING;
            getTimeViaRTC(timestamp, sizeof(timestamp));
            logEvent("State change: IDLE -> ERROR (soil critical < 5%)", timestamp);
          } else if (isSoilDry()) {
            // Soil is dry (< 50%) - transition to RUNNING
            ProgramStatus = RUNNING;
            ErrorCode = NONE;
            getTimeViaRTC(timestamp, sizeof(timestamp));
            logEvent("State change: IDLE -> RUNNING (soil dry < 50%)", timestamp);
          }
        }
      }
      
      // Log sensor readings every minute (less frequent logging)
      if ((currentTime - MainspreviousTime) >= SENSOR_DISPLAY_INTERVAL_MS) {
        MainspreviousTime = currentTime;
        
        // Read temperature and humidity
        ReadTemperature();
        float tempC = GetTemperature();
        float humidity = GetHumidity();
        
        // Read soil moisture
        int soilPercent = ReadSoilSensor();
        
        // Log sensor readings
        getTimeViaRTC(timestamp, sizeof(timestamp));
        char sensorMsg[100];
        snprintf(sensorMsg, sizeof(sensorMsg), "IDLE: Temp=%.1fC Hum=%.0f%% Soil=%d%%", tempC, humidity, soilPercent);
        logEvent(sensorMsg, timestamp);
      }
      break;

    case RUNNING:
      // Disable low power mode in RUNNING state (use full speed)
      StepperMotorSetLowPower(false);
      
      // Update LCD immediately when entering RUNNING state
      if (lastDisplayedState != RUNNING) {
        // Re-initialize LCD to ensure it works
        lcd.begin(16, 2);
        
        lcd.display();  // Turn display on first
        lcd.clear();
        
        // Set contrast - adjust based on voltage and visibility
        setLCDContrast(50);
        
        // Read sensors and update LCD immediately
        ReadTemperature();
        float tempC = GetTemperature();
        int soilPercent = ReadSoilSensor();
        
        String statusStr = "RUNNING";
        updateLCD(statusStr, tempC, soilPercent);
        
        lastDisplayedState = RUNNING;
        lastLCDUpdateTime = currentTime;  // Reset update timer
      }
      
      // Always show "RUNNING" message - refresh continuously to prevent fading
      lcd.display();  // Turn display on
      setLCDContrast(50);  // Ensure contrast is set
      
      // Refresh LCD every 1 second to prevent flicker from frequent clears
      static unsigned long lastLCDRefreshRunning = 0;
      if ((currentTime - lastLCDRefreshRunning) > 1000) {  // Refresh every 1 second
        lastLCDRefreshRunning = currentTime;
        
        // Read sensors and update LCD
        ReadTemperature();
        float tempC = GetTemperature();
        int soilPercent = ReadSoilSensor();
        
        String statusStr = "RUNNING";
        updateLCD(statusStr, tempC, soilPercent);
      }
      
      // Check soil sensor and state transitions every 5 seconds (faster response)
      static unsigned long lastSoilCheckRunning = 0;
      if ((currentTime - lastSoilCheckRunning) > 5000) {
        lastSoilCheckRunning = currentTime;
        
        // Check water level and temperature/humidity sensors
        if(waterlevelcheck() && TempandHumanitySensorCheck()) {
          // Read soil moisture
          int soilPercent = ReadSoilSensor();
          
          // Check soil moisture levels and transition states accordingly
          if (isSoilCritical()) {
            // Soil is critical (< 5%) - transition to ERROR
            ProgramStatus = ERROR;
            ErrorCode = ERR_SOIL_NOT_RECOVERING;
        
            getTimeViaRTC(timestamp, sizeof(timestamp));
            logEvent("State change: RUNNING -> ERROR (soil critical < 5%)", timestamp);
          } else if (!isSoilDry()) {

            // Soil is now moist (>= 50%) - return to IDLE
            ProgramStatus = IDLE;
            ErrorCode = NONE;
            getTimeViaRTC(timestamp, sizeof(timestamp));
            logEvent("State change: RUNNING -> IDLE (soil moist >= 50%)", timestamp);
          }
          // If soil is between 5% and 50%, stay in RUNNING
        }
      }
      
      // Log sensor readings every minute (less frequent logging)
      if ((currentTime - MainspreviousTime) >= SENSOR_DISPLAY_INTERVAL_MS) {
        MainspreviousTime = currentTime;
        
        // Read temperature and humidity
        ReadTemperature();
        float tempC = GetTemperature();
        float humidity = GetHumidity();
        
        // Read soil moisture
        int soilPercent = ReadSoilSensor();
        
        // Log sensor readings
        getTimeViaRTC(timestamp, sizeof(timestamp));
        char sensorMsg[100];
        snprintf(sensorMsg, sizeof(sensorMsg), "RUNNING: Temp=%.1fC Hum=%.0f%% Soil=%d%%", tempC, humidity, soilPercent);
        logEvent(sensorMsg, timestamp);
      }
      break;

    case ERROR:
      // Make sure pump is off
      if(isPumpOn) {
        pumpOff();
      }
      
      // Initialize LCD when entering ERROR state
      if (lastDisplayedState != ERROR) {
        
        // Re-initialize LCD to ensure it works (same as IDLE)
        lcd.begin(16, 2);
        
        lcd.display();  // Turn display on first
        lcd.clear();

        // Set contrast - adjust based on voltage and visibility
        setLCDContrast(50);

        // Immediately show ERROR message after initialization
        ReadTemperature();
        float tempC = GetTemperature();
        int soilPercent = ReadSoilSensor();

        String statusStr = "ERROR";
        updateLCD(statusStr, tempC, soilPercent);
        lastDisplayedState = ERROR;
      }
      
      // Always show "ERROR" message with sensor data - refresh continuously to prevent fading
      lcd.display();  // Turn display on
      setLCDContrast(50);  // Ensure contrast is set
      
      // Refresh LCD every 1 second to prevent flicker from frequent clears
      static unsigned long lastLCDRefreshError = 0;
      if ((currentTime - lastLCDRefreshError) > 1000) {  // Refresh every 1 second
        lastLCDRefreshError = currentTime;

       // Read sensors and update LCD with data (same as IDLE state)
        ReadTemperature();
        float tempC = GetTemperature();
        int soilPercent = ReadSoilSensor();

        String statusStr = "ERROR";
        updateLCD(statusStr, tempC, soilPercent);
      }
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
