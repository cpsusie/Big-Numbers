#include "pch.h"
#include <Scanner.h>

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

String SourcePosition::toString() const {
  if(m_fileName.length()) {
    return format(_T("%s:(%d,%d)"), m_fileName.cstr(), m_lineNumber, m_column);
  } else {
    return format(_T("(%d,%d)"), m_lineNumber, m_column);
  }
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
