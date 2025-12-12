#include "Button.h"
#include "Logging.h"

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------
// Port D registers for pin 18 (PD3)
volatile unsigned char *my_DDRD  = (unsigned char *)0x2A;  // DDRD
volatile unsigned char *my_PORTD = (unsigned char *)0x2B;  // PORTD
volatile unsigned char *my_PIND  = (unsigned char *)0x29;  // PIND

volatile unsigned char* my_PINC  = (volatile unsigned char*)0x26; // Port C input (reset button, PC1 / pin 36)
volatile unsigned char* my_DDRC  = (volatile unsigned char*)0x27;
volatile unsigned char* my_PORTC = (volatile unsigned char*)0x28;

// true  = program running
// false = program stopped
//volatile bool ProgramStatus        = false;
// set by ISR when a valid press is detected, cleared in main loop
volatile bool StartStopButtonEvent = false;

// separate timing for Start/Stop debounce (don’t share with reset button)
static volatile unsigned long startStopLastInterruptUs = 0;

int lastReading;
int stableState;
int reading;
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

// -----------------------------------------------------------------------------
// Reset button (unchanged logic, just shown for completeness)
// -----------------------------------------------------------------------------
void ResetButtonInit()
{
  // Configure pin 36 (PC1) as input with pull-up
  *my_DDRC  &= ~(1 << PC1_BIT);  // input
  *my_PORTC |=  (1 << PC1_BIT);  // enable pull-up
}

void readResetButton()
{
  btncurrentTime = micros();

  // Direct hardware read — no digitalRead()
  reading = ((*my_PINC & (1 << PC1_BIT)) ? HIGH : LOW);

  // Debounce
  if (reading != lastReading) {
    btnpreviousTime = btncurrentTime;
    lastReading     = reading;
  }

  if ((btncurrentTime - btnpreviousTime) >= RESET_DEBOUNCE_US && reading != stableState) {
    stableState = reading;

    if (stableState == LOW) {
      // Reset button pressed - return to IDLE from ERROR state
      if (ProgramStatus == ERROR) {
        ProgramStatus = IDLE;
        ErrorCode = NONE;
        char timestamp[30];
        getTimeViaRTC(timestamp, sizeof(timestamp));
        logEvent("Reset pressed - returning to IDLE", timestamp);
      }
    }
  }
}