#pragma once

#include <winsock.h>
#include "PragmaLib.h"
#include "Exception.h"

SOCKET tcpOpen(   unsigned short portnr, const char *hostName = NULL);
SOCKET tcpCreate( unsigned short portnr);
SOCKET tcpAccept( SOCKET listener);
void   tcpClose(  SOCKET socket);

int    tcpRead(  SOCKET s,       void *buffer, int size);
int    tcpWrite( SOCKET s, const void *buffer, int size);
bool   tcpPoll(  SOCKET s);

String tcpReadString( SOCKET s);
void   tcpWriteString(SOCKET s, const String &str);

String getWSAErrorText(int errorCode);
inline String getLastWSAErrorText() {
  return getWSAErrorText(WSAGetLastError());
}

class TcpException : public Exception {
public:
  TcpException(const String &msg) : Exception(msg) {
  }
};

inline void throwLastWSAErrorText() {
  throw TcpException(getLastWSAErrorText());
}

#pragma comment(lib, "wsock32.lib")

