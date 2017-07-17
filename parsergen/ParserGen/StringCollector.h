#pragma once

#include <Scanner.h>

class SourceText {
public:
  SourcePosition m_pos;
  String         m_sourceText;
  // -1 indicates that text is empty
  SourceText() : m_pos(-1, 0) {
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
  inline void init() {
    ((String&)*this) = EMPTYSTRING;
  }
  void addChar(TCHAR ch);
  inline TCHAR *getBuffer() {
    return cstr();
  }
  SourceText getSourceText(int lastSymbolLength);
  inline const SourcePosition &getSourcePos() const {
    return m_startPos;
  }
};
