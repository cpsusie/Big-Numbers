#pragma once

#include "StringSearchAlgorithm.h"

// Knuth-Morris-Pratt string search algorithm
template<class Ctype> class KMPAutomateTemplate : public StringSearchAlgorithm<Ctype> {
private:
  typedef intptr_t (KMPAutomateTemplate<Ctype>::*SearchFunction)(const Ctype *buf, size_t length) const;

  Ctype         *m_pattern;
  int           *m_next;
  size_t         m_patternLength;
  size_t         m_tableSize; // == m_patternLength + 1
  bool           m_forwardSearch;
  SearchFunction m_search;

public:
  KMPAutomateTemplate() {
    initPointers();
  }

  KMPAutomateTemplate(const Ctype *pattern, size_t patternLength, bool forwardSearch = true, const Ctype *translateTable = NULL) {
    initPointers();
    compilePattern(pattern, patternLength, forwardSearch, translateTable);
  }

  KMPAutomateTemplate(const KMPAutomateTemplate<Ctype> &src) {
    allocate(src.m_patternLength);
    copyFrom(src);
  }

  StringSearchAlgorithm<Ctype> *clone() const {
    return new KMPAutomateTemplate<Ctype>(*this);
  }

  ~KMPAutomateTemplate() {
    deallocate();
  }

  KMPAutomateTemplate<Ctype> &operator=(const KMPAutomateTemplate<Ctype> &src) {
    if(this == &src) {
      return *this;
    }
    if(m_patternLength != src.m_patternLength) {
      deallocate();
      allocate(src.m_patternLength);
    }
    copyFrom(src);
    return *this;
  }

  void compilePattern(const Ctype *pattern, size_t patternLength, bool forwardSearch = true, const Ctype *translateTable = NULL) {
    if(patternLength != m_patternLength) {
      deallocate();
      allocate(patternLength);
    }
    memcpy(m_pattern, pattern, sizeof(m_pattern[0])*m_tableSize); // '\0' may occur in pattern
    m_forwardSearch  = forwardSearch;
    m_translateTable = translateTable;

    if(m_forwardSearch) {
      compileForward(); 
      m_search = m_translateTable 
               ? &KMPAutomateTemplate<Ctype>::searchForwardTranslate
               : &KMPAutomateTemplate<Ctype>::searchForwardNoTranslate;
    } else {
      compileBackward();
      m_search = m_translateTable 
               ? &KMPAutomateTemplate<Ctype>::searchBackwardTranslate
               : &KMPAutomateTemplate<Ctype>::searchBackwardNoTranslate;
    }
  }

  intptr_t search(const Ctype *text, size_t textLength) const {
    return (this->*m_search)(text, textLength);
  }

private:

  void allocate(size_t patternLength) {
    if (patternLength > INT_MAX) {
      throwInvalidArgumentException(__TFUNCTION__, _T("patternLength > %u not allowed"), INT_MAX);
    }
    m_tableSize = (m_patternLength = patternLength) + 1;
    m_next      = new int[  m_tableSize];
    m_pattern   = new Ctype[m_tableSize];
  }

  void deallocate() {
    if(m_pattern) {
      delete[] m_pattern;
    }
    if(m_next) {
      delete[] m_next;
    }
    initPointers();
  }

  void initPointers() {
    m_pattern       = NULL;
    m_next          = NULL;
    m_patternLength = -1;
    m_tableSize     =  0;
    m_search        = &KMPAutomateTemplate<Ctype>::notCompiledsearchFunction;
  }

  void copyFrom(const KMPAutomateTemplate &src) {
    if(m_tableSize > 0) {
      memcpy(m_next   , src.m_next   , sizeof(m_next[0]    )*m_tableSize);
      memcpy(m_pattern, src.m_pattern, sizeof(m_pattern[0] )*m_tableSize);
    }
    m_forwardSearch  = src.m_forwardSearch;
    m_translateTable = src.m_translateTable;
    m_search         = src.m_search;
  }

  void compileForward() {
    m_next[0] = -1;
    int j = -1;
    size_t i = 0;
    do { 
      if(j == -1 || charsEqualC(m_pattern[i], m_pattern[j])) {
        i++;
        j++;
        m_next[i] = charsEqualC(m_pattern[j], m_pattern[i]) ? m_next[j] : j;
      } else {
        j = m_next[j]; 
      }
    }
    while(i < m_patternLength);
  }

  void compileBackward() {
    reversebuf(m_pattern, m_patternLength);
    compileForward();
  }

  intptr_t notCompiledsearchFunction(const Ctype *buf, size_t length) const {
    throwException(_T("Cannot search before a pattern is compiled"));
    return -1;
  }

  // text points to start of String, textLength is the length of the String.
  // searchForward finds the first occurence of the pattern in text
  intptr_t searchForwardNoTranslate(const Ctype *text, size_t textLength) const {
    for(intptr_t i = 0, j = 0; i < (intptr_t)textLength; ) {
      if((j == -1) || (m_pattern[j] == text[i])) { 
        i++; j++;
        if(j == m_patternLength) {
          return i - j;
        }
      } else {
        j = m_next[j];
      }
    }
    return -1;
  }

  // searchBackward finds the last occurence of the pattern in text
  intptr_t searchBackwardNoTranslate(const Ctype *text, size_t textLength) const {
    for(intptr_t i = textLength - 1, j = 0; i >= 0;) {
      if((j == -1) || (m_pattern[j] == text[i])) { 
        i--; j++;
        if(j == m_patternLength) {
          return i + 1;
        }
      } else {
        j = m_next[j];
      }
    }
    return -1;
  }

  intptr_t searchForwardTranslate(const Ctype *text, size_t textLength) const {
    for(intptr_t i = 0, j = 0; i < (intptr_t)textLength; ) {
      if((j == -1) || charsEqual(m_pattern[j],text[i])) { 
        i++; j++;
        if(j == m_patternLength) {
          return i - j;
        }
      } else {
        j = m_next[j];
      }
    }
    return -1;
  }

  // searchBackward finds the last occurence of the pattern in text
  intptr_t searchBackwardTranslate(const Ctype *text, size_t textLength) const {
    for(intptr_t i = textLength - 1, j = 0; i >= 0;) {
      if((j == -1) || charsEqual(m_pattern[j],text[i])) { 
        i--; j++;
        if(j == m_patternLength) {
          return i + 1;
        }
      } else {
        j = m_next[j];
      }
    }
    return -1;
  }
};

