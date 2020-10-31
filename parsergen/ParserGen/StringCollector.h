#pragma once

#include "SourceText.h"

class StringCollector : public String {
private:
  SourcePositionWithName m_startPos;
public:
  void                                 begin(TCHAR *str, int length, const SourcePositionWithName &startPos);
  inline void                          init()                                     { ((String&)*this) = EMPTYSTRING;  }
  inline void                          addChar(TCHAR ch)                          { ((String&)*this) += ch;          }
  inline const TCHAR                  *getBuffer()                          const {  return cstr();                  }
  inline const SourcePositionWithName &getSourcePos()                       const { return m_startPos;               }
  SourceText                           getSourceText(int lastSymbolLength);
  static String                        trimIndent(const SourcePosition &startPos, const String &s);
};
