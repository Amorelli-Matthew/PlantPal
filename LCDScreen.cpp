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
  // Set initial contrast to 80
  setLCDContrast(80);
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
    // Ensure display is always on and contrast is set
    lcd.display();
    setLCDContrast(80);  // Use contrast 80
    
    // Convert Celsius → Fahrenheit
    float tempF = (tempC * 9.0 / 5.0) + 32.0;

    // Clear the entire display first to remove any gibberish
    lcd.clear();
    delay(2);  // Small delay for clear to complete
    
    // Line 1: Write Status
    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.print(status);
    // Pad with spaces to ensure clean display
    int statusLen = 7 + status.length();  // "Status:" is 7 chars
    for (int i = statusLen; i < 16; i++) {
      lcd.print(" ");
    }

    // Line 2: Write Temp + Soil
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(tempF, 0);
    lcd.print("F ");
    
    lcd.print("Soil:");
    lcd.print(soilPercent);
    lcd.print("%");
    // Pad with spaces to ensure clean display
    int tempLen = (tempF < 10) ? 1 : ((tempF < 100) ? 2 : 3);
    int soilLen = (soilPercent < 10) ? 1 : ((soilPercent < 100) ? 2 : 3);
    int line2Len = 2 + tempLen + 2 + 5 + soilLen + 1;  // "T:" + temp + "F " + "Soil:" + percent + "%"
    for (int i = line2Len; i < 16; i++) {
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
  setLCDContrast(50);  // Use consistent contrast
  
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
  // Re-initialize LCD to fix any garbled text issues
  lcd.begin(16, 2);
  delay(200);
  lcd.display();
  lcd.clear();
  delay(100);
  
  setLCDContrast(100);  // Use moderate contrast for test
  delay(100);
  
  // Test with simple characters first
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
  
  // Final clear and small delay before returning to normal display
  lcd.clear();
  delay(100);
}

// Test function to cycle through contrast values to find the right one
void testLCDContrast(void) {
  // Re-initialize LCD
  lcd.begin(16, 2);
  delay(500);
  lcd.display();
  lcd.clear();
  delay(200);
  
  // Test a wider range of contrast values
  // Some LCDs work with very low values, others need higher
  int contrastValues[] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 120, 150, 180, 200, 220, 240, 255};
  int numValues = sizeof(contrastValues) / sizeof(contrastValues[0]);
  
  for (int i = 0; i < numValues; i++) {
    int contrast = contrastValues[i];
    setLCDContrast(contrast);
    delay(200);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Contrast Test");
    lcd.setCursor(0, 1);
    lcd.print("Value: ");
    lcd.print(contrast);
    
    // Show each value for 3 seconds so you can see which one works
    delay(3000);
  }
  
  // After cycling, set to a middle value
  setLCDContrast(50);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test Complete");
  lcd.setCursor(0, 1);
  lcd.print("Using: 50");
  delay(3000);
}
