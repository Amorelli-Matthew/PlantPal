#ifndef LCDSCREEN_H
#define LCDSCREEN_H
#include <LiquidCrystal.h>

// LCD pins <--> Arduino pins
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;

extern LiquidCrystal lcd;

#endif