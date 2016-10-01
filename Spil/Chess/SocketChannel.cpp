#include "stdafx.h"
#include "SocketChannel.h"

class SocketConnecter : public Thread {
private:
  const int m_port;
  SOCKET   &m_socket;
public:
  SocketConnecter(int port, SOCKET &socket) : m_port(port), m_socket(socket) {
    start();
  }
  unsigned int run();
};

unsigned int SocketConnecter::run() {
  try {
    m_socket = tcpOpen(m_port);
  } catch(...) {
    CLOSESOCKET(m_socket);
  }
  return 0;
}

SocketChannel::SocketChannel(int port) {
  m_readSocket = m_writeSocket = INVALID_SOCKET;
  SOCKET listener = INVALID_SOCKET;
  try {
    listener = tcpCreate(port);
    SocketConnecter scThread(port, m_readSocket);
    m_writeSocket = tcpAccept(listener);
    CLOSESOCKET(listener);
  } catch(...) {
    CLOSESOCKET(listener);
    close();
    throw;
  }
}

void SocketChannel::close() {
  CLOSESOCKET(m_readSocket );
  CLOSESOCKET(m_writeSocket);
}

void SocketChannel::clear() {
  m_readSocket = m_writeSocket = INVALID_SOCKET;
}

void SocketChannel::read(void *dst, size_t size) const {
  tcpRead(m_readSocket, dst, size);
}

void SocketChannel::write(const void *src, size_t size) const {
  tcpWrite(m_writeSocket, src, size);
}

void SocketChannel::write(const String &s) const {
  tcpWriteString(m_writeSocket, s);
}

String &SocketChannel::read(String &s) const {
  return s = tcpReadString(m_readSocket);
}

void SocketChannel::write(const Player &player) const {
  write(&player, sizeof(Player));
}

Player &SocketChannel::read(Player &player) const {
  read(&player, sizeof(Player));
  return player;
}

void SocketChannel::write(const Game &game) const {
  write(game.toString());
}

Game &SocketChannel::read(Game &game) const {
  String s;
  return game = read(s);
}

void SocketChannel::writeInt(int n) const {
  n = htonl(n);
  write(&n, sizeof(n));
}

int SocketChannel::readInt() const {
  int n;
  read(&n, sizeof(n));
  return ntohl(n);
}
