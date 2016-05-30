#include "pch.h"

int rename(const String &oldName, const String &newName) {
  return _trename(oldName.cstr(), newName.cstr());
}

void RENAME(const String &oldName, const String &newName) {
  if(rename(oldName,newName) < 0) {
    throwException(_T("%s,%s:%s"), oldName.cstr(), newName.cstr(), getErrnoText().cstr());
  }
}
