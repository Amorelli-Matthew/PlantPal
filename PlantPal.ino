#include "Serial.h"
#include "WaterSensor.h"
#include "Button.h"
#include "Logging.h"
#include "Temperature.h"
#include "StatusCodes.h"
#include "SoilSensor.h"
unsigned long currentTime = 0;

//prototypes
void CheckifProgramIsOnOrOff(void);
void readResetButton(void);
void ledStatus(void);
void updateLCDAndLog(void);
void ErrorHanlding(ErrorCodes &ErrorCode);


void setup(void) {
  // Initialize in this order:
  // 1) Serial operations
  // 2) LCD screen
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
      break;

    case IDLE:

      //check for every minute
      if ((currentTime - MainspreviousTime) >= 60000) {
        MainspreviousTime = currentTime;

       //waterlevelcheck();
        //TempandHumanitySensorCheck();

        // Later: soil moisture check & pump control can be added here
        if (isSoilDry())
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

      break;

    case ERROR:
      //for error handling
      //check what error message it is
      //print error message
      //force pump off/all other sensors
      ErrorHanlding(ErrorCode);
      break;
  }
}

// Checks if Start/Stop button event occurred and toggles system state
void CheckifProgramIsOnOrOff(void) {

  // If button has been pressed (flag set by button ISR)
  if (StartStopButtonEvent) {
    // Reset StartStopButtonEvent and previousTime back to zero
    StartStopButtonEvent = 0;
    previousTime = 0;

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

// Debounced read for Reset button
void readResetButton(void) {
  // Professor said millis() isn't ideal here, so use micros()
  currentTime = micros();

  // Direct hardware read from the reset button
  reading = ((*my_PINC & (1 << 1)) ? HIGH : LOW);

  // If this button's state is not the same as the last one
  // store the time and update lastReading (used for debouncing)
  if (reading != lastReading) {
    previousTime = currentTime;
    lastReading = reading;
  }

  // If enough time has passed and the reading is stable
  if ((currentTime - previousTime) >= RESET_DEBOUNCE_US && reading != stableState) {
    stableState = reading;

    if (stableState == LOW) {

      // Reset Button is pressed: TODO implement reset logic.
      if (ProgramStatus == ERROR) {
      }

      if (ProgramStatus == IDLE) {
        //Turn off the program
      }
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
  RTCTest();
}

void ErrorHanlding(ErrorCodes &ErrorCode) {
  switch (ErrorCode) {
      //make them print to lcd screen now
    case NONE:
      println("OK: No active error");
      break;

    // 1xx: Sensor & Data Errors
    case ERR_SOIL_ADC_RANGE:
      println("101: Soil ADC out of expected range");
      break;

    case ERR_SOIL_ADC_SATURATED:
      println("102: Soil ADC saturated");
      break;

    case ERR_DHT_TIMEOUT:
      println("111: DHT sensor timed out");
      break;

    case ERR_DHT_SENSOR_FAULT:
      println("112: DHT sensor hardware fault");
      break;

    // 2xx: Pump / Motor Errors
    case ERR_PUMP_TIMEOUT:
      println("201: Pump timeout");
      break;

    case ERR_PUMP_NO_CURRENT:
      println("202: Pump no current draw");
      break;

    case ERR_PUMP_OVERCURRENT:
      println("203: Pump overcurrent!");
      break;

    // 3xx: Environmental Condition Errors
    case ERR_LOW_WATER:
      println("301: Low water level");
      break;

    case ERR_SOIL_NOT_RECOVERING:
      println("302: Soil moisture not improving");
      break;

    // 4xx: System / Logic Errors
    case ERR_RTC_FAILURE:
      println("401: RTC communication failure");
      break;

    case ERR_INIT_FAILURE:
      println("402: System initialization failed");
      break;

    case ERR_UNEXPECTED_STATE:
      println("403: Unexpected state detected");
      break;

    case ERR_UNEXPECTED_RESTART:
      println("404: Unexpected restart");
      break;

    default:
      println("Unknown error code!");
      break;
  }
}
