#ifndef STATUSCODES_H
#define STATUSCODES_H

enum Status {
  DISABLED = 0,
  IDLE = 1,
  RUNNING = 2,
  ERROR = 3,
};

enum ErrorCodes {
  NONE = 0,
  //1xx: Sensor & Data Errors
  ERR_SOIL_ADC_RANGE = 101,
  ERR_SOIL_ADC_SATURATED = 102,
  ERR_DHT_TIMEOUT = 111,
  ERR_DHT_SENSOR_FAULT = 112,

  //2xx: Pump / Motor Errors
  ERR_PUMP_TIMEOUT = 201,
  ERR_PUMP_NO_CURRENT = 202,
  ERR_PUMP_OVERCURRENT = 203,

  //3xx: Environmental Condition Errors
  ERR_LOW_WATER = 301,
  ERR_SOIL_NOT_RECOVERING = 302,

  //4xx: System / Logic Errors
  ERR_RTC_FAILURE = 401,
  ERR_INIT_FAILURE = 402,
  ERR_UNEXPECTED_STATE = 403,
  ERR_UNEXPECTED_RESTART = 404,
};


extern Status ProgramStatus;
extern ErrorCodes ErrorCode;
#endif