#include "Button.h"

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------
volatile unsigned char *my_DDRE  = (unsigned char *)0x2D;  // DDR for Port E (Start/Stop button, PE4 / pin 2)
volatile unsigned char *my_PORTE = (unsigned char *)0x2E;  // PORT for Port E

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
  // Equivalent to pinMode(StartStopButtonPin, INPUT_PULLUP)
  *my_DDRE  &= ~(1 << 4);  // PE4 as input
  *my_PORTE |=  (1 << 4);  // enable pull-up on PE4

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

  // toggle program status
 // ProgramStatus = !ProgramStatus;

  if (ProgramStatus == DISABLED) {
    // Program just turned ON
  println("Program ON");
  ProgramStatus = IDLE;

  } 
  //if the program is in IDLE, running or Error
  else {
    // Program just turned OFF
  println("Program OFF");
  ProgramStatus = DISABLED;
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

/*
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
     println("Button Pressed");
     // uart_print_crlf();
    }
  }
}
*/