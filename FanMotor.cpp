#include "FanMotor.h"
#include "Serial.h"
#include "Logging.h"

// Declare logEvent function (defined in PlantPal.ino)
extern void logEvent(const char *msg, const char* timestamp);

// Use pin 31 for fan motor control
#define FAN_PIN 31

static bool fanEnabled = false;
static Status lastState = DISABLED;

void FanMotorInit(void) {
  // Set fan pin as output (using digitalWrite like the test code)
  pinMode(FAN_PIN, OUTPUT);
  
  // Turn fan off initially (active HIGH: set LOW to turn OFF)
  digitalWrite(FAN_PIN, LOW);
  
  fanEnabled = false;
  lastState = DISABLED;
  
  // Log initialization
  char timestamp[30];
  getTimeViaRTC(timestamp, sizeof(timestamp));
  logEvent("Fan motor initialized on pin 31", timestamp);
}

void FanMotorControl(Status state) {
  // Always update the pin state (don't skip if state hasn't changed)
  // This ensures the pin stays HIGH even if called multiple times
  if (state == RUNNING) {
    // Turn on fan motor only in RUNNING state
    // Active HIGH: set pin HIGH to turn fan ON (matches test code)
    digitalWrite(FAN_PIN, HIGH);
    fanEnabled = true;
    
    // Log when entering RUNNING state
    if (lastState != RUNNING) {
      char timestamp[30];
      getTimeViaRTC(timestamp, sizeof(timestamp));
      logEvent("Fan motor: PIN 31 set to HIGH (ON) - RUNNING state", timestamp);
      
      // Verify pin is actually HIGH
      int pinState = digitalRead(FAN_PIN);
      char verifyMsg[60];
      snprintf(verifyMsg, sizeof(verifyMsg), "Fan pin 31 readback: %d (should be 1)", pinState);
      logEvent(verifyMsg, timestamp);
    }
  } else {
    // Turn off fan motor in all other states (DISABLED, IDLE, ERROR)
    // Active HIGH: set pin LOW to turn fan OFF
    digitalWrite(FAN_PIN, LOW);
    fanEnabled = false;
    
    // Log when leaving RUNNING state
    if (lastState == RUNNING) {
      char timestamp[30];
      getTimeViaRTC(timestamp, sizeof(timestamp));
      logEvent("Fan motor: PIN 31 set to LOW (OFF)", timestamp);
    }
  }
  lastState = state;
}

