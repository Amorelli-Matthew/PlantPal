#ifndef WATERSENSOR_H
#define WATERSENSOR_H

//Use pin A0 for the S pin on the little sensor and plug plus and minus into the breadboard's plus and minuses respectively

#define AVG_SAMPLES 8

// water full at 385
//water medium at 360 on the sensor
#define HIGH_LEVEL 385  //380 via arduino 5 port but 385 if plugged into breadboard and it has its own power supply
#define MID_LEVEL 360

//ADC registers for water
extern volatile unsigned char *my_ADMUX;
extern volatile unsigned char *my_ADCSRB;
extern volatile unsigned char *my_ADCSRA;
extern volatile unsigned int *my_ADC_DATA;  // ADCL/ADCH (right-adjusted)

//variable to store water senesor value
extern unsigned int WaterSensorValue;

//methods to initilize water sensor and read the sensor
void adc_init(void);
unsigned int adc_read(unsigned char adc_channel);

#endif
