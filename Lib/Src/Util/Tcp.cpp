#include "pch.h"
#include <Tcp.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class TcpStartUp {
public:
  TcpStartUp();
  ~TcpStartUp();
private:
  bool initok;
};

TcpStartUp::TcpStartUp() {
  WSADATA     WsaData;
  INT         err = WSAStartup (0x0101, &WsaData);
  if(err == 0)  {
    initok = true;
  } else {
    initok = false;
    throw TcpException(getWSAErrorText(err));
  }
/*
  printf("desc:<%s> vendor:<%s> status:<%s> maxsockets:%d version:%d.%d\n"
    ,WsaData.szDescription
    ,WsaData.lpVendorInfo
    ,WsaData.szSystemStatus
    ,WsaData.iMaxSockets
    ,WsaData.wHighVersion
    ,WsaData.wVersion
    ,WsaData.iMaxUdpDg);
*/
}

TcpStartUp::~TcpStartUp() {
  if(initok) {
    WSACleanup();
  }
}

static TcpStartUp tcpInit;

SOCKET tcpOpen(unsigned short portnr, const char *hostName) {
  SOCKET      s = INVALID_SOCKET;
  SOCKADDR_IN remoteAddr;
  IN_ADDR     RemoteIpAddress;
  INT         err;
  INT         bufferSize = 2048;

  s = socket (AF_INET, SOCK_STREAM, 0);
  if(s == INVALID_SOCKET) {
    throwLastWSAErrorText();
  }
  try {
    err = setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&bufferSize, sizeof(bufferSize));
    if(err == SOCKET_ERROR) {
      throwLastWSAErrorText();
    }

    err = setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&bufferSize, sizeof(bufferSize));
    if(err == SOCKET_ERROR) {
      throwLastWSAErrorText();
    }

    RemoteIpAddress.s_addr = htonl(INADDR_LOOPBACK);

    if(hostName != NULL) {
      PHOSTENT host = gethostbyname(hostName);
      if(host) {
        memcpy(&RemoteIpAddress.s_addr,host->h_addr,host->h_length);
      } else {
        RemoteIpAddress.s_addr = inet_addr(hostName); /* perhaps 999.999.999.999 */
        if(RemoteIpAddress.s_addr == INADDR_NONE) {
          String strHostName = hostName;
          throw TcpException(format(_T("Unknown host:%s"), strHostName.cstr()).cstr());
        }
      }
    }

    ZeroMemory(&remoteAddr, sizeof(remoteAddr));

    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port   = htons(portnr);
    remoteAddr.sin_addr   = RemoteIpAddress;

    err = connect(s, (PSOCKADDR) &remoteAddr, sizeof(remoteAddr));
    if(err == SOCKET_ERROR) {
      throwLastWSAErrorText();
    }
    return s;
  } catch(...) {
    if(s != INVALID_SOCKET) {
      closesocket(s);
    }
    throw;
  }
}

SOCKET tcpCreate(unsigned short portnr) {
  SOCKET      listener = INVALID_SOCKET;
  SOCKADDR_IN localAddr;
  INT         err;

  listener = socket(AF_INET, SOCK_STREAM, 0);
  if(listener == INVALID_SOCKET) {
    throwLastWSAErrorText();
  }
  try {
    ZeroMemory(&localAddr, sizeof (localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port   = htons (portnr);

    err = bind(listener, (PSOCKADDR)&localAddr, sizeof(localAddr));
    if(err == SOCKET_ERROR) {
      throwLastWSAErrorText();
    }

    err = listen(listener, SOMAXCONN);
    if(err == SOCKET_ERROR) {
      throwLastWSAErrorText();
    }
    return listener;

  } catch(...) {
    if(listener != INVALID_SOCKET) {
      closesocket(listener);
    }
    throw;
  }
}

SOCKET tcpAccept(SOCKET listener) {
  SOCKET fd = accept(listener, NULL, NULL);
  if(fd == INVALID_SOCKET) {
    throwLastWSAErrorText();
  }
  return fd;
}

void tcpClose(SOCKET s) {
  closesocket(s);
}

#define MAXBLOCKSIZE 0x7fffffff

size_t tcpRead(SOCKET s, void *buffer, size_t size) {
  int n;
  for(size_t i = 0; i < size; i += n) {
    const UINT blockSize = (size - i > MAXBLOCKSIZE) ? MAXBLOCKSIZE : (UINT)(size - i);
    if((n = recv(s, (char*)buffer + i, blockSize, 0)) <= 0) {
      throwLastWSAErrorText();
    }
  }
  return size;
}

size_t tcpWrite(SOCKET s, const void *buffer, size_t size) {
  int n;
  for(size_t i = 0; i < size; i += n) {
    const UINT blockSize = (size - i > MAXBLOCKSIZE) ? MAXBLOCKSIZE : (UINT)(size - i);
    if((n = send(s, (char*)buffer + i, blockSize, 0)) <= 0) {
      throwLastWSAErrorText();
    }
  }
  return size;
}

bool tcpPoll(SOCKET s) {
  fd_set socketSet;
  struct timeval timeout;
  FD_ZERO(&socketSet);
  memset(&timeout,0,sizeof(timeout));
  FD_SET(s,&socketSet);
  if(select(1,&socketSet,NULL,NULL,&timeout) == SOCKET_ERROR) {
    throwLastWSAErrorText();
  }
  if(FD_ISSET(s, &socketSet)) {
    return true;
  }
  return false;
}

String getHostName() {
  char result[256];
  DWORD size = sizeof(result);
  gethostname(result,size);
  return result;
}
