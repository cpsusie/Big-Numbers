#pragma once

#include "Array.h"
#include "CompactArray.h"
#include <psapi.h>

HANDLE                  getProcessHandle(     DWORD processID = -1);
PROCESS_MEMORY_COUNTERS getProcessMemoryUsage(DWORD processID = -1); // processID == -1 gives Usage for current process

void enableTokenPrivilege(LPCTSTR privilege, bool enable);
String getProcessCommandLine(       HANDLE hProcess = nullptr);
CompactArray<HMODULE> getModules(   HANDLE hProcess = nullptr);
CompactArray<DWORD>   getProcessIds();
// microseconds. if process == nullptr, return time for current Process
double getProcessTime(              HANDLE hProcess = nullptr);

class ResourceCounters {
public:
  DWORD m_gdiObjectCount;
  DWORD m_userObjectCount;
};

ResourceCounters getProcessResources(DWORD processID = -1);          // processID == -1 gives ResourceCounters for current process

class ModuleInfo {
private:
  const String m_path;
  const DWORD  m_size;
public:
  ModuleInfo(const String &path, DWORD size) : m_path(path), m_size(size) {
  }

  inline const String &getPath() const {
    return m_path;
  }

  inline DWORD getSize() const {
    return m_size;
  }
};

class ProcessInfo {
private:
  DWORD             m_processId;
  String            m_commandLine;
  Array<ModuleInfo> m_moduleArray;
public:
  ProcessInfo(DWORD processId);

  inline DWORD getProcessId() const {
    return m_processId;
  }

  String getExePath() const;

  inline const String &getCommandLine() const {
    return m_commandLine;
  }

  inline const Array<ModuleInfo> &getModuleArray() const {
    return m_moduleArray;
  }
};

typedef enum {
  PRCLASS_IDLE          = IDLE_PRIORITY_CLASS
 ,PRCLASS_BELOW_NORMAL  = BELOW_NORMAL_PRIORITY_CLASS
 ,PRCLASS_NORMAL        = NORMAL_PRIORITY_CLASS
 ,PRCLASS_ABOVE_NORMAL  = ABOVE_NORMAL_PRIORITY_CLASS
 ,PRCLASS_HIGH_PRIORITY = HIGH_PRIORITY_CLASS
 ,PRCLASS_REALTIME      = REALTIME_PRIORITY_CLASS
} PriorityClass;

void          setPriorityClass(PriorityClass prClass, HANDLE hProcess = nullptr);
PriorityClass getPriorityClass(                       HANDLE hProcess = nullptr);
