#include "Serial.h"
#include "WaterSensor.h"
#include "Button.h"
#include "Logging.h"
#include "Temperature.h"
#include "StatusCodes.h"
#include "SoilSensor.h"

Status ProgramStatus = DISABLED;
ErrorCodes ErrorCode = NONE;
unsigned long currentTime = 0;

//prototypes
void CheckifProgramIsOnOrOff(void);
void readResetButton(void);
void ledStatus(void);
void updateLCDAndLog(void);
void ErrorHandling(ErrorCodes &ErrorCode);


void setup(void) {
  // Initialize in this order:
  // 1) Serial operations
  // 3) Buttons
  // 4) Water sensor / ADC
  // 5) Logging

  U0init(9600);

  StartStopButtonInit();
  ResetButtonInit();
  adc_init();
  initLogging();
  //put soil sensors init here
  ProgramStatus = IDLE;  //put as DISABLED in final Program
  ErrorCode = NONE;
}

unsigned long MainspreviousTime = 0;
void loop(void) {
  CheckifProgramIsOnOrOff();  // Handle Start/Stop button flag

  readResetButton();  // Handle Reset button

  // Reflect state on LEDs
  ledStatus();

  switch (ProgramStatus) {
    case DISABLED:
      //sensors off
      //print to lcd: system disabled
    //check if pump is still on and if so, turn it off
     if(isPumpOn)
     {
       pumpOff();
     }
      UpdateLCD("System Disabled");
      break;

    case IDLE:
    
    //check if pump is still on and if so, turn it off
     if(isPumpOn)
     {
       pumpOff();
     }

       currentTime = millis();
      //check for every minute
      if ((currentTime - MainspreviousTime) >= 60000) {
        MainspreviousTime = currentTime;

        waterlevelcheck();
        TempandHumanitySensorCheck();

        // Later: soil moisture check & pump control can be added here
        if (false)  // if (isSoilDry()) uncommit once method is written
          //if all of the sensors report correct readings then change program status to running
          ProgramStatus = RUNNING;
        else
          updateLCDAndLog();
      }
     

      break;

    case RUNNING:
      //run water and other logic
      //  simple methods for pumping water from waterpump
      //water for 3 to 5 seconds at a time untill thw water pump is good

      //after the plant is watered, return back to program IDLE
      ProgramStatus = IDLE;
      break;

    case ERROR:
      //for error handling
      //check what error message it is
      //print error message
      //make sure pump is off
      //pumpOff();
      ErrorHandling(ErrorCode);
      break;
  }
}

// Checks if Start/Stop button event occurred and toggles system state
void CheckifProgramIsOnOrOff(void) {

  // If button has been pressed (flag set by button ISR)
  if (StartStopButtonEvent) {
    // Reset StartStopButtonEvent and previousTime back to zero
    StartStopButtonEvent = 0;
    startStopPreviousTime = 0;

    // Check current system state
    switch (ProgramStatus) {
      case DISABLED:
        // Starts the program
        ProgramStatus = IDLE;
        println("Program on!");
        break;

      case IDLE:
      case RUNNING:
      case ERROR:
        // Turns off the program from any active state
        ProgramStatus = DISABLED;
        ErrorCode = NONE;
        println("Program off!");
        break;
    }
  }
}


// Display LED status based on current ProgramStatus
void ledStatus(void) {
  switch (ProgramStatus) {
    case DISABLED:
      // Yellow LED on, others off
      break;

    case IDLE:
      // Green LED on, others off
      break;

    case RUNNING:
      // Blue LED on, others off
      break;

    case ERROR:
      // Red LED on, others off
      break;
  }
}

void updateLCDAndLog() {
  //updateLCD(status,temperature,soilPercent);
  //logMethod();
  RTCTest();
}

