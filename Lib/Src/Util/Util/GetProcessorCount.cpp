#include "pch.h"
#include <CPUInfo.h>

int getProcessorCount() {
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return info.dwNumberOfProcessors;
}
