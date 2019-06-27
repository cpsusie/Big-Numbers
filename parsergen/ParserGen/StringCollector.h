#pragma once

#include <Scanner.h>

class SourceText {
public:
  SourcePositionWithName m_pos;
  String                 m_sourceText;
  // -1 indicates that text is empty
  SourceText() : m_pos(EMPTYSTRING, -1, 0) {
  }
  bool isDefined() const {
    return m_pos.getLineNumber() != -1;
  }
};

class StringCollector : public String {
private:
  SourcePositionWithName m_startPos;
public:
  void begin(TCHAR *str, int length, const SourcePositionWithName &startPos);
  inline void init() {
    ((String&)*this) = EMPTYSTRING;
  }
  void addChar(TCHAR ch);
  inline const TCHAR *getBuffer() const {
    return cstr();
  }
  SourceText getSourceText(int lastSymbolLength);
  inline const SourcePositionWithName &getSourcePos() const {
    return m_startPos;
  }
  static String trimIndent(const SourcePosition &startPos, const String &s);
};
