// getdcname.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <lm.h>

#pragma comment(lib, "netapi32.lib")

String getDomainControllerName(const TCHAR *serverName, const TCHAR *domainName) {
  LPBYTE buf = NULL;
  NET_API_STATUS error = NetGetDCName(NULL,NULL,&buf);
  if(error != NERR_Success) {
    switch(error) {
    case NERR_DCNotFound   : throwException(_T("getDomainControllerName:Could not find the domain controller for the domain,"));
    case ERROR_INVALID_NAME: throwException(_T("getDomainControllerName:The name could not be found."));
    default                : throwException(_T("getDomainControllerName(%s,%s):Unknown errorcode=%#lx"),serverName, domainName, error);
    }
  }
  String result = (char*)buf;
  if(buf != NULL) {
    NetApiBufferFree(buf);
  }
  return result;
}

static void usage() {
  _ftprintf(stderr,_T("Usage:getdcname [-help] [-?] [servername] [domainname]\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR *serverName = NULL;
  TCHAR *domainName = NULL;

  argv++;
  if(*argv && (_tcscmp(*argv,_T("-?")) == 0 || _tcsicmp(*argv,_T("-help")) == 0)) {
    usage();
  }
  if(*argv) {
    serverName = *(argv++);
  }
  if(*argv) {
    domainName = *(argv++);
  }
  if(*argv) {
    usage();
  }
  try {
    String domainControler = getDomainControllerName(serverName,domainName);
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }

  return 0;
}
