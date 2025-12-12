#include "LCDScreen.h"
#include "Serial.h"
#include <string.h>

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Initialize contrast control using PWM
// LCD V0 pin is connected directly to pin 9 (V0_CONTRAST_PIN)
// PWM generates 0-5V signal: 0 = darkest, 255 = lightest
// Typical LCD contrast range: 40-60 (lower values = more visible text)
void initLCDContrast(void) {
  pinMode(V0_CONTRAST_PIN, OUTPUT);
  // Start with comfortable contrast value (lower = less bright/flashing)
  // If text is too dark/invisible: increase value (try 70, 80, 90)
  // If text is still too bright/flashing: decrease value (try 50, 40)
  setLCDContrast(120);  // Increased to 120 for better visibility when motor is running
}

// Set contrast level (0-255)
// Lower values = darker/more contrast, higher values = lighter/less contrast
void setLCDContrast(int contrastLevel) {
  // Clamp to valid PWM range
  if (contrastLevel < 0) contrastLevel = 0;
  if (contrastLevel > 255) contrastLevel = 255;
  
  analogWrite(V0_CONTRAST_PIN, contrastLevel);
}

void updateLCD(const String& status, float tempC, int soilPercent) {
    // Ensure display is always on
    lcd.display();
    
    // Don't clear every time - just overwrite to prevent flickering
    // Only clear if we need to (when status changes significantly)
    
    // Convert Celsius → Fahrenheit
    float tempF = (tempC * 9.0 / 5.0) + 32.0;

    // Line 1: Status (overwrite existing text)
    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.print(status);
    // Pad with spaces to clear any leftover characters
    int statusLen = 7 + status.length();  // "Status:" is 7 chars
    for (int i = statusLen; i < 16; i++) {
      lcd.print(" ");
    }

    // Line 2: Temp + Soil (overwrite existing text)
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(tempF, 0);
    lcd.print("F ");

    lcd.print("Soil:");
    lcd.print(soilPercent);
    lcd.print("%");
    // Pad with spaces to clear any leftover characters
    // Rough estimate: "T:XXF Soil:XX%" is about 15 chars, pad to 16
    for (int i = 0; i < 2; i++) {
      lcd.print(" ");
    }
    
    lcd.display();  // Final display call
}

void UpdateLCD(const char *line1) {
  // Don't re-initialize LCD every time - that causes garbled text
  // Just ensure display is on and update text without unnecessary clears
  
  // Explicitly turn display on before any operations
  lcd.display();
  
  // Only clear if we need to (to prevent fading from repeated clears)
  // Just update the text directly
  lcd.setCursor(0, 0);
  lcd.print(line1);
  // Pad with spaces to clear any leftover characters
  int len = strlen(line1);
  for (int i = len; i < 16; i++) {
    lcd.print(" ");
  }
  
  // Clear line 2
  lcd.setCursor(0, 1);
  lcd.print("                ");  // 16 spaces to clear line 2
  
  // Final display() call to ensure it stays on
  lcd.display();
}

void UpdateLCD(const char *line1, const char *line2) {
  // Ensure display is on and set contrast
  lcd.display();
  setLCDContrast(35);  // Use same contrast as other states
  
  // Clear display first to remove any leftover characters
  lcd.clear();
  delay(15);  // Small delay for clear to complete
  lcd.display();  // Ensure display stays on after clear
  delay(15);  // Additional delay for stability
  
  // Print line 1
  lcd.setCursor(0, 0);
  lcd.print(line1);
  // Pad line 1 with spaces to ensure clean display
  int len1 = strlen(line1);
  for (int i = len1; i < 16; i++) {
    lcd.print(" ");
  }
  
  // Print line 2
  lcd.setCursor(0, 1);
  lcd.print(line2);
  // Pad line 2 with spaces to ensure clean display
  int len2 = strlen(line2);
  for (int i = len2; i < 16; i++) {
    lcd.print(" ");
  }
  
  // Final display() call to ensure it stays on
  lcd.display();
}

// Simple test function to verify LCD is working
void testLCDBasic(void) {
  println("LCD Test: Turning display on...");
  
  // Re-initialize LCD to fix any garbled text issues
  lcd.begin(16, 2);
  delay(100);
  lcd.clear();
  delay(50);
  lcd.clear();  // Clear twice to ensure reset
  delay(50);
  
  lcd.display();
  setLCDContrast(100);  // Use moderate contrast for test
  delay(100);
  
  // Test with simple characters first
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD TEST 12345");
  lcd.setCursor(0, 1);
  lcd.print("ABCDEFGHIJKLM");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Working!");
  lcd.setCursor(0, 1);
  lcd.print("Contrast: 100");
  delay(3000);  // Show for 3 seconds so user can see it
  
  // Restore contrast
  setLCDContrast(120);
  
  // Final clear and small delay before returning to normal display
  lcd.clear();
  delay(100);
}
