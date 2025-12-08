#include "Logging.h"

void initLogging()
{
  rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}


void RTCTest()
{
  DateTime now = rtc.now();
print(now.year(), 10);
print("/");
print(now.month(),10);
print("/");
print(now.day(),10);
print(" (");
print(daysOfTheWeek[now.dayOfTheWeek()]);
print(") ");
print(now.hour(),10);
print(":");
print(now.minute(),10);
print(":");
print(now.second(),10);
print_crlf();
delay(3000);
}