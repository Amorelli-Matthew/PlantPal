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

static char timestamp[30];

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
 // CheckifProgramIsOnOrOff();  // Handle Start/Stop button flag

// readResetButton();  // Handle Reset button

  // Reflect state on LEDs
//  ledStatus();
/*
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

       if(waterlevelcheck() && TempandHumanitySensorCheck())
        {
        // Later: soil moisture check & pump control can be added here
        if (false)  // if (isSoilDry()) uncommit once method is written
          //if all of the sensors report correct readings then change program status to running
          ProgramStatus = RUNNING;
        else
          println("none");
          
         // Println(const String& status, float tempC, int soilPercent);

        }

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
*/}

/*

// Checks if Start/Stop button event occurred and toggles system state
void CheckifProgramIsOnOrOff(void) {

  // If button has been pressed (flag set by button ISR)
  if (StartStopButtonEvent) {
    // Reset StartStopButtonEvent and previousTime back to zero
    StartStopButtonEvent = 0;
    startStopPreviousTime = 0;
  getTimeViaRTC(timestamp, sizeof(timestamp));

    // Check current system state
    switch (ProgramStatus) {
      case DISABLED:
        // Starts the program
       // ProgramStatus = IDLE;
        logEvent("Program on", timestamp);
        break;

      case IDLE:
      case RUNNING:
      case ERROR:
        // Turns off the program from any active state
        ProgramStatus = DISABLED;
        ErrorCode = NONE;
        logEvent("Program off", timestamp);
        break;
    }
  }
}
*/

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

void ErrorHandling(ErrorCodes &ErrorCode) {
  // get the time in order to record the error
  getTimeViaRTC(timestamp, sizeof(timestamp));

  switch (ErrorCode) {
    // 1xx: Sensor & Data Errors
    case ERR_SOIL_ADC_RANGE:
      UpdateLCD("ERROR: Soil", "Sensor out of range");
      logEvent("101: Soil ADC out of range; check wiring, press RESET", timestamp);
      break;

    case ERR_SOIL_ADC_SATURATED:
      UpdateLCD("ERROR: Soil", "ADC saturated");
      logEvent("102: Soil ADC saturated; stuck at min/max", timestamp);
      break;

    case ERR_DHT_TIMEOUT:
      UpdateLCD("ERROR: DHT", "Read timeout");
      logEvent("111: DHT read timeout; will retry; press RESET after recover", timestamp);
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
      logEvent("301: Low water; pump disabled until refill and RESET", timestamp);
      break;

    case ERR_SOIL_NOT_RECOVERING:
      UpdateLCD("ERROR: Soil", "Not improving");
      logEvent("302: Soil not recovering; check plant/soil, then RESET", timestamp);
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
      logEvent("404: Unexpected restart; power loss or reset", timestamp);
      break;

    default:
      UpdateLCD("ERROR", "Unknown code");
      logEvent("000: Unknown error code", timestamp);
      break;
  }
}
void pumpOff() {
  //method to turn pumpoff
  isPumpOn = false;
}


void logEvent(const char *msg,const char* timestamp) {
    char buffer[200];  // enough for timestamp + message
    snprintf(buffer, sizeof(buffer), "[%s] %s", timestamp, msg);

    println(buffer);   // ALWAYS one line
}
