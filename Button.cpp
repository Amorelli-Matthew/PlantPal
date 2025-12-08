#include "Button.h"

volatile unsigned char* my_DDRE = (unsigned char*)0x2D;   // DDR for Port E interupt pin, pin 2, aka for the starting stoping the progran
volatile unsigned char* my_PORTE = (unsigned char*)0x2E;  // PORT for Port E

volatile unsigned char* my_PINC = (volatile unsigned char*)0x26;  //for pin 36 aka the rest pin
volatile unsigned char* my_DDRC = (volatile unsigned char*)0x27;
volatile unsigned char* my_PORTC = (volatile unsigned char*)0x28;

void StartStopButtonInit() {
  //for the starting and stopping the system basically does pinMode(StartStopButton, INPUT_PULLUP);

  *my_DDRE &= ~(1 << 4);
  *my_PORTE |= (1 << 4);  //enables pull up

  //attach the interupt
  attachInterrupt(digitalPinToInterrupt(StartStopButtonPin), StartStopISR, FALLING);
}

//init reset button
void ResetButtonInit() {
  // Configure pin 36 as input with pull-up like using pinMode
  *my_DDRC &= ~(1 << 1);  // input
  *my_PORTC |= (1 << 1);  // enable pull-up
}


void StartStopISR() {
  //get the current time
  currentTime = micros();

  //check the difference in time and see if its been after 10 microseconds
  //if so, store the current time into the previous time variable and then invert the startStopButton button events state.
  if ((currentTime - previousTime) >= (STARTSTOP_DEBOUNCE_US)) {
    previousTime = currentTime;
    StartStopButtonEvent = !StartStopButtonEvent;
  }
}



