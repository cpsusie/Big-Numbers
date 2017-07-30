#include "pch.h"
#include <SourcePosition.h>

SourcePosition::SourcePosition(const String &s, UINT index) {
  setLocation(0,0);
  for(const TCHAR *t = s.cstr(); index-- && *t;) {
    if(*(t++) == _T('\n')) {
      incrLineNumber();
    } else {
      incrColumn();
    }
  }
}

void SourcePosition::setLocation(int lineNumber, int column) {
  m_lineNumber = lineNumber;
  m_column     = column;
}

int SourcePosition::findCharIndex(const String &s) const {
  SourcePosition tmp;
  int index = 0;
  for(const TCHAR *t = s.cstr(); *t && (tmp < *this); index++) {
    if(*(t++) == _T('\n')) {
      tmp.incrLineNumber();
    } else {
      tmp.incrColumn();
    }
  }
  return index;
}

String SourcePositionWithName::toString() const {
  return m_name.length()
       ? format(_T("%s%s"), m_name.cstr(), __super::toString().cstr())
       : __super::toString();
}
