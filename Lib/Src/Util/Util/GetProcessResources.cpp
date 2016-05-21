#include "pch.h"
#include <ProcessTools.h>

//#pragma comment(lib, "User32.lib")

ResourceCounters getProcessResources(DWORD processID) {
  HANDLE hProcess = getProcessHandle(processID);
  ResourceCounters result;

  result.m_gdiObjectCount  = GetGuiResources(hProcess, GR_GDIOBJECTS);
  result.m_userObjectCount = GetGuiResources(hProcess, GR_USEROBJECTS);

  CloseHandle(hProcess);

  return result;
}

