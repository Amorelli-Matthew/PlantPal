#include "FanMotor.h"


// Declare logEvent function (defined in PlantPal.ino)
extern void logEvent(const char *msg, const char* timestamp);

// Arduino Mega D31 -> PC6 (Port C bit 6)
#define FAN_BIT   6

// Port C registers (ATmega2560)
volatile unsigned char *my_DDRC  = (unsigned char *)0x27;  // DDRC
volatile unsigned char *my_PORTC = (unsigned char *)0x28;  // PORTC
volatile unsigned char *my_PINC  = (unsigned char *)0x26;  // PINC
static bool fanEnabled = false;
static Status lastState = DISABLED;

void FanMotorInit(void)
{
  // PC6 as output
  *my_DDRC |= (1 << FAN_BIT);

  // Fan off initially (active HIGH => LOW = OFF)
  *my_PORTC &= ~(1 << FAN_BIT);

  fanEnabled = false;
  lastState = DISABLED;

  char timestamp[30];
  getTimeViaRTC(timestamp, sizeof(timestamp));
  logEvent("Fan motor initialized on pin 31 (PC6)", timestamp);
}

void FanMotorControl(Status state)
{
  if (state == RUNNING)
  {
    // ON: set PC6 high
    *my_PORTC |= (1 << FAN_BIT);
    fanEnabled = true;

    if (lastState != RUNNING)
    {
      char timestamp[30];
      getTimeViaRTC(timestamp, sizeof(timestamp));
      logEvent("Fan motor: PC6 set HIGH (ON) - RUNNING state", timestamp);

      // Readback using PINC
      unsigned char pinState = ((*my_PINC & (1 << FAN_BIT)) != 0) ? 1 : 0;
      char verifyMsg[60];
      snprintf(verifyMsg, sizeof(verifyMsg), "Fan pin readback (PC6): %u (should be 1)", pinState);
      logEvent(verifyMsg, timestamp);
    }
  }
  else
  {
    // OFF: set PC6 low
    *my_PORTC &= ~(1 << FAN_BIT);
    fanEnabled = false;

    if (lastState == RUNNING)
    {
      char timestamp[30];
      getTimeViaRTC(timestamp, sizeof(timestamp));
      logEvent("Fan motor: PC6 set LOW (OFF)", timestamp);
    }
  }

  lastState = state;
}
