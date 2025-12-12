#include "Button.h"
#include "Logging.h"

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------
// Port D registers for pin 18 (PD3)
volatile unsigned char *my_DDRD  = (unsigned char *)0x2A;  // DDRD
volatile unsigned char *my_PORTD = (unsigned char *)0x2B;  // PORTD
volatile unsigned char *my_PIND  = (unsigned char *)0x29;  // PIND

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