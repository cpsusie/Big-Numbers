#include "pch.h"
#include <Scanner.h>

SourcePosition::SourcePosition(const String &fileName, int lineNumber, int column) {
  m_fileName   = fileName;
  m_lineNumber = lineNumber;
  m_column     = column;
}

SourcePosition::SourcePosition(int lineNumber, int column) {
  m_lineNumber = lineNumber;
  m_column     = column;
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

int SourcePosition::findCharIndex(const TCHAR *s, const SourcePosition &pos) { // static
  int lineCount = 0;
  int col       = 0;
  int i;
  for(i = 0; *s; i++, s++) {
    if(lineCount == pos.getLineNumber() && col == pos.getColumn() ) {
      break;
    }
    if(*s == _T('\n')) {
      lineCount++;
      col = 0;
    } else {
      col++;
    }
  }
  return i;
}

SourcePosition SourcePosition::findSourcePosition(const TCHAR *s, int index) { // static
  SourcePosition pos(0, 0);
  for(int i = 0; i < index && *s; i++) {
    if(*(s++) == _T('\n')) {
      pos.incrLineNumber();
    } else {
      pos.incrColumn();
    }
  }
  return pos;
}


