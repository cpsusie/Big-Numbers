#include "stdafx.h"
#include <StringArray.h>

void StringCollector::begin(TCHAR *str, int length, const SourcePositionWithName &startPos) {
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

static size_t findFirstNonSpace(const String &s) {
  const TCHAR *cp = s.cstr();
  for (; iswspace(*cp); cp++);
  return cp - s.cstr();
}

static size_t getMinIndent(const StringArray &lines) {
  if (lines.isEmpty()) {
    return 0;
  }
  size_t result = findFirstNonSpace(lines[0]);
  for (size_t i = 1; i < lines.size(); i++) {
    size_t l = findFirstNonSpace(lines[i]);
    if (l < result) {
      result = l;
    }
  }
  return result;
}

String StringCollector::trimIndent(const SourcePosition &startPos, const String &s) { // static
  String text = spaceString(startPos.getColumn()) + s;
  text.trimRight();
  StringArray lines(Tokenizer(text, _T("\n\r"), 0, TOK_SINGLEDELIMITERS | TOK_CSTRING));
  const size_t minIndent = getMinIndent(lines);
  if(minIndent == 0) {
    return s;
  }
  String result;
  for(size_t i = 0; i < lines.size(); i++) {
    String &line = lines[i];
    line = substr(line, minIndent, line.length()).trimRight();
    result += format(_T("%s\n"), line.cstr());
  }
  return result;
}
