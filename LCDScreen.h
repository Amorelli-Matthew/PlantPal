#ifndef LCDSCREEN_H
#define LCDSCREEN_H
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "StatusCodes.h"
// LCD pins <--> Arduino pins
// RS=23, EN=22, D4=2, D5=3, D6=4, D7=5
const int RS = 23, EN = 22, D4 = 2, D5 = 3, D6 = 4, D7 = 5;

// Accessible everywhere
extern LiquidCrystal lcd;

// Function to update LCD
void updateLCD(const String& status, float tempC, int soilPercent);
void UpdateLCD(const char *line1);
void UpdateLCD(const char *line1, const char *line2);

#endif
