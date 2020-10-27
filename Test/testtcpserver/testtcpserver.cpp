#include "stdafx.h"
#include <Array.h>
#include <Tcp.h>

static int findclientindex(Array<SOCKET> &clientsockets,fd_set &fds) {
  for(UINT i = 0; i < clientsockets.size(); i++)
    if(FD_ISSET(clientsockets[i],&fds))
      return i;
  return -1;
}

int main(int argc, char **argv) {
  SOCKET listensocket;
  Array<SOCKET> clientsockets;

  try {
    listensocket = tcpCreate(10459);
  } catch(Exception e) {
    _tprintf(_T("tcpCreate:%s\n"), e.what());
    exit(-1);
  }

  for(;;) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(listensocket, &readfds );
    for(size_t i = 0; i < clientsockets.size(); i++) {
      FD_SET(clientsockets[i], &readfds );
    }
    if( select( 2, &readfds, nullptr, nullptr, nullptr ) == SOCKET_ERROR ) {
      _tprintf(_T("select failed. error:%s\n"),getLastWSAErrorText().cstr());
      break;
    }
    if( FD_ISSET( listensocket, &readfds ) ) { // handle new clients
      try {
        SOCKET s = tcpAccept(listensocket);
    	  _tprintf(_T("got new socket %zd\n"), s);
        clientsockets.add(s);
      } catch(...) {
      };
      continue;
    }
    int socketindex = findclientindex(clientsockets,readfds);
    if(socketindex >= 0) {
      try {
    	  SOCKET &cs = clientsockets[socketindex];
    	  char stmt[4096];
    	  tcpRead(cs,stmt,sizeof(stmt));
        printf("read from %zd <%s>\n",cs,stmt);
      } catch(Exception) {
    	  printf("closed socket %zd\n",clientsockets[socketindex]);
    	  tcpClose(clientsockets[socketindex]);
    	  clientsockets.remove(socketindex);
      }
    }
  }
  return 0;
}
