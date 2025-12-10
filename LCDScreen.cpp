#include "LCDScreen.h"

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void updateLCD(const String& status, float tempC, int soilPercent) {
    lcd.clear();

    // Convert Celsius → Fahrenheit
    float tempF = (tempC * 9.0 / 5.0) + 32.0;

    // Line 1: Status
    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.print(status);

    // Line 2: Temp + Soil
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(tempF, 0);
    lcd.print("F ");

    lcd.print("Soil:");
    lcd.print(soilPercent);
    lcd.print("%");
}

void UpdateLCD(const char *line1) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
}

void UpdateLCD(const char *line1, const char *line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}
