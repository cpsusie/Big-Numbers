#include "stdafx.h"

string getSysErrorText(int n) {
  string msg = strerror(n);
  uint len = msg.length();
  if((len-- > 0) && msg[len] == '\n') {
    msg.erase(len);
  }
  return msg;
}

string getErrnoText() {
  return getSysErrorText(errno);
}

void throwErrNoOnNameException(const string &name) {
  throwException("%s:%s", name.c_str(), getErrnoText().c_str());
}

void throwErrNoOnSysCallException(const char *function) {
  throwException("%s failed:%s", function, getErrnoText().c_str());
}
