#include "stdafx.h"
#include <Tcp.h>

static void usage() {
  _ftprintf(stderr,_T("testtcpclient [-l] [-hhostname]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  char *servername = NULL;
  bool loop = false;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'l': loop = true; continue;
      case 'h': servername = cp+1; break;
      default : usage();
      }
      break;
    }
  }

  SOCKET s;
  try {
    s = tcpOpen(10459,servername);
  } catch(Exception e) {
    _tprintf(_T("tcpOpen:%s\n"), e.what());
    exit(-1);
  }

  for(;;) {
    char stmt[4096];
    if(loop) {
      Sleep(1000);
    } else {
      pause();
    }
    sprintf(stmt,"I am client %d", GetCurrentProcessId());
    try {
      tcpWrite(s,stmt,sizeof(stmt));
    } catch(Exception e) {
      _tprintf(_T("tcp_write:%s\n"), e.what());
      break;
    }
  }
  return 0;
}
