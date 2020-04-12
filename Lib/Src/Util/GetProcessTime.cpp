#include "pch.h"
#include <ProcessTools.h>
#include <Date.h>

double getProcessTime(HANDLE hProcess) {
  FILETIME crtime, exittime, kernelTime, userTime;

  if(!GetProcessTimes(hProcess ? hProcess : GetCurrentProcess(), &crtime, &exittime, &kernelTime, &userTime)) {
    return 0;
  }
  return fileTimeToTotalTime(kernelTime, userTime);
}
