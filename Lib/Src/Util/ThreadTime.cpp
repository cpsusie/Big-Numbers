#include "pch.h"
#include <ThreadBase.h>
#include <Date.h>

double getThreadTime(HANDLE hThread) {
  FILETIME crtime, exittime, kernelTime, userTime;
  if(!GetThreadTimes((hThread == INVALID_HANDLE_VALUE) ? GetCurrentThread() : hThread, &crtime, &exittime, &kernelTime, &userTime)) {
    return 0;
  }
  return fileTimeToTotalTime(kernelTime, userTime);
}
