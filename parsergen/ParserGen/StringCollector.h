#pragma once

#include <Scanner.h>

class SourceText {
public:
  SourcePosition m_pos;
  String         m_sourceText;
  SourceText() : m_pos(-1, 0) { // -1 indicates that text is empty
  }
  bool isDefined() const {
    return m_pos.getLineNumber() != -1;
  }
};

class StringCollector : public String {
private:
  SourcePosition m_startPos;
public:
  void begin(TCHAR *str, int length, const SourcePosition &startPos);
  void init() {
    ((String&)*this) = _T("");
  }
  void addChar(TCHAR ch);
  TCHAR *getBuffer() {
    return cstr();
  }
  SourceText getSourceText(int lastSymbolLength);
  SourcePosition getSourcePos() const {
    return m_startPos;
  }
};