class KMPAutomateBYTE : public KMPAutomateTemplate<BYTE> {
public:
  KMPAutomateBYTE() {
  }
  KMPAutomateBYTE(const BYTE *pattern, size_t patternLength, bool forwardSearch, const BYTE *translateTable = NULL)
   : KMPAutomateTemplate<BYTE>(pattern, patternLength, forwardSearch, translateTable)
  {
  }
};

class KMPAutomatewchar_t : public KMPAutomateTemplate<wchar_t> {
public:
  KMPAutomatewchar_t() {
  }
  KMPAutomatewchar_t(const wchar_t *pattern, size_t patternLength, bool forwardSearch, const wchar_t *translateTable = NULL)
   : KMPAutomateTemplate<wchar_t>(pattern, patternLength, forwardSearch, translateTable)
  {
  }
};

class KMPAutomate : public KMPAutomateTemplate<TCHAR> {
public:
  KMPAutomate() {
  }
  KMPAutomate(const TCHAR *pattern, size_t patternLength, bool forwardSearch, const TCHAR *translateTable = NULL)
   : KMPAutomateTemplate<TCHAR>(pattern, patternLength, forwardSearch, translateTable)
  {
  }
  KMPAutomate(const String &pattern, bool forwardSearch, const TCHAR *translateTable = NULL)
   : KMPAutomateTemplate<TCHAR>(pattern.cstr(), pattern.length(), forwardSearch, translateTable)
  {
  }
  void compilePattern(const String &pattern, bool forwardSearch, const TCHAR *translateTable) {
    __super::compilePattern(pattern.cstr(), pattern.length(), forwardSearch, translateTable);
  }
  intptr_t search(const String &text) const {
    return __super::search(text.cstr(), text.length());
  }
};
