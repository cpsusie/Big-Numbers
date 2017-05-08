#include "pch.h"
#include <Tcp.h>

#define DEFAULTBUFSIZE 1024

String tcpReadString(SOCKET s) {
  size_t len;
  tcpRead(s, &len, sizeof(len));
  if(len == 0) {
    return EMPTYSTRING;
  }
  TCHAR tmpBuf[DEFAULTBUFSIZE+1];
  TCHAR *buf = NULL;
  try {
    buf = (len <= DEFAULTBUFSIZE) ? tmpBuf : new TCHAR[len+1];
    tcpRead(s, buf, len*sizeof(TCHAR));
    buf[len] = 0;
    const String result = buf;
    if(buf != tmpBuf) delete[] buf;
    buf = NULL;
    return result;
  } catch(...) {
    if(buf != tmpBuf) delete[] buf;
    buf = NULL;
    throw;
  }
}

void tcpWriteString(SOCKET s, const String &str) {
  const size_t len = str.length();
  tcpWrite(s, &len, sizeof(len));
  if(len > 0) {
    tcpWrite(s, str.cstr(), len*sizeof(TCHAR));
  }
}
