#include "stdafx.h"
#include <Date.h>

int main(int argc, char **argv) {
  try {
    SOCKET listen = tcpCreate(3742);
    _tprintf(_T("lytter på port 3742...\n"));
    for(;;) {
      SOCKET s = tcpAccept(listen);
      _tprintf(_T("%s. got a connection\n"), Timestamp::cctime().cstr());
      tcpClose(s);
    }
  } catch(Exception e) {
    _tprintf(_T("%s\n"), e.what());
    return -1;
  }
  return 0;
}
