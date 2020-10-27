#include "stdafx.h"
#include <RefCountedObject.h>
#include <ThreadPool.h>
#include <Runnable.h>
#include "SocketChannel.h"
#include <DebugLog.h>

class SocketConnecter : public Runnable {
private:
  const int m_port;
  SOCKET   &m_socket;
public:
  SocketConnecter(int port, SOCKET &socket) : m_port(port), m_socket(socket) {
    ThreadPool::executeNoWait(*this);
  }
  UINT run();
};

UINT SocketConnecter::run() {
  try {
    m_socket = tcpOpen(m_port);
  } catch(...) {
    CLOSESOCKET(m_socket);
  }
  return 0;
}

class SocketPair : public RefCountedObject {
public:
  SOCKET m_readSocket, m_writeSocket;
  SocketPair(SOCKET rs, SOCKET ws);
  ~SocketPair();
};

SocketPair::SocketPair(SOCKET rs, SOCKET ws)
: m_readSocket(rs), m_writeSocket(ws)
{
  assert(rs != INVALID_SOCKET);
  assert(ws != INVALID_SOCKET);
}

SocketPair::~SocketPair() {
  CLOSESOCKET(m_readSocket );
  CLOSESOCKET(m_writeSocket);
#if defined(_DEBUG)
  debugLog(_T("SocketPair deleted\n"));
#endif
}

SocketChannel::SocketChannel(int portnr, const String &serverName) {
  USES_CONVERSION;
  const char *aserverName = T2A(serverName.cstr());
  SOCKET ws = tcpOpen(portnr, aserverName);
  SOCKET rs = tcpOpen(portnr, aserverName);
  m_socketPair = new SocketPair(rs,ws); TRACE_NEW(m_socketPair);
}

SocketChannel::SocketChannel(SOCKET listener) {
  SOCKET rs = tcpAccept(listener);
  SOCKET ws = tcpAccept(listener);
  m_socketPair = new SocketPair(rs,ws); TRACE_NEW(m_socketPair);
}

SocketChannel::SocketChannel(const SocketChannel &src) {
  m_socketPair = src.m_socketPair;
  addref();
}

SocketChannel &SocketChannel::operator=(const SocketChannel &src) {
  if(this != &src) {
    clear();
    m_socketPair = src.m_socketPair;
    addref();
  }
  return *this;
}

void SocketChannel::clear() {
  release();
  m_socketPair = nullptr;
}

void SocketChannel::addref() {
  if(m_socketPair) m_socketPair->addref();
}

void SocketChannel::release() {
  if (m_socketPair && (m_socketPair->release() == 0)) {
    SAFEDELETE(m_socketPair);
  }
}

void SocketChannel::read(void *dst, size_t size) const {
  assert(isOpen());
  tcpRead(m_socketPair->m_readSocket, dst, size);
}

void SocketChannel::write(const void *src, size_t size) const {
  assert(isOpen());
  tcpWrite(m_socketPair->m_writeSocket, src, size);
}

void SocketChannel::write(const String &s) const {
  assert(isOpen());
  tcpWriteString(m_socketPair->m_writeSocket, s);
}

String &SocketChannel::read(String &s) const {
  return s = tcpReadString(m_socketPair->m_readSocket);
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
