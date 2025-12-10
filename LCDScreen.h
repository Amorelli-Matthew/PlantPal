#ifndef LCDSCREEN_H
#define LCDSCREEN_H
#include <LiquidCrystal.h>
#include "StatusCodes.h"
// LCD pins <--> Arduino pins
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

// Accessible everywhere
extern LiquidCrystal lcd;

// Function to update LCD
void updateLCD(const String& status, float tempC, int soilPercent);
void UpdateLCD(const char *line1);
void UpdateLCD(const char *line1, const char *line2);

#endif
