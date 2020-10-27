//------------------------------------------------------------------
//  Ports.c - TCP port scanner to list active ports. Ton Plooy 1997
//  Compile and link with 'cl ports.c -link wsock32.lib icmp.lib'
//------------------------------------------------------------------
#include "StdAfx.h"
#include <winsock.h>
#include <IPExport.h>
#include <icmpapi.h>

#define LINELEN                     16
#define MAX_TIMEOUT                 1500

#pragma comment(lib, "ws2_32.lib" )
#pragma comment(lib, "Iphlpapi.lib")

// Do a hexdump with the supplied data to stdout
void HexDump(const BYTE *Data, int nLen) {
  int   i, nCount;
  BYTE  Line[LINELEN];
  ULONG ulOffset = 0L;

  while (nLen > 0L) {
    nCount = nLen > LINELEN ? LINELEN : nLen;
    memcpy(Line, Data + ulOffset, nCount);
    printf("%04lX  ",  ulOffset);
    for (i = 0; i < LINELEN; i++) {
      (i < nCount) ? printf("%02X ", (unsigned int)Line[i]) :
                      printf("   ");
    }
    printf(" ");
    for (i = 0; (i < LINELEN) && (i < nCount); i++) {
      printf("%c", isprint(Line[i]) ? Line[i] : '.');
    }
    printf("\n");
    nLen     -= nCount;
    ulOffset += nCount;
  }
  return ;
}

// As connect() but with timeout setting.
int connectex(SOCKET s, const struct sockaddr *name, int namelen, long timeout) {
  int            rc = 0;
  ULONG          ulB;
  struct timeval Time;
  fd_set         FdSet;

  ulB = TRUE; // Set socket to non-blocking mode
  ioctlsocket(s, FIONBIO, &ulB);

  if (connect(s, name, sizeof(SOCKADDR)) == SOCKET_ERROR) {
    if (WSAGetLastError() == WSAEWOULDBLOCK) {
      // now wait for the specified time
      FD_ZERO(&FdSet);
      FD_SET(s, &FdSet);

      Time.tv_sec  = timeout / 1000L;
      Time.tv_usec = (timeout % 1000) * 1000;
      rc = select(0, nullptr, &FdSet, nullptr, &Time);
    }
  }

  ulB = FALSE; // Restore socket to blocking mode
  ioctlsocket(s, FIONBIO, &ulB);

  return (rc > 0) ? 0 : SOCKET_ERROR;
}

// Close socket. If reset is true we do a hard close.
int closesocketex(SOCKET sock, bool reset) {
  int    rc;
  LINGER linger;

  if (sock == INVALID_SOCKET) {
    return 0;
  }

  if (reset) {
    // Enable linger with a zero timeout
    linger.l_onoff  = TRUE;
    linger.l_linger = 0;
    setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));
  }
  rc = closesocket(sock);

  return rc;
}

