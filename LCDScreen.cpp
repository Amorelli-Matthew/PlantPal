#include "LCDScreen.h"

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Initialize contrast control using PWM
// LCD V0 pin is connected directly to pin 9 (V0_CONTRAST_PIN)
void initLCDContrast(void)
{
  // PH6 (OC2B) as output
  DDRH |= (1 << V0_BIT);
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);

  // Start Timer2 with prescaler = clk/64
  TCCR2B = (1 << CS22);

  // Initial duty (replaces analogWrite default)
  setLCDContrast(80);
}


// Set contrast level (0-255)
// Lower values = darker/more contrast, higher values = lighter/less contrast
void setLCDContrast(int contrastLevel)
{
  if (contrastLevel < 0)   contrastLevel = 0;
  if (contrastLevel > 255) contrastLevel = 255;

  // Duty cycle for OC2B PWM (replaces analogWrite)
  OCR2B = (uint8_t)contrastLevel;
}

void updateLCD(const String& status, float tempC, int soilPercent) {
    // Ensure display is always on and contrast is set
    lcd.display();
    setLCDContrast(80);  // Use contrast 80
    
    // Convert Celsius → Fahrenheit
    float tempF = (tempC * 9.0 / 5.0) + 32.0;

    // Clear the entire display first to remove any gibberish
    lcd.clear();
    
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
  lcd.display();
  setLCDContrast(50);

  lcd.clear();
  lcd.display();

  lcd.setCursor(0, 0);
  lcd.print(line1);
  int len1 = strlen(line1);
  for (int i = len1; i < 16; i++) lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print(line2);
  int len2 = strlen(line2);
  for (int i = len2; i < 16; i++) lcd.print(" ");

  lcd.display();
}



