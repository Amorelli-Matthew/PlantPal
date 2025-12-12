#include "Logging.h"
#include <Arduino.h>

void initLogging()
{
  // Initialize system start time for relative timestamps
  systemStartTime = millis();
}

void getTimeViaRTC(char *outBuffer, size_t bufferSize)
{
    // Use millis() to create relative timestamps (time since system start)
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - systemStartTime;
    
    // Convert to hours:minutes:seconds
    unsigned long seconds = elapsed / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    // Format as HH:MM:SS (relative time since startup)
    snprintf(outBuffer, bufferSize,
             "Uptime: %02lu:%02lu:%02lu",
             hours, minutes, seconds);
}
