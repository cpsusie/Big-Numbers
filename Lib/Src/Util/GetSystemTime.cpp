#include "pch.h"
#include <ProcessTools.h>

double getSystemTime() {
  FILETIME sysTime;
  GetSystemTimeAsFileTime(&sysTime);
  return ((double)sysTime.dwHighDateTime * (double)0x100000000 + (double)sysTime.dwLowDateTime) / 10;
}
