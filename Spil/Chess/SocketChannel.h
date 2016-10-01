#pragma once

#include <Tcp.h>

#define CLOSESOCKET(s) if(s != INVALID_SOCKET) { tcpClose(s); s = INVALID_SOCKET; }

class SocketChannel {
private:
  SOCKET m_readSocket, m_writeSocket;
  void read(void        *dst, size_t size) const;
  void write(const void *src, size_t size) const;
public:
  SocketChannel() {
    clear();
  }
  SocketChannel(SOCKET readSocket, SOCKET writeSocket) : m_readSocket(readSocket), m_writeSocket(writeSocket) {
  }
  SocketChannel(int port); // create an internal socketpair
  inline bool isOpen() const {
    return m_readSocket != INVALID_SOCKET;
  }
  void close();
  void clear(); // Only reset descriptors to INVALID_SOCKET
  const SOCKET &getReadSocket() const {
    return m_readSocket;
  }

  void    write(const String   &s     ) const;
  String &read(       String   &s     ) const;
  void    write(const Player   &player) const;
  Player &read(       Player   &player) const;
  void    write(const Game     &game  ) const;
  Game   &read(       Game     &game  ) const;
  void    writeInt(   int       n     ) const;
  int     readInt()                     const;
};
