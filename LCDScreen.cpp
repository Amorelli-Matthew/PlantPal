#include "LCDScreen.h"
#include "Serial.h"

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void updateLCD(const String& status, float tempC, int soilPercent) {
    lcd.clear();
    lcd.display();  // Ensure display is on
    delay(10);  // Small delay after clear

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
  lcd.display();  // Ensure display is on
  delay(10);  // Small delay after clear
  lcd.setCursor(0, 0);
  lcd.print(line1);
}

void UpdateLCD(const char *line1, const char *line2) {
  lcd.clear();
  lcd.display();  // Ensure display is on
  delay(10);  // Small delay after clear
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

// Test function to diagnose LCD issues
// Displays a pattern that helps identify contrast and display problems
void testLCD(void) {
  lcd.clear();
  lcd.display();
  lcd.noCursor();
  lcd.noBlink();
  delay(50);
  
  // Display a test pattern
  // Line 1: Numbers 0-9 to test all segments
  lcd.setCursor(0, 0);
  lcd.print("0123456789ABCDEF");
  
  // Line 2: Alternating pattern
  lcd.setCursor(0, 1);
  lcd.print("TEST PATTERN OK!");
  
  delay(2000);
  
  // Clear and show another pattern
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Line 1: 16 chars");
  lcd.setCursor(0, 1);
  lcd.print("Line 2: 16 chars");
  
  delay(2000);
  
  // Show cursor positions
  lcd.clear();
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 0);
    lcd.print((char)('A' + (i % 26)));
  }
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.print((char)('0' + (i % 10)));
  }
  
  delay(2000);
  
  // Final test - simple message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Test Complete");
  lcd.setCursor(0, 1);
  lcd.print("Adjust contrast!");
}

// Diagnostic function to check if LCD is wired correctly
// Prints wiring information to serial and tests LCD communication
bool checkLCDWiring(void) {
  println("\n=== LCD WIRING DIAGNOSTIC ===");
  println("Expected pin connections:");
  print("  VSS (GND) -> GND");
  println("");
  print("  VDD (5V)  -> 5V");
  println("");
  print("  V0 (Contrast) -> Potentiometer middle pin");
  println("");
  print("  RS  -> Pin 23");
  println("");
  print("  EN  -> Pin 22");
  println("");
  print("  D4  -> Pin 2");
  println("");
  print("  D5  -> Pin 3");
  println("");
  print("  D6  -> Pin 4");
  println("");
  print("  D7  -> Pin 5");
  println("");
  print("  A (Backlight +) -> 5V (or through resistor)");
  println("");
  print("  K (Backlight -) -> GND");
  println("");
  println("Testing LCD communication...");
  
  // Try to initialize and clear the display
  lcd.begin(16, 2);
  delay(100);
  
  // Test 1: Try to clear display
  lcd.clear();
  delay(50);
  println("  [OK] LCD clear command sent");
  
  // Test 2: Try to write to display
  lcd.setCursor(0, 0);
  lcd.print("WIRING TEST");
  delay(100);
  println("  [OK] Write to line 1 attempted");
  
  // Test 3: Try to move cursor and write to second line
  lcd.setCursor(0, 1);
  lcd.print("CHECK PINS");
  delay(500);
  println("  [OK] Write to line 2 attempted");
  
  println("\nIf you see black squares but no text:");
  println("  -> Adjust contrast potentiometer");
  println("\nIf you see nothing at all:");
  println("  -> Check power connections (5V and GND)");
  println("  -> Check backlight connections");
  println("  -> Verify all data pins (RS, EN, D4-D7)");
  println("\nIf text appears garbled:");
  println("  -> Check data pins D4-D7 connections");
  println("  -> Verify RS and EN pins");
  println("==============================\n");
  
  return true;
}
