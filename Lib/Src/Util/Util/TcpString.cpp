#include "pch.h"
#include <Tcp.h>

String tcpReadString(SOCKET s) {
  int len;
  tcpRead(s, &len, sizeof(len));
  if(len == 0) {
    return "";
  }
  TCHAR *buf = NULL;
  try {
    buf = new TCHAR[len+1];
    tcpRead(s, buf, len*sizeof(TCHAR));
    buf[len] = 0;
    const String result = buf;
    delete[] buf;
    buf = NULL;
    return result;
  } catch(...) {
    delete[] buf;
    buf = NULL;
    throw;
  }
}

void tcpWriteString(SOCKET s, const String &str) {
  const int len = str.length();
  tcpWrite(s, &len, sizeof(len));
  if(str.length() > 0) {
    tcpWrite(s, str.cstr(), len*sizeof(TCHAR));
  }
}

