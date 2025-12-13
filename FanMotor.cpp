#include "FanMotor.h"

// Port C registers for pin 32 (PC7)
volatile unsigned char *my_DDRC  = (unsigned char *)0x27;  // DDRC
volatile unsigned char *my_PORTC = (unsigned char *)0x28;  // PORTC

// Pin bit position
#define FAN_PIN_32_BIT 7  // PC7 = pin 32

static bool fanEnabled = false;

void FanMotorInit(void) {
  // Set fan pin as output
  *my_DDRC |= (1 << FAN_PIN_32_BIT);
  
  // Turn fan off initially (active LOW: set HIGH to turn OFF)
  *my_PORTC |= (1 << FAN_PIN_32_BIT);
  
  fanEnabled = false;
}

void FanMotorControl(Status state) {
  if (state == RUNNING) {
    // Turn on fan motor only in RUNNING state
    // Active LOW: set pin LOW to turn fan ON
    *my_PORTC &= ~(1 << FAN_PIN_32_BIT);
    fanEnabled = true;
  } else {
    // Turn off fan motor in all other states (DISABLED, IDLE, ERROR)
    // Active LOW: set pin HIGH to turn fan OFF
    *my_PORTC |= (1 << FAN_PIN_32_BIT);
    fanEnabled = false;
  }
}

