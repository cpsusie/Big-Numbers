#pragma once

#include <Tcp.h>

#define CLOSESOCKET(s) if(s != INVALID_SOCKET) { tcpClose(s); s = INVALID_SOCKET; }

class SocketPair;

class SocketChannel {
private:
  SocketPair *m_socketPair;
  void read(void        *dst, size_t size) const;
  void write(const void *src, size_t size) const;
  void addref();  // if(m_socketPair != NULL) then call m_socketPair->addref()
  void release(); // if(m_socketPair != NULL) then call m_socketPair->release()
                  // and return-value = 0 then delete m_socketPair
public:
  SocketChannel() : m_socketPair(NULL) {
  }
  SocketChannel(int portnr, const String &serverName); // call tcpOpen   twice
  SocketChannel(SOCKET listener);                      // call tcpAccept twice
  SocketChannel(const SocketChannel &src);
  virtual ~SocketChannel() {
    clear();
  }
  SocketChannel &operator=(const SocketChannel &src);

  inline bool isOpen() const {
    return m_socketPair != NULL;
  }
  void clear(); // call release() and and set m_socketPair = NULL

  void    write(const String   &s     ) const;
  String &read(       String   &s     ) const;
  void    write(const Player   &player) const;
  Player &read(       Player   &player) const;
  void    write(const Game     &game  ) const;
  Game   &read(       Game     &game  ) const;
  void    writeInt(   int       n     ) const;
  int     readInt()                     const;
};