void ErrorHandling(ErrorCodes &ErrorCode) {

  switch (ErrorCode) {
    // 1xx: Sensor & Data Errors
    case ERR_SOIL_ADC_RANGE:
      println("101: Soil ADC out of expected range");
      UpdateLCD("ERROR: Soil", "Sensor out of range");
      logEvent("101: Soil ADC out of expected range: check wiring, then press RESET");
      // User: fix wiring, then press reset to try to return to IDLE
      break;

    case ERR_SOIL_ADC_SATURATED:
      println("102: Soil ADC saturated");
      UpdateLCD("ERROR: Soil", "ADC saturated");
      logEvent("102: Soil ADC saturated – sensor stuck at min/max");
      // Pump already off; condition must clear before reset succeeds
      break;

    case ERR_DHT_TIMEOUT:
      println("111: DHT sensor timed out");
      UpdateLCD("ERROR: DHT", "Read timeout");
      logEvent("111: DHT timeout – will retry periodically; press RESET after it recovers");
      // Main loop in ERROR state can retry DHT read every minute
      break;

    case ERR_DHT_SENSOR_FAULT:
      println("112: DHT sensor hardware fault");
      UpdateLCD("ERROR: DHT", "Hardware fault");
      logEvent("112: DHT hardware fault – check/replace sensor");
      // Pump remains off until sensor is healthy and user resets
      break;

    // 2xx: Pump / Motor Errors
    case ERR_PUMP_TIMEOUT:
      println("201: Pump timeout, check waterpump");
      UpdateLCD("ERROR: Pump", "Timeout/Low water?");
      logEvent("201: Pump timeout – stopped to protect hardware; check pump & water level");
      // Pump already forced off at top
      break;

    case ERR_PUMP_NO_CURRENT:
      println("202: Pump no current draw");
      UpdateLCD("ERROR: Pump", "No current draw");
      logEvent("202: Pump no current – wiring or pump disconnected");
      break;

    case ERR_PUMP_OVERCURRENT:
      println("203: Pump overcurrent!");
      UpdateLCD("ERROR: Pump", "Overcurrent!");
      logEvent("203: Pump overcurrent – possible jam; pump turned OFF");
      break;

    // 3xx: Environmental Condition Errors
    case ERR_LOW_WATER:
      println("301: Low water level");
      UpdateLCD("ERROR: Low Water", "Refill reservoir");
      logEvent("301: Low water – pump disabled until reservoir refilled and RESET pressed");
      break;

    case ERR_SOIL_NOT_RECOVERING:
      println("302: Soil moisture not improving");
      UpdateLCD("ERROR: Soil", "Not improving");
      logEvent("302: Soil not recovering – ask user to check plant/soil then press RESET");
      // Suggest to user: Please press reset after fixing the issue
      break;

    // 4xx: System / Logic Errors
    case ERR_RTC_FAILURE:
      println("401: RTC communication failure");
      UpdateLCD("ERROR: RTC", "Comm failure");
      logEvent("401: RTC failure – timestamps may be invalid; watering disabled");
      break;

    case ERR_INIT_FAILURE:
      println("402: System initialization failed");
      UpdateLCD("ERROR: System", "Init failed");
      logEvent("402: Initialization failed – hardware problem at startup");
      break;

    case ERR_UNEXPECTED_STATE:
      UpdateLCD("ERROR: Logic", "Unexpected state");
    logEvent("403: Unexpected state – logic bug, system locked in ERROR");
      break;

    case ERR_UNEXPECTED_RESTART:
      UpdateLCD("ERROR", "Unexpected restart");
      logEvent("404: Unexpected restart – power loss or reset; user intervention required");
      break;

    default:
      println("Unknown error code!");
      UpdateLCD("ERROR", "Unknown code");
      break;
  }
}

void pumpOff() {
  //method to turn pumpoff
  isPumpOn = false;
}


void logEvent(const char *msg) {
  // Use your Logging.h + RTC:
  // timestamp + msg over Serial and/or SD/EEPROM
  // e.g., logWithTimestamp(msg);
}
