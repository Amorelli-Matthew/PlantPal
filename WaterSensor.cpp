#include "WaterSensor.h"

//pointers to regs that interact with the sample
volatile unsigned char *my_ADMUX = (unsigned char *)0x007C;
volatile unsigned char *my_ADCSRB = (unsigned char *)0x007B;
volatile unsigned char *my_ADCSRA = (unsigned char *)0x007A;
volatile unsigned int *my_ADC_DATA = (unsigned int *)0x0078;  // ADCL/ADCH (right-adjusted)


void adc_init(void) {
  // ADCSRA: enable ADC, prescaler=128 for 16 MHz -> 125 kHz ADC clock
  *my_ADCSRA |= (1 << 7);                    // ADEN = 1
  *my_ADCSRA &= ~(1 << 5);                   // ADATE = 0 (no auto trigger)
  *my_ADCSRA &= ~(1 << 3);                   // ADIE  = 0 (no interrupt)
  *my_ADCSRA = (*my_ADCSRA & ~0x07) | 0x07;  // ADPS2:0 = 111

  // ADCSRB: free running off, MUX5=0 initially
  *my_ADCSRB &= ~(1 << 3);  // MUX5 = 0
  *my_ADCSRB &= ~0x07;      // ADTS2:0 = 000

  // ADMUX: AVcc ref, right-adjusted, start with channel 0
  *my_ADMUX &= ~(1 << 7);  // REFS1 = 0
  *my_ADMUX |= (1 << 6);   // REFS0 = 1  (AVcc with cap)
  *my_ADMUX &= ~(1 << 5);  // ADLAR = 0  (right adjust)
  *my_ADMUX &= ~0x1F;      // MUX4 = 00000 (ADC0)
}

unsigned int adc_read(unsigned char adc_channel) {
  // select channel (0..15)
  *my_ADMUX &= ~0x1F;        // clear MUX4:0
  *my_ADCSRB &= ~(1 << 3);   // clear MUX5
  if (adc_channel & 0x08) {  // channels 8..15 set MUX5
    *my_ADCSRB |= (1 << 3);
  }
  *my_ADMUX |= (adc_channel & 0x07);  // set low bits MUX2:0

  // start conversion
  *my_ADCSRA |= (1 << 6);                  // ADSC = 1
  while ((*my_ADCSRA & (1 << 6)) != 0) {}  // wait until ADSC clears

  // read 10-bit result (ADCL then ADCH; using 16-bit IO pointer)
  return *my_ADC_DATA;  // 0..1023 (right-adjusted)
}

int ReadWaterSensor() {
  //gather 8 samples for stability
  unsigned int sum = 0;
  for (int i = 0; i < AVG_SAMPLES; i++) {
    sum += adc_read(0);  // A0 = channel 0
  }
  //return the average of samples inorder to get a stable sample value
  return (unsigned int)(sum / AVG_SAMPLES);
}

// Method that checks how high the water is
bool waterlevelcheck(void) {
  // Get the current time
  currentTime = millis();

  // Read the water every few seconds
  if ((currentTime - lastWaterCheckTime) >= 3000UL) {
    lastWaterCheckTime = currentTime;

    WaterSensorValue = ReadWaterSensor();
    // Assume WaterSensorValue is updated elsewhere (ADC / ISR / WaterSensor module)
    //using custom printin via serial.h file
    if (WaterSensorValue > HIGH_LEVEL) {
      print("Water Level: High at ");
      println(WaterSensorValue);

    } else if (WaterSensorValue <= HIGH_LEVEL && WaterSensorValue >= MID_LEVEL) {
      print("Water Level: Medium at ");
      println(WaterSensorValue);

    } else {
      // If water level low, then set ProgramStatus to ERROR and set the error satus to low water
      ProgramStatus = ERROR;
      ErrorCode = ERR_LOW_WATER;
      print("Water Level: Low at ");
      println(WaterSensorValue);

      //return that the water level is low and that the program cannot continue
      return false;  
    }
  }

    //return that the water level check was an sucesses 
  return true;
}
