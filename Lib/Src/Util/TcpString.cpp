#include "pch.h"
#include <Tcp.h>

#define DEFAULTBUFSIZE 1024

String tcpReadString(SOCKET s) {
  UINT len;
  tcpRead(s, &len, sizeof(len));
  if(len == 0) {
    return EMPTYSTRING;
  }
  TCHAR tmpBuf[DEFAULTBUFSIZE+1];
  TCHAR *buf = nullptr;
  try {
    buf = (len <= DEFAULTBUFSIZE) ? tmpBuf : new TCHAR[len+1];
    tcpRead(s, buf, len*sizeof(TCHAR));
    buf[len] = 0;
    const String result = buf;
    if(buf != tmpBuf) delete[] buf;
    buf = nullptr;
    return result;
  } catch(...) {
    if(buf != tmpBuf) delete[] buf;
    buf = nullptr;
    throw;
  }
}

void tcpWriteString(SOCKET s, const String &str) {
  if(str.length() >= UINT_MAX) {
    throwInvalidArgumentException(__TFUNCTION__,__T("String too long. must not exceed %lu"), UINT_MAX-1);
  }
  const UINT len = (UINT)str.length();
  tcpWrite(s, &len, sizeof(len));
  if(len > 0) {
    tcpWrite(s, str.cstr(), len*sizeof(TCHAR));
  }
}
