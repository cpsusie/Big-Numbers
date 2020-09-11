#include "pch.h"
#include <Pipe.h>

Pipe::Pipe(int bufferSize, int textMode) {
  init();
  open(bufferSize, textMode);
}

void Pipe::open(int bufferSize, int textMode) {
  close();
  PIPE(m_fd,bufferSize, textMode);
}

void Pipe::dupAndClose(int index) {
  assert(isOpen(index));
  DUP2(m_fd[index], index);
  close(index);
}

void Pipe::close() {
  close(READ_FD);
  close(WRITE_FD);
}

void Pipe::close(int index) {
  int &fd = m_fd[index];
  if(fd != -1) {
    ::_close(fd);
    fd = -1;
  }
}

void Pipe::saveStdFiles() {
  close();
  m_fd[0] = DUP(0);
  m_fd[1] = DUP(1);
}

void Pipe::restoreStdFilesAndClose() {
  for(int i = 0; i < 2; i++) {
    int fd = m_fd[i];
    if(fd != -1) {
      DUP2(fd, i);
      close(i);
    }
  }
}

void Pipe::init() {
  m_fd[0] = m_fd[1] = -1;
}
