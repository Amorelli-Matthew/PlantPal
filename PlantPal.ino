#include "Serial.h"
#include "WaterSensor.h"
#include "LCDScreen.h"
#include "Button.h"
#include "Logging.h"

enum Status {
    DISABLED = 0,
    IDLE     = 1,
    RUNNING  = 2,
    ERROR    = 3,
};

enum Status ProgramStatus = DISABLED;

//prototypes
void CheckifProgramIsOnOrOff(void);
void readResetButton(void);
void waterlevelcheck(void);
void ledStatus(void);

void setup(void) {
    // Initialize in this order:
    // 1) Serial operations
    // 2) LCD screen
    // 3) Buttons
    // 4) Water sensor / ADC
    // 5) Logging

    U0init(9600);

    // LCD init goes here
    StartStopButtonInit();
    ResetButtonInit();
    adc_init();
    // TODO: Logging init

    ProgramStatus = DISABLED;
}

void loop(void) {
    CheckifProgramIsOnOrOff();  // Handle Start/Stop button flag

    readResetButton();          // Handle Reset button
       // Reflect state on LEDs
    ledStatus();     

    switch(ProgramStatus) {
      case DISABLED:
        break;
      case IDLE:
        waterlevelcheck();
        // Later: soil moisture check & pump control can be added here
        break;

      case RUNNING:
        break;
      
      case ERROR:
          break;
    }
}

// Checks if Start/Stop button event occurred and toggles system state
void CheckifProgramIsOnOrOff(void) {

    // If button has been pressed (flag set by button ISR)
    if (StartStopButtonEvent) {
        // Reset StartStopButtonEvent and previousTime back to zero
        StartStopButtonEvent = 0;
        previousTime = 0;

        // Check current system state
        switch (ProgramStatus) {
            case DISABLED:
                // Starts the program
                ProgramStatus = IDLE;
                uart_print_str("Program on!");
                uart_print_crlf();
                break;

            case IDLE:
            case RUNNING:
            case ERROR:
                // Turns off the program from any active state
                ProgramStatus = DISABLED;
                uart_print_str("Program off!");
                uart_print_crlf();
                break;
    }
}

}

// Debounced read for Reset button
void readResetButton(void) {
    // Professor said millis() isn't ideal here, so use micros()
    currentTime = micros();

    // Direct hardware read from the reset button
    reading = ((*my_PINC & (1 << 1)) ? HIGH : LOW);

    // If this button's state is not the same as the last one
    // store the time and update lastReading (used for debouncing)
    if (reading != lastReading) {
        previousTime = currentTime;
        lastReading = reading;
    }

    // If enough time has passed and the reading is stable
    if ((currentTime - previousTime) >= RESET_DEBOUNCE_US && reading != stableState) {
        stableState = reading;

        if (stableState == LOW) {

            // Reset Button is pressed: TODO implement reset logic.
            if (ProgramStatus == ERROR)
            {
             

            }
          
            if(ProgramStatus == IDLE){
                  //Turn off the program
              }
        }
    }
}

// Method that checks how high the water is
void waterlevelcheck(void) {
    // Use a separate timing variable so we don't interfere with button debounce
    static unsigned long lastWaterCheckTime = 0;

    // Get the current time
    currentTime = millis();

    // Read the water every few seconds
    if ((currentTime - lastWaterCheckTime) >= 5000UL) {
        lastWaterCheckTime = currentTime;

        // Assume WaterSensorValue is updated elsewhere (ADC / ISR / WaterSensor module)
        if (WaterSensorValue > HIGH_LEVEL) {
            uart_print_str("Water Level: High at ");
            uart_print_uint(WaterSensorValue);
            uart_print_crlf();
        }
        else if (WaterSensorValue <= HIGH_LEVEL && WaterSensorValue >= MID_LEVEL) {
            uart_print_str("Water Level: Medium at ");
            uart_print_uint(WaterSensorValue);
            uart_print_crlf();
        }
        else {
            // If water level low, then set ProgramStatus to ERROR
            ProgramStatus = ERROR;

            uart_print_str("Water Level: Low at ");
            uart_print_uint(WaterSensorValue);
            uart_print_crlf();
        }
    }
}

// Display LED status based on current ProgramStatus
void ledStatus(void) {
    switch (ProgramStatus) {
        case DISABLED:
            // Yellow LED on, others off
            break;

        case IDLE:
            // Green LED on, others off
            break;

        case RUNNING:
            // Blue LED on, others off
            break;

        case ERROR:
            // Red LED on, others off
            break;
    }
}
