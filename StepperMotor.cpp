#include "StepperMotor.h"

// Port G registers for pins 40-41
volatile unsigned char *my_DDRG  = (unsigned char *)0x33;  // DDRG
volatile unsigned char *my_PORTG = (unsigned char *)0x34;  // PORTG

// Port L registers for pins 42-43
volatile unsigned char *my_DDRL  = (unsigned char *)0x10A;  // DDRL
volatile unsigned char *my_PORTL = (unsigned char *)0x10B;  // PORTL

// Pin bit positions
#define STEPPER_PIN_40_BIT 1  // PG1 = pin 40
#define STEPPER_PIN_41_BIT 0  // PG0 = pin 41
#define STEPPER_PIN_42_BIT 7  // PL7 = pin 42
#define STEPPER_PIN_43_BIT 6  // PL6 = pin 43

// Stepper motor step sequence (4-step sequence for full step)
static const unsigned char stepSequence[4] = {
  0b1010,  // Step 0: pins 41,43 HIGH; 40,42 LOW
  0b0110,  // Step 1: pins 40,43 HIGH; 41,42 LOW
  0b0101,  // Step 2: pins 40,42 HIGH; 41,43 LOW
  0b1001   // Step 3: pins 41,42 HIGH; 40,43 LOW
};

static int currentStep = 0;
static unsigned long lastStepTime = 0;
static const unsigned long STEP_DELAY_MS = 10;  // 10ms between steps (normal speed)
static const unsigned long STEP_DELAY_LOW_POWER_MS = 20;  // 20ms between steps (faster, visible movement while still saving some power)
static bool stepperEnabled = false;
static bool lowPowerMode = false;  // Enable low power mode to reduce current

void StepperMotorInit(void) {
  // Set all stepper pins as outputs
  *my_DDRG |= (1 << STEPPER_PIN_40_BIT);
  *my_DDRG |= (1 << STEPPER_PIN_41_BIT);
  *my_DDRL |= (1 << STEPPER_PIN_42_BIT);
  *my_DDRL |= (1 << STEPPER_PIN_43_BIT);
  
  // Turn all pins off initially
  *my_PORTG &= ~(1 << STEPPER_PIN_40_BIT);
  *my_PORTG &= ~(1 << STEPPER_PIN_41_BIT);
  *my_PORTL &= ~(1 << STEPPER_PIN_42_BIT);
  *my_PORTL &= ~(1 << STEPPER_PIN_43_BIT);
  
  currentStep = 0;
  stepperEnabled = false;
}

void StepperMotorControl(Status state) {
  if (state == DISABLED) {
    // Turn off stepper motor only in disabled state
    stepperEnabled = false;
    *my_PORTG &= ~(1 << STEPPER_PIN_40_BIT);
    *my_PORTG &= ~(1 << STEPPER_PIN_41_BIT);
    *my_PORTL &= ~(1 << STEPPER_PIN_42_BIT);
    *my_PORTL &= ~(1 << STEPPER_PIN_43_BIT);
  } else {
    // Enable stepper motor in all other states (IDLE, RUNNING, ERROR)
    stepperEnabled = true;
  }
}

void StepperMotorStep(void) {
  if (!stepperEnabled) {
    return;
  }
  
  unsigned long now = millis();
  // Use longer delay in low power mode to reduce current consumption
  unsigned long stepDelay = lowPowerMode ? STEP_DELAY_LOW_POWER_MS : STEP_DELAY_MS;
  
  if ((now - lastStepTime) >= stepDelay) {
    lastStepTime = now;
    
    // Get current step pattern
    unsigned char pattern = stepSequence[currentStep];
    
    // Apply pattern to pins
    // Pattern bits: bit3=pin43, bit2=pin42, bit1=pin41, bit0=pin40
    if (pattern & 0x01) {
      *my_PORTG |= (1 << STEPPER_PIN_40_BIT);
    } else {
      *my_PORTG &= ~(1 << STEPPER_PIN_40_BIT);
    }
    
    if (pattern & 0x02) {
      *my_PORTG |= (1 << STEPPER_PIN_41_BIT);
    } else {
      *my_PORTG &= ~(1 << STEPPER_PIN_41_BIT);
    }
    
    if (pattern & 0x04) {
      *my_PORTL |= (1 << STEPPER_PIN_42_BIT);
    } else {
      *my_PORTL &= ~(1 << STEPPER_PIN_42_BIT);
    }
    
    if (pattern & 0x08) {
      *my_PORTL |= (1 << STEPPER_PIN_43_BIT);
    } else {
      *my_PORTL &= ~(1 << STEPPER_PIN_43_BIT);
    }
    
    // Move to next step
    currentStep = (currentStep + 1) % 4;
  }
}

// Enable or disable low power mode (slower stepping = less current)
void StepperMotorSetLowPower(bool enable) {
  lowPowerMode = enable;
  // If enabling low power, also increase step delay immediately
  if (enable) {
    lastStepTime = millis();  // Reset timer to apply new delay
  }
}

