#include "stdafx.h"

void StringCollector::begin(TCHAR *str, int length, const SourcePosition &startPos) {
  const TCHAR save  = str[length];
  str[length]       = '\0';
  ((String&)*this)  = str;
  str[length]       = save;
  m_startPos        = startPos;
}

void StringCollector::addChar(TCHAR ch) {
  ((String&)*this) += ch;
}

SourceText StringCollector::getSourceText(int lastSymbolLength) {
  SourceText result;
  result.m_sourceText = substr(*this, 0, length()-lastSymbolLength);
  result.m_pos        = m_startPos;
  return result;
}
