#ifndef LCDSCREEN_H
#define LCDSCREEN_H
#include <LiquidCrystal.h>

// LCD pins <--> Arduino pins
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;


LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif