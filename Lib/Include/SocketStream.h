#pragma once

#include "MyUtil.h"
#include "tcp.h"

class SocketStream : public ByteInputOutputStream {
private:
  SOCKET m_socket;
  bool   m_doClose;
public:
  SocketStream(SOCKET s) : m_socket(s) {
    m_doClose = false;
  }
  SocketStream() {
    m_socket  = INVALID_SOCKET;
    m_doClose = false;
  }
  void open(int portNr, const TCHAR *hostName = NULL);
  void close();

  ~SocketStream() {
    if(m_doClose) {
      close();
    }
  }

  void putByte(BYTE c);
  int getByte();

  void putBytes(const BYTE *src, size_t n) {
    tcpWrite(m_socket, src, n);
  }

  intptr_t getBytes(BYTE *dst, size_t n) {
    return tcpRead(m_socket, dst, n);
  }
};

class BufferedSocket : public Packer {
private:
  SocketStream m_stream;
public:
  BufferedSocket(SOCKET socket);
  BufferedSocket(int portNr, const TCHAR *hostName = NULL);

  void send() {
    __super::write(m_stream);
    clear();
  }

  bool receive() {
    return __super::read(m_stream);
  }
};
