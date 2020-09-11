#pragma once

#include "MyUtil.h"
#include <io.h>
#include <fcntl.h>

#define READ_FD  0
#define WRITE_FD 1

class Pipe {
private:
  int m_fd[2];
  void init();
  void close(int index);
public:
  Pipe() { init(); }
  Pipe(int bufferSize, int textMode);
  // NB...No destructor. Call close when file descriptors are no longer in use.
  void open(int bufferSize = 256, int textMode = _O_TEXT | _O_NOINHERIT);
  void close();
  void dupAndClose(int index);
  void saveStdFiles();
  void restoreStdFilesAndClose();
  inline bool isOpen(int index) const {
    return m_fd[index] != -1;
  }
  inline int getfd(int index) const {
    return m_fd[index];
  }
  inline FILE *getFile(int index, const TCHAR *mode) {
    assert(isOpen(index));
    return _tfdopen(m_fd[index], mode);
  }
};

