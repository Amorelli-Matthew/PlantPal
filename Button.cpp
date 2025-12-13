#include "Button.h"
#include "Logging.h"

// Globals
// Port D registers for pin 18 (PD3)
volatile unsigned char *my_DDRD  = (unsigned char *)0x2A;  // DDRD
volatile unsigned char *my_PORTD = (unsigned char *)0x2B;  // PORTD
volatile unsigned char *my_PIND  = (unsigned char *)0x29;  // PIND

// Port J registers for pin 14 (PJ1)
volatile unsigned char *my_DDRJ  = (unsigned char *)0x105;  // DDRJ
volatile unsigned char *my_PORTJ = (unsigned char *)0x106;  // PORTJ
volatile unsigned char *my_PINJ  = (unsigned char *)0x104;  // PINJ


// set by ISR when a valid press is detected, cleared in main loop
volatile bool StartStopButtonEvent = false;

// separate timing for Start/Stop debounce
static volatile unsigned long startStopLastInterruptUs = 0;

// Start/Stop button
void StartStopButtonInit()
{
  // Pin 18 = PD3, configure as input with pull-up
  *my_DDRD  &= ~(1 << 3);  // PD3 as input
  *my_PORTD |=  (1 << 3);  // enable pull-up on PD3

  attachInterrupt(digitalPinToInterrupt(StartStopButtonPin), StartStopISR, FALLING);
}

void ResetButtonInit()
{

  *my_DDRJ  &= ~(1 << 1);   // PJ1 as input
  *my_PORTJ |=  (1 << 1);   // enable internal pull-up
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

// Reset button is PJ1 (Pin 14), active-low with pull-up
void CheckResetButton()
{
  static unsigned long lastResetCheck = 0;
  static bool lastPressed = false;          // previous debounced pressed state
  static unsigned long pressStartTime = 0;  // for debounce timing
  static bool fired = false;                // prevents retrigger while held

  unsigned long now = millis();

  // Poll every 20ms
  if (now - lastResetCheck < 20) return;
  lastResetCheck = now;

  // Raw read: pressed when PJ1 reads 0 (active-low)
  bool pressedRaw = ((*my_PINJ & (1 << 1)) == 0);

  // Edge: just went down -> start debounce timer
  if (pressedRaw && !lastPressed && pressStartTime == 0) {
    pressStartTime = now;
  }

  // If released, clear timers/flags so next press can trigger
  if (!pressedRaw) {
    pressStartTime = 0;
    fired = false;
    lastPressed = false;
    return;
  }

  // Still pressed: confirm after 50ms debounce
  if (!fired && pressStartTime != 0 && (now - pressStartTime) >= 50) {
    fired = true;          // lock until release
    lastPressed = true;

    // Log + reset state (non-blocking)
  char timestamp[30];
  getTimeViaRTC(timestamp, sizeof(timestamp));
    ProgramStatus = IDLE;
    ErrorCode = NONE;
    logEvent("System reset - returning to IDLE state", timestamp);
  }
}
