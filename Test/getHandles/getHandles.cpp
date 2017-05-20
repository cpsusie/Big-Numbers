#include "stdafx.h"

int main(int argc, char **argv) {
  try {
    CompactArray<DWORD> processIds = getProcessIds();
    enableTokenPrivilege(SE_DEBUG_NAME,true);

    for(size_t i = 1; i < processIds.size(); i++) {
      HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processIds[i]);
      if(hProcess != NULL) {
        CloseHandle(hProcess);
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    return -1;
  }

  return 0;
}
