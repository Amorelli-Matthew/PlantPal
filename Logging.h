#ifndef LOGGING_H
#define LOGGING_H

//install lib first on system by searching
#include <RTClib.h>

//clock can live here too
#ifdef __cplusplus
extern "C" {
#endif
RTC_DS3231 rtc;

char daysofTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday" ,"Thursday", "Friday", "Saturday"};


#ifdef __cplusplus
}
#endif
#endif