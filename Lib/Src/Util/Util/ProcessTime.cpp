#include "pch.h"

static double getTotalTime(const FILETIME &kernelTime, const FILETIME &userTime) {
  const double ht = (double)kernelTime.dwHighDateTime + userTime.dwHighDateTime;
  const double lt = (double)kernelTime.dwLowDateTime  + userTime.dwLowDateTime;
  
  return (ht * (double)0x100000000 + lt) / 10;
}

double getProcessTime(HANDLE process) {
  FILETIME crtime,exittime,kernelTime,userTime;

  if(!GetProcessTimes(process?process:GetCurrentProcess(),&crtime,&exittime,&kernelTime,&userTime)) {
    return 0;
  }
  return getTotalTime(kernelTime,userTime);
}

double getThreadTime(HANDLE thread) {
  FILETIME crtime,exittime,kernelTime,userTime;
  if(!GetThreadTimes(thread?thread:GetCurrentThread(),&crtime,&exittime,&kernelTime,&userTime)) {
    return 0;
  }
  return getTotalTime(kernelTime,userTime);
}
  
double getSystemTime() {
  FILETIME sysTime;
  GetSystemTimeAsFileTime(&sysTime);
  return ((double)sysTime.dwHighDateTime * (double)0x100000000 + (double)sysTime.dwLowDateTime) / 10;
}
