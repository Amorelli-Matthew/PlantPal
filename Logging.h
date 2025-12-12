#ifndef LOGGING_H
#define LOGGING_H
#include "Serial.h"
#include <string.h>

static unsigned long systemStartTime = 0;

void initLogging();
void getTimeViaRTC(char *outBuffer, size_t bufferSize);
#endif