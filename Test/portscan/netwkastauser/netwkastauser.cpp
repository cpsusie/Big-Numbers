#include "stdafx.h"
#include <assert.h>
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

StringArray getWorkStationUsers(TCHAR *serverName) {
  LPWKSTA_USER_INFO_0 buf  = NULL;

  try {
    const DWORD    level           = 0;
    DWORD          entryCount      = 0;
    DWORD          totalEntryCount = 0;
    DWORD          resumeHandle    = 0;
    NET_API_STATUS status;

    StringArray    result;

    do {
      status = NetWkstaUserEnum(serverName
                               ,level
                               ,(LPBYTE*)&buf
                               ,MAX_PREFERRED_LENGTH
                               ,&entryCount
                               ,&totalEntryCount
                               ,&resumeHandle);
      if(status == NERR_Success || status == ERROR_MORE_DATA) {
        if(buf == NULL) {
          throwException(_T("An access violation has occurred"));
        }

        for(DWORD i = 0; i < entryCount; i++) {
          result.add(buf[i].wkui0_username);
        }
      } else {
        throwException(_T("getWorkStationUsers:NetWkstaUserEnum failed:ErrorCode=%#lx"),status);
      }
      if (buf != NULL) {
        NetApiBufferFree(buf);
        buf = NULL;
      }
    } while(status == ERROR_MORE_DATA);

    return result;

  } catch(Exception e) {
    if(buf != NULL) {
      NetApiBufferFree(buf);
    }
    throw;
  }
}

int _tmain(int argc, TCHAR **argv) {
  try {
    StringArray list = getWorkStationUsers(NULL);
    for(size_t i = 0; i < list.size(); i++) {
      const String &user = list[i];
      _tprintf(_T("%s\n"),user.cstr());
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

