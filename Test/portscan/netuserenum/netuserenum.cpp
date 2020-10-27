#include "stdafx.h"
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

static StringArray getUserAccounts(const TCHAR *serverName) {
  LPUSER_INFO_0 buf = nullptr;

  try {
    DWORD          level           = 0;
    DWORD          entryCount      = 0;
    DWORD          totalEntryCount = 0;
    DWORD          resumeHandle    = 0;
    NET_API_STATUS status;

    StringArray result;
    do {
      status = NetUserEnum(serverName
                          ,level
                          ,FILTER_NORMAL_ACCOUNT // global users
                          ,(LPBYTE*)&buf
                          ,MAX_PREFERRED_LENGTH
                          ,&entryCount
                          ,&totalEntryCount
                          ,&resumeHandle);
      if(status == NERR_Success || status == ERROR_MORE_DATA) {
        if(buf == nullptr) {
          throwException(_T("An access violation has occurred"));
        }
        for(DWORD i = 0; i < entryCount; i++) {
          result.add(buf[i].usri0_name);
        }
      } else {
        throwException(_T("getUserAccounts:NetUserEnum failed:errorCode=%#lx"),status);
      }
      if(buf != nullptr) {
        NetApiBufferFree(buf);
        buf = nullptr;
      }
    } while(status == ERROR_MORE_DATA);

    return result;

  } catch(Exception e) {
    if(buf != nullptr) {
      NetApiBufferFree(buf);
    }
    throw;
  }
}

int _tmain(int argc, TCHAR **argv) {
  try {
    StringArray list = getUserAccounts(_T("localhost"));
    for(size_t i = 0; i < list.size(); i++) {
      const String &user = list[i];
      _tprintf(_T("%s\n"), user.cstr());
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

