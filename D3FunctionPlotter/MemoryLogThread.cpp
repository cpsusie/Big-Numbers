#include "stdafx.h"
#include <ProcessTools.h>
#include <Date.h>
#include "MemoryLogThread.h"

UINT MemoryLogThread::run() {
  setDeamon(true);
  for(;;) {
    const PROCESS_MEMORY_COUNTERS mem = getProcessMemoryUsage();
    const ResourceCounters        res = getProcessResources();
    debugLog(_T("%s %8zu %4d %4d\n")
            ,Timestamp().toString(hhmmss).cstr()
            ,mem.WorkingSetSize
            ,res.m_userObjectCount
            ,res.m_gdiObjectCount);
    Sleep(10000);
  }
};
