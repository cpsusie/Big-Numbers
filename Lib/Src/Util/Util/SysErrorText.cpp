#include "pch.h"

String getSysErrorText(int n) {
  String msg = strerror(n);
  if(msg.length() > 0 && msg[msg.length()-1] == '\n') {
    msg.remove(msg.length()-1);
  }
  return msg;
}

String getErrnoText() {
  return getSysErrorText(errno);
}

void throwErrNoOnNameException(const String &name) {
  throwException(_T("%s:%s"), name.cstr(), getErrnoText().cstr());
}

void throwErrNoOnSysCallException(const TCHAR *function) {
  throwException(_T("%s failed:%s"), function, getErrnoText().cstr());
}
