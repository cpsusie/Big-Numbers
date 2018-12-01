#pragma once

#include <winsock.h>
#include "PragmaLib.h"
#include "Exception.h"

SOCKET tcpOpen(   unsigned short portnr, const char *hostName = NULL);
SOCKET tcpCreate( unsigned short portnr);
SOCKET tcpAccept( SOCKET listener);
void   tcpClose(  SOCKET socket);

size_t tcpRead(  SOCKET s,       void *buffer, size_t size);
size_t tcpWrite( SOCKET s, const void *buffer, size_t size);
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

#ifndef _WINSOCK2API_

#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__

inline unsigned __int64 htonll(unsigned __int64 hostlong) {
  return ((unsigned __int64)htonl((u_long)((hostlong) & 0xffffffff)) << 32) | htonl((u_long)((hostlong) >> 32));
}

inline unsigned __int64 ntohll(unsigned __int64 netlong) {
  return ((unsigned __int64)ntohl((u_long)((netlong) & 0xffffffff)) << 32) | ntohl((u_long)((netlong) >> 32));
}

#else

#define htonll(hostlong) hostlong
#define ntohll(netlong ) netlong

#endif

#endif // _WINSOCK2API_

#pragma comment(lib, "wsock32.lib")

