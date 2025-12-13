#ifndef LCDSCREEN_H
#define LCDSCREEN_H
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "StatusCodes.h"
// LCD pins <--> Arduino pins
// RS=35, EN=22, D4=2, D5=53, D6=5, D7=49
// V0_CONTRAST=9 (PWM pin for contrast control - no potentiometer needed)
const int RS = 35, EN = 22, D4 = 2, D5 = 53, D6 = 5, D7 = 49;
const int V0_CONTRAST_PIN = 9;  // PWM pin for contrast (connect LCD V0 to this pin)

// Accessible everywhere
extern LiquidCrystal lcd;

// Function to update LCD
void updateLCD(const String& status, float tempC, int soilPercent);
void UpdateLCD(const char *line1);
void UpdateLCD(const char *line1, const char *line2);

// Initialize contrast control (PWM on V0_CONTRAST_PIN)
void initLCDContrast(void);

// Set contrast level (0-255, typical range 80-150)
void setLCDContrast(int contrastLevel);

// Test function to verify LCD is working
void testLCDBasic(void);

// Test function to cycle through contrast values
void testLCDContrast(void);

#endif
