#include "Button.h"
#include "Logging.h"

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------
// Port D registers for pin 18 (PD3)
volatile unsigned char *my_DDRD  = (unsigned char *)0x2A;  // DDRD
volatile unsigned char *my_PORTD = (unsigned char *)0x2B;  // PORTD
volatile unsigned char *my_PIND  = (unsigned char *)0x29;  // PIND

// Port J registers for pin 14 (PJ1)
volatile unsigned char *my_DDRJ  = (unsigned char *)0x105;  // DDRJ
volatile unsigned char *my_PORTJ = (unsigned char *)0x106;  // PORTJ
volatile unsigned char *my_PINJ  = (unsigned char *)0x104;  // PINJ

// true  = program running
// false = program stopped
//volatile bool ProgramStatus        = false;
// set by ISR when a valid press is detected, cleared in main loop
volatile bool StartStopButtonEvent = false;

// separate timing for Start/Stop debounce
static volatile unsigned long startStopLastInterruptUs = 0;
// -----------------------------------------------------------------------------
// Start/Stop button
// -----------------------------------------------------------------------------
void StartStopButtonInit()
{
  // Pin 18 = PD3, configure as input with pull-up
  *my_DDRD  &= ~(1 << 3);  // PD3 as input
  *my_PORTD |=  (1 << 3);  // enable pull-up on PD3

  attachInterrupt(digitalPinToInterrupt(StartStopButtonPin), StartStopISR, FALLING);
}

// Reset button initialization
// Note: Pin 14 doesn't support hardware interrupts, so we use polling instead
void ResetButtonInit()
{
  // Pin 14 = PJ1, configure as input with pull-up using Arduino function
  pinMode(ResetButtonPin, INPUT_PULLUP);
}

// Called regularly from loop() to apply the effect of a press
void CheckifProgramIsOnOrOff()
{
  // Only act once per button press detected by ISR
  if (!StartStopButtonEvent) {
    return;
  }

  // consume the event
  StartStopButtonEvent = false;

  // Get timestamp for logging
  char timestamp[30];
  getTimeViaRTC(timestamp, sizeof(timestamp));

  if (ProgramStatus == DISABLED) {
    // Program just turned ON - transition to IDLE
    ProgramStatus = IDLE;
    ErrorCode = NONE;
    logEvent("State change: DISABLED -> IDLE (Start button)", timestamp);
  } 
  //if the program is in IDLE, running or Error
  else {
    // Program just turned OFF - transition to DISABLED
    ProgramStatus = DISABLED;
    ErrorCode = NONE;
    logEvent("State change: -> DISABLED (Stop button)", timestamp);
  }
}

// ISR: runs on falling edge of Start/Stop button
void StartStopISR()
{
  unsigned long now = micros();

  // Debounce: ignore edges that occur too soon after the last one
  if ((now - startStopLastInterruptUs) < STARTSTOP_DEBOUNCE_US) {
    return;
  }

  startStopLastInterruptUs = now;
  StartStopButtonEvent     = true;  // signal main loop that a valid press occurred
}

// Called regularly from loop() to check reset button (polling method)
// Pin 14 doesn't support interrupts, so we poll it instead
void CheckResetButton()
{
  static unsigned long lastResetCheck = 0;
  static bool lastButtonState = true;  // true = not pressed (pull-up)
  static unsigned long pressStartTime = 0;
  
  unsigned long now = millis();
  
  // Check button state every 20ms (polling)
  if ((now - lastResetCheck) < 20) {
    return;
  }
  lastResetCheck = now;
  
  // Read pin 14 using digitalRead - LOW means button is pressed (pull-up resistor)
  bool currentButtonState = digitalRead(ResetButtonPin) == LOW;
  
  // Detect button press (transition from not pressed to pressed)
  if (currentButtonState && !lastButtonState) {
    // Button was just pressed - start debounce timer
    pressStartTime = now;
  }
  
  // If button is pressed and held for at least 50ms (debounce)
  if (currentButtonState && (now - pressStartTime) > 50) {
    // Button is confirmed pressed - reset system
    pressStartTime = 0;  // Reset timer
    
    // Get timestamp for logging
    char timestamp[30];
    getTimeViaRTC(timestamp, sizeof(timestamp));
    
    // Reset system to IDLE state and clear all errors
    ProgramStatus = IDLE;
    ErrorCode = NONE;
    logEvent("System reset - returning to IDLE state", timestamp);
    
    // Wait for button release to prevent multiple triggers
    while (digitalRead(ResetButtonPin) == LOW) {
      delay(10);  // Wait for button release
    }
  }
  
  lastButtonState = currentButtonState;
}