#include "pch.h"
#include <ProcessTools.h>

ProcessInfo::ProcessInfo(DWORD processId) {
  m_processId = processId;
  HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
  if(nullptr == hProcess) {
    return;
  }
  try {
    m_commandLine = getProcessCommandLine(hProcess);
  } catch(Exception e) {
    m_commandLine = e.what();
  }
  try {
    CompactArray<HMODULE> moduleArray = getModules(hProcess);
    for(int i = 0; i < (int)moduleArray.size(); i++ ) {
      TCHAR name[MAX_PATH];
      if(GetModuleFileNameEx(hProcess, moduleArray[i], name, ARRAYSIZE(name))) {
        struct _stat st;
        UINT size = 0;
        if(_tstat(name, &st) >= 0) {
          size = st.st_size;
        }
        m_moduleArray.add(ModuleInfo(name, size));
      }
    }
  } catch(...) {
    // ignore
  }
  CloseHandle(hProcess);
}

String ProcessInfo::getExePath() const {
  return m_moduleArray.size() > 0 ? m_moduleArray[0].getPath() : EMPTYSTRING;
}
