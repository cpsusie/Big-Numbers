#include "pch.h"
#include <Date.h>

double fileTimeToTotalTime(const FILETIME &kernelTime, const FILETIME &userTime) {
  const double ht = (double)kernelTime.dwHighDateTime + userTime.dwHighDateTime;
  const double lt = (double)kernelTime.dwLowDateTime + userTime.dwLowDateTime;

  return (ht * (double)0x100000000 + lt) / 10;
}
