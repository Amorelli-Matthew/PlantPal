#ifndef WATERSENSOR_H
#define WATERSENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define AVG_SAMPLES 8
//ADC registers
extern volatile unsigned char *my_ADMUX; 
extern volatile unsigned char *my_ADCSRB; 
extern volatile unsigned char *my_ADCSRA;
extern volatile unsigned int  *my_ADC_DATA; // ADCL/ADCH (right-adjusted)

static unsigned int WaterSensorValue = 0;

 void adc_init(void);
 unsigned int adc_read(unsigned char adc_channel);


#ifdef __cplusplus
}
#endif
#endif