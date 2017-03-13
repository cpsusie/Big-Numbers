#include "pch.h"
#include <SocketStream.h>
#include <comdef.h>
#include <atlconv.h>

void SocketStream::open(int portNr, const TCHAR *hostName) {
  USES_CONVERSION;

  m_socket  = tcpOpen(portNr, T2A(hostName));
  m_doClose = true;
}

void SocketStream::close() {
  if(m_socket != INVALID_SOCKET) {
    tcpClose(m_socket);
  }
  m_socket = INVALID_SOCKET;
}

void SocketStream::putByte(BYTE c) {
  putBytes(&c, 1);
}

int SocketStream::getByte() {
  BYTE b;
  if(getBytes(&b,1) != 1) {
    return EOF;
  }
  return b;
}

BufferedSocket::BufferedSocket(SOCKET socket) : m_stream(socket) {
}

BufferedSocket::BufferedSocket(int portNr, const TCHAR *hostName) {
  m_stream.open(portNr, hostName);
}

