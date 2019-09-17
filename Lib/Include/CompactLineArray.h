#pragma once

#include "StringArray.h"
#include "CompactArray.h"

class CompactLineArray : public CompactStrArray {
private:
  const StringArray m_lines;
  void init() {
    const size_t n = m_lines.size();
    setCapacity(n);
    for(size_t i = 0; i < n; i++) {
      add((TCHAR*)m_lines[i].cstr());
    }
  }
public:
  inline CompactLineArray(const String &str, const String &delimiters = _T("\n\r"))
    : m_lines(Tokenizer(str, delimiters)) {
    init();
  }
  inline CompactLineArray(const std::string &str, const String &delimiters = _T("\n\r"))
    : m_lines(Tokenizer(String(str.c_str()), delimiters)) {
    init();
  }
  inline CompactLineArray(const std::wstring &str, const String &delimiters=_T("\n\r"))
    : m_lines(Tokenizer(String(str.c_str()), delimiters))
  {
    init();
  }
  const StringArray &geStringArray() const {
    return m_lines;
  }
  size_t minLength() const {
    return m_lines.minLength();
  }
  size_t maxLength() const {
    return m_lines.maxLength();
  }
};
