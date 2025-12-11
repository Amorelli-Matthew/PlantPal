#ifndef LOGGING_H
#define LOGGING_H
#include "Serial.h"
#include <string.h>
//install lib first on system by searching
#include <RTClib.h>

//clock can live here too
static RTC_DS3231 rtc;

static const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
"Friday", "Saturday"};

void initLogging();
void getTimeViaRTC(char *outBuffer, size_t bufferSize);
#endif