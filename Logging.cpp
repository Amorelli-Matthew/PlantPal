#include "Logging.h"

void initLogging()
{
  rtc.begin();
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void getTimeViaRTC(char *outBuffer, size_t bufferSize)
{
    DateTime now = rtc.now();

    snprintf(outBuffer, bufferSize,
             "%04d-%02d-%02d %02d:%02d:%02d",
             now.year(), now.month(), now.day(),
             now.hour(), now.minute(), now.second());
}
