#include "Serial.h"
#include "WaterSensor.h"
#include "LCDScreen.h"

// water full at 385
//watter medium at 
#define HIGH_LEVEL 385   //380 via arduino 5 port but 385 if plugged into breadboard and it has its own power supply
#define MID_LEVEL 360

enum Status {
  Disabled =0;
  Green = 1;
  Running = 2;
  Error = 3;
}

//Optional smoothing
// Small average to stabilize readings
static const int AVG_SAMPLES = 8;


void setup() {
  U0init(9600);   
  adc_init();   
}

void loop() {

  //gather a few samples for stability
  unsigned long sum = 0;
  for (int i = 0; i < AVG_SAMPLES; i++) {
    sum += adc_read(0);         // A0 = channel 0
  }
  unsigned int value = (unsigned int)(sum / AVG_SAMPLES);

  if ( ( value > HIGH_LEVEL ) ) 
  {
    uart_print_str("Water Level: High at ");
    uart_print_uint(value); 
  }

 else if ( HIGH_LEVEL >= value && value >= MID_LEVEL)
   {
        uart_print_str("Water Level: Medium at ");
        uart_print_uint(value); 
   }
  
    else 
    {
     uart_print_str("Water Level: Low at ");
     uart_print_uint(value);     
     uart_print_crlf();
    }

  // small software delay to slow printing
  for (volatile long i = 0; i < 120000; i++) { }
}


