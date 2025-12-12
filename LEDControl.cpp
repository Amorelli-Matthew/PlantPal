#include "LEDControl.h"

// Port A registers for pins 24-26 (Green, Yellow, Red LEDs)
volatile unsigned char *my_DDRA  = (unsigned char *)0x21;  // DDRA
volatile unsigned char *my_PORTA = (unsigned char *)0x22;  // PORTA

// Port B registers for pin 12 (Blue LED)
volatile unsigned char *my_DDRB  = (unsigned char *)0x24;  // DDRB
volatile unsigned char *my_PORTB = (unsigned char *)0x25;  // PORTB

// Pin bit positions in Port A
// Port A: PA0=pin22, PA1=pin23, PA2=pin24, PA3=pin25, PA4=pin26, PA5=pin27, PA6=pin28, PA7=pin29
#define LED_GREEN_BIT  2  // PA2 = pin 24
#define LED_YELLOW_BIT 3  // PA3 = pin 25
#define LED_RED_BIT    4  // PA4 = pin 26

// Pin bit position in Port B
// Port B: PB0=pin53, PB1=pin52, ..., PB6=pin12, PB7=pin13
#define LED_BLUE_BIT   6  // PB6 = pin 12

void LEDInit(void) {
  // Set Port A LED pins as outputs (Green, Yellow, Red)
  *my_DDRA |= (1 << LED_YELLOW_BIT);
  *my_DDRA |= (1 << LED_GREEN_BIT);
  *my_DDRA |= (1 << LED_RED_BIT);
  
  // Set Port B LED pin as output (Blue)
  *my_DDRB |= (1 << LED_BLUE_BIT);
  
  // Turn all LEDs off initially
  *my_PORTA &= ~(1 << LED_YELLOW_BIT);
  *my_PORTA &= ~(1 << LED_GREEN_BIT);
  *my_PORTA &= ~(1 << LED_RED_BIT);
  *my_PORTB &= ~(1 << LED_BLUE_BIT);
}

void LEDControl(Status state) {
  // Turn all LEDs off first
  *my_PORTA &= ~(1 << LED_YELLOW_BIT);
  *my_PORTA &= ~(1 << LED_GREEN_BIT);
  *my_PORTA &= ~(1 << LED_RED_BIT);
  *my_PORTB &= ~(1 << LED_BLUE_BIT);
  
  // Turn on the appropriate LED based on state
  switch (state) {
    case DISABLED:
      *my_PORTA |= (1 << LED_YELLOW_BIT);  // Yellow LED on (pin 25)
      break;
      
    case IDLE:
      *my_PORTA |= (1 << LED_GREEN_BIT);   // Green LED on (pin 24)
      break;
      
    case RUNNING:
      *my_PORTB |= (1 << LED_BLUE_BIT);    // Blue LED on (pin 12)
      break;
      
    case ERROR:
      *my_PORTA |= (1 << LED_RED_BIT);     // Red LED on (pin 26)
      break;
  }
}

