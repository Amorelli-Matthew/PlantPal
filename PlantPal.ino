#include "Serial.h"
#include "WaterSensor.h"
#include "Button.h"
#include "Logging.h"
#include "Temperature.h"
#include "StatusCodes.h"
// enum Status {
//   DISABLED = 0,
//   IDLE = 1,
//   RUNNING = 2,
//   ERROR = 3,
// };

// enum ErrorCodes {
//   NONE = 0,

//   //1xx: Sensor & Data Errors
//   ERR_SOIL_ADC_RANGE = 101,
//   ERR_SOIL_ADC_SATURATED = 102,
//   ERR_DHT_TIMEOUT = 111,
//   ERR_DHT_SENSOR_FAULT = 112,

//   //2xx: Pump / Motor Errors
//   ERR_PUMP_TIMEOUT = 201,
//   ERR_PUMP_NO_CURRENT = 202,
//   ERR_PUMP_OVERCURRENT = 203,

//   //3xx: Environmental Condition Errors
//   ERR_LOW_WATER = 301,
//   ERR_SOIL_NOT_RECOVERING = 302,

//   //4xx: System / Logic Errors
//   ERR_RTC_FAILURE = 401,
//   ERR_INIT_FAILURE = 402,
//   ERR_UNEXPECTED_STATE = 403,
//   ERR_UNEXPECTED_RESTART = 404,
// };



Status ProgramStatus = DISABLED;
ErrorCodes ErrorCode = NONE;

unsigned long currentTime = 0;

//prototypes
void CheckifProgramIsOnOrOff(void);
void readResetButton(void);
void waterlevelcheck(void);
void soilCheck(void);
void TempandHumanitySensorCheck(void);
void ledStatus(void);

void setup(void) {
  // Initialize in this order:
  // 1) Serial operations
  // 2) LCD screen
  // 3) Buttons
  // 4) Water sensor / ADC
  // 5) Logging

  U0init(9600);

  // LCD init goes here
  StartStopButtonInit();
  ResetButtonInit();
  adc_init();
  // TODO: Logging init

  ProgramStatus = DISABLED;
  ErrorCode = NONE;
}

void loop(void) {
  CheckifProgramIsOnOrOff();  // Handle Start/Stop button flag

  readResetButton();  // Handle Reset button
                      // Reflect state on LEDs
  ledStatus();

  switch (ProgramStatus) {
    case DISABLED:
      //track if pumps off, if not then turn water pumpoff
      //sensors off
      //print to lcd: system disabled
      //
      break;
    case IDLE:
      waterlevelcheck();
      //soil check

      ReadTempature();
      // Later: soil moisture check & pump control can be added here

      break;

    case RUNNING:
      //run water and other logic
      waterlevelcheck();
      ReadTempature();
      break;

    case ERROR:
      //for error handling
      //check what error message it is
      //print error message
      //force pump off
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
// Use a separate timing variable so we don't interfere with button debounce
static unsigned long lastWaterCheckTime = 0;

// Method that checks how high the water is
void waterlevelcheck(void) {

  // Get the current time
  currentTime = millis();

  // Read the water every few seconds
  if ((currentTime - lastWaterCheckTime) >= 3000UL) {
    lastWaterCheckTime = currentTime;

    // Assume WaterSensorValue is updated elsewhere (ADC / ISR / WaterSensor module)
    if (WaterSensorValue > HIGH_LEVEL) {
      print("Water Level: High at ");
      println(WaterSensorValue);

    } else if (WaterSensorValue <= HIGH_LEVEL && WaterSensorValue >= MID_LEVEL) {
      print("Water Level: Medium at ");
      println(WaterSensorValue);

    } else {
      // If water level low, then set ProgramStatus to ERROR and set the error satus to low water
      ProgramStatus = ERROR;
      ErrorCode = ERR_LOW_WATER;
      print("Water Level: Low at ");
      println(WaterSensorValue);
    }
  }
}

void soilCheck() {
}

void TempandHumanitySensorCheck() {

  int status = ReadTempature();

  if (status != DHTLIB_OK) {

    bool tempInvalid = (temperature < 0 || temperature > 50);
    bool humidityInvalid = (humidity < 20 || humidity > 90);

    if (tempInvalid || humidityInvalid) {
      ProgramStatus = ERROR;
      ErrorCode = ERR_DHT_SENSOR_FAULT;
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