// Check if the IP port is listening on the given machine
int CheckPort(DWORD ipaddr, USHORT usPort, ULONG timeout) {
  int                 nLen;
  BYTE                Data[512];
  SOCKET              s;
  fd_set              FdSet;
  struct timeval      Time = { 1, 0 };
  struct sockaddr_in  SrvAddr, CliAddr;

  if ((s = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
    printf("Error: unable to create socket\n");
    return 0;
  }

  // Bind the socket
  CliAddr.sin_family      = AF_INET;
  CliAddr.sin_addr.s_addr = INADDR_ANY;
  CliAddr.sin_port        = 0;
  bind(s, (PSOCKADDR)&CliAddr, sizeof(CliAddr));

  // Connect
  SrvAddr.sin_family      = AF_INET;
  SrvAddr.sin_addr.s_addr = ipaddr;
  SrvAddr.sin_port        = htons(usPort);
  if(connectex(s, (LPSOCKADDR)&SrvAddr, sizeof(SrvAddr), timeout) == SOCKET_ERROR) {
    closesocketex(s, TRUE);
    return 0;
  }
  // Connect succeeded, so the port is listening
  printf("Port %d is listening\n", usPort);

  // Send some data to the port to see if we get anything
  // back. If we do, hexdump the received data.
  memset(Data, 0, sizeof(Data));
  Data[0] = 'X'; // Send just one byte
  if (send(s, (char*)Data, 1, 0) > 0) {
    FD_ZERO(&FdSet);
    FD_SET(s, &FdSet);
    Time.tv_sec  = timeout / 1000L;
    Time.tv_usec = (timeout % 1000) * 1000;
    if ( (select(0, &FdSet, nullptr, nullptr, &Time) > 0)
      && ((nLen = recv(s, (char*)Data, sizeof(Data), 0)) > 0)) {
      HexDump(Data, nLen);
    }
  }
  closesocketex(s, false);
  return 1;
}

ULONG ping(IPAddr ipaddr, ULONG ulTimeOut) {
  ULONG   ulTime = 0;
  HANDLE  hIcmp;
  static char pingData[20] = "Crunch Technologies";
  const  WORD requestSize  = (SHORT)strlen(pingData);
  const  WORD replySize    = sizeof(ICMP_ECHO_REPLY) + requestSize;
  char       *replyBuffer  = new char[replySize];

  try {
    hIcmp = IcmpCreateFile();
    if (IcmpSendEcho(hIcmp
                    ,ipaddr
                    ,pingData
                    ,requestSize
                    ,nullptr
                    ,replyBuffer
                    ,replySize
                    ,ulTimeOut) == 1) {
      // Return at least 1 msec
      const ICMP_ECHO_REPLY *echo = (const ICMP_ECHO_REPLY*)replyBuffer;
      ulTime = max(echo->RoundTripTime, 1);
      delete[] replyBuffer;
    }
  } catch (...) {
    delete[] replyBuffer;
    throw;
  }
  IcmpCloseHandle(hIcmp);
  return ulTime;
}

// String to IP: optionally resolve a host name and convert
// an ACII IP dot address to its binary network form.
IPAddr stoip(const char *lpIP) {
  int    i;
  bool   bDot = true;
  char   szBuffer[64];
  IPAddr ipaddr = 0L;

  if ((lpIP[0] != '\0') && (!isdigit(lpIP[0]))) {
    // Name is specified, lookup ip address
    struct hostent *pHost;
    if ((pHost = gethostbyname((LPSTR)lpIP)) != nullptr) {
      memcpy(&ipaddr, pHost->h_addr_list[0], 4);
    }
  } else {
    // Address specified. Remove all preceding zeros first,
    // inet_adr will interpret it as an octal number.
    i = 0;
    while (*lpIP) {
      while ((*(lpIP) == '0') && (isdigit(*(lpIP + 1))) && bDot) {
        lpIP++;
      }
      bDot = (*lpIP == '.');
      szBuffer[i++] = *lpIP++;
    }
    szBuffer[i] = 0;
    ipaddr = inet_addr(szBuffer);
  }
  return ipaddr;
}

int main(int argc, char *argv[]) {
  IPAddr       ipaddr;
  DWORD        missed = 0;
  WORD         wVersionRequested;
  WSADATA      wsaData;

  if (argc < 2) {
    printf("Usage: ports <ip-address>\n");
    return 0;
  }

  /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
  wVersionRequested = MAKEWORD(2, 2);

  WSAStartup(wVersionRequested, &wsaData);

  // Resolve machine name or translate address
  if ((ipaddr = stoip(argv[1])) == 0) {
    printf("Error: address %s unknown\n", argv[1]);
    WSACleanup();
    return 0;
  }
  HANDLE hCon = GetStdHandle(STD_INPUT_HANDLE);

  // Calculate avarage echo reply time over four pings
  int tries = 0;
  int time  = 0;
  do {
    DWORD p;
    if ((p = ping(ipaddr, MAX_TIMEOUT)) == 0) {
      missed++;
    } else {
      time += p;
    }
  } while (++tries < 4);

  if (time == 0) {
    printf("No reply from %s...\n", argv[1]);
  } else {
    // At least one response seen, continue
    time = (time + (missed * MAX_TIMEOUT)) / tries;
    printf("Avarage reply time from %s is %lu msecs\n", argv[1], time);

    // Set port connect timeout at ping time plus 50 %
    time = min(time + (time / 2), MAX_TIMEOUT);
    printf("Starting portscan (press space to stop)...\n");
    bool bStop = false;
    for (int i = 1; (i <= 1024) && !bStop; i++) {
      if ((i % 128) == 0) {
        printf("Scanning port %4d...\n", i);
      }

      CheckPort(ipaddr, (USHORT)i, time);

      // Check keyboard input, stop if space bar is pressed
      INPUT_RECORD Rec;
      DWORD        dwRead;
      PeekConsoleInput(hCon, &Rec, 1, &dwRead);
      if ((dwRead > 0) && (Rec.EventType == KEY_EVENT)) {
        ReadConsoleInput(hCon, &Rec, 1, &dwRead);
        if (Rec.Event.KeyEvent.wVirtualKeyCode == ' ') {
          bStop = true;
        }
      }
    }
  }
  WSACleanup();
  return 0;
}
