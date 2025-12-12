#include "LEDControl.h"

// Port B registers for pins 10-13
volatile unsigned char *my_DDRB  = (unsigned char *)0x24;  // DDRB
volatile unsigned char *my_PORTB = (unsigned char *)0x25;  // PORTB

// Pin bit positions in Port B
#define LED_YELLOW_BIT 5  // PB5 = pin 11
#define LED_GREEN_BIT  4  // PB4 = pin 10
#define LED_RED_BIT    6  // PB6 = pin 12
#define LED_BLUE_BIT   7  // PB7 = pin 13

void LEDInit(void) {
  // Set all LED pins as outputs
  *my_DDRB |= (1 << LED_YELLOW_BIT);
  *my_DDRB |= (1 << LED_GREEN_BIT);
  *my_DDRB |= (1 << LED_RED_BIT);
  *my_DDRB |= (1 << LED_BLUE_BIT);
  
  // Turn all LEDs off initially
  *my_PORTB &= ~(1 << LED_YELLOW_BIT);
  *my_PORTB &= ~(1 << LED_GREEN_BIT);
  *my_PORTB &= ~(1 << LED_RED_BIT);
  *my_PORTB &= ~(1 << LED_BLUE_BIT);
}

void LEDControl(Status state) {
  // Turn all LEDs off first
  *my_PORTB &= ~(1 << LED_YELLOW_BIT);
  *my_PORTB &= ~(1 << LED_GREEN_BIT);
  *my_PORTB &= ~(1 << LED_RED_BIT);
  *my_PORTB &= ~(1 << LED_BLUE_BIT);
  
  // Turn on the appropriate LED based on state
  switch (state) {
    case DISABLED:
      *my_PORTB |= (1 << LED_YELLOW_BIT);  // Yellow LED on
      break;
      
    case IDLE:
      *my_PORTB |= (1 << LED_GREEN_BIT);   // Green LED on
      break;
      
    case RUNNING:
      *my_PORTB |= (1 << LED_BLUE_BIT);    // Blue LED on
      break;
      
    case ERROR:
      *my_PORTB |= (1 << LED_RED_BIT);     // Red LED on
      break;
  }
}

