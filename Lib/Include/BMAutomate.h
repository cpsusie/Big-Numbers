#pragma once

#include "StringSearchAlgorithm.h"

// Boyer-Moore string search algorithm
template<class Ctype> class BMAutomateTemplate : public StringSearchAlgorithm<Ctype> {
private:
  typedef intptr_t (BMAutomateTemplate<Ctype>::*SearchFunction)(const Ctype *buf, size_t length) const;

  Ctype         *m_pattern;
  int           *m_delta1;
  int           *m_delta2;
  int            m_patternLength;
  UINT           m_delta1Size;
  int            m_plm1; // = m_patternLength-1
  bool           m_forwardSearch;
  SearchFunction m_search;

public:
  BMAutomateTemplate() {
    initPointers();
  }

  BMAutomateTemplate(const Ctype *pattern, size_t patternLength, bool forwardSearch, const Ctype *translateTable) {
    initPointers();
    compilePattern(pattern, patternLength, forwardSearch, translateTable);
  }

  BMAutomateTemplate(const BMAutomateTemplate<Ctype> &src) {
    allocate(src.m_patternLength);
    copyFrom(src);
  }

  StringSearchAlgorithm<Ctype> *clone() const {
    BMAutomateTemplate *a = new BMAutomateTemplate<Ctype>(*this); TRACE_NEW(a); return a;
  }

  ~BMAutomateTemplate() {
    deallocate();
  }

  BMAutomateTemplate<Ctype> &operator=(const BMAutomateTemplate<Ctype> &src) {
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

  void compilePattern(const Ctype *pattern, size_t patternLength, bool forwardSearch, const Ctype *translateTable) {
    if(patternLength != m_patternLength) {
      deallocate();
      allocate(patternLength);
    }
    memcpy(m_pattern, pattern, (m_patternLength+1)*sizeof(Ctype)); // '\0' may occur in pattern
    m_forwardSearch  = forwardSearch;
    m_translateTable = translateTable;

    if(m_forwardSearch) {
      compileForward();
      m_search = m_translateTable
               ? &BMAutomateTemplate<Ctype>::searchForwardTranslate
               : &BMAutomateTemplate<Ctype>::searchForwardNoTranslate;
    } else {
      compileBackward();
      m_search = m_translateTable
               ? &BMAutomateTemplate<Ctype>::searchBackwardTranslate
               : &BMAutomateTemplate<Ctype>::searchBackwardNoTranslate;
    }
  }

  intptr_t search(const Ctype *text, size_t length) const {
    return (this->*m_search)(text, length);
  }

private:
  void compileForward() {
    makeDelta1();
    makeDelta2();
  }

  void compileBackward() {
    reversebuf(m_pattern, m_patternLength);
    compileForward();
  }

  intptr_t notCompiledsearchFunction(const Ctype *buf, size_t length) const {
    throwException(_T("Cannot search before a pattern is compiled"));
    return -1;
  }

  intptr_t searchForwardNoTranslate(const Ctype *buf, size_t length) const {
    int d1,d2,j;
    for(intptr_t i = m_plm1; i < (intptr_t)length; i += max(d1, d2)) {
      for(j = m_plm1; (j >= 0) && (buf[i] == m_pattern[j]); i--, j--);
      if(j < 0) return i + 1;
      d1 = m_delta1[buf[i]];
      d2 = m_delta2[j];
    }
    return -1;
  }

  intptr_t searchBackwardNoTranslate(const Ctype *buf, size_t length) const {
    int d1,d2,j;
    for(intptr_t i = length - m_patternLength; i >= 0; i -= max(d1, d2)) {
      for(j = m_plm1; (j >= 0) && (buf[i] == m_pattern[j]); i++, j--);
      if(j < 0) return i - m_patternLength;
      d1 = m_delta1[buf[i]];
      d2 = m_delta2[j];
    }
    return -1;
  }

  intptr_t searchForwardTranslate(const Ctype *buf, size_t length) const {
    int d1,d2,j;
    for(intptr_t i = m_plm1; i < (intptr_t)length; i += max(d1, d2)) {
      for(j = m_plm1; (j >= 0) && charsEqual(buf[i], m_pattern[j]); i--, j--);
      if(j < 0) return i + 1;
      d1 = m_delta1[translate(buf[i])];
      d2 = m_delta2[j];
    }
    return -1;
  }

  intptr_t searchBackwardTranslate(const Ctype *buf, size_t length) const {
    int d1,d2,j;
    for(intptr_t i = length - m_patternLength; i >= 0; i -= max(d1, d2)) {
      for(j = m_plm1; (j >= 0) && charsEqual(buf[i], m_pattern[j]); i++, j--);
      if(j < 0) return i - m_patternLength;
      d1 = m_delta1[translate(buf[i])];
      d2 = m_delta2[j];
    }
    return -1;
  }

  void allocate(size_t patternLength) {
    if(patternLength > INT_MAX) {
      throwInvalidArgumentException(__TFUNCTION__, _T("patternLength > %u not allowed"), INT_MAX);
    }
    m_plm1    = (m_patternLength = (int)patternLength) - 1;
    m_pattern = new Ctype[patternLength+1]; TRACE_NEW(m_pattern);
    int CtypeMaxValue;
    switch (sizeof(Ctype)) {
    case 1 : CtypeMaxValue = 0xff  ; break;
    case 2 : CtypeMaxValue = 0xffff; break;
    default: throwException(_T("BMAutomateTemplate is valid for sizeof(Ctype) <= 2 only"));
    }
    m_delta1Size = CtypeMaxValue + 1;
    m_delta1     = new int[m_delta1Size ]; TRACE_NEW(m_delta1);
    m_delta2     = new int[patternLength]; TRACE_NEW(m_delta2);
  }

  void deallocate() {
    SAFEDELETEARRAY(m_pattern);
    SAFEDELETEARRAY(m_delta1);
    SAFEDELETEARRAY(m_delta2);
    initPointers();
  }

  void initPointers() {
    m_pattern    = NULL;
    m_delta1     = NULL;
    m_delta2     = NULL;
    m_delta1Size = 0;
    m_plm1       = (m_patternLength = -1) - 1;
    m_search     = &BMAutomateTemplate<Ctype>::notCompiledsearchFunction;
  }

  void copyFrom(const BMAutomateTemplate<Ctype> &src) {
    memcpy(m_pattern  , src.m_pattern, sizeof(m_pattern[0] ) * (m_patternLength+1));
    memcpy(m_delta1   , src.m_delta1 , sizeof(m_delta1[0]  ) *  m_delta1Size      );
    memcpy(m_delta2   , src.m_delta2 , sizeof(m_delta2[0]  ) *  m_patternLength   );
    m_forwardSearch  = src.m_forwardSearch;
    m_translateTable = src.m_translateTable;
    m_search         = src.m_search;
  }

  // m_delta1[c] contains the distance between the last symbol
  // of pattern and the rightmost occurence of c in pattern.
  // If c does not occur in pattern, then m_delta1[c] = patternLength.
  // If c is at textbuf[i] and c != pattern[patternLength-1], we can
  // safely shift i over by m_delta1[c], which is the minimum distance
  // needed to shift pattern forward to get textbuf[i] lined up
  // with some symbol in pattern.
  // This algorithm runs in time O(alphabet length + pattern length)
  void makeDelta1() {
    for(UINT i = 0; i < m_delta1Size; i++) {
      m_delta1[i] = m_patternLength;
    }
    for(int i = 0; i < m_patternLength-1; i++) {
      m_delta1[translateC(m_pattern[i])] = m_plm1 - i;
    }
  }

  // delta2 table: given a mismatch at pattern[pos], we want to align
  // with the next possible full match could be based on what we
  // know about pattern[pos+1] to pattern[patternLength-1].
  //
  // In case 1:
  // pattern[pos+1] to pattern[patternLength-1] does not occur elsewhere in pattern,
  // the next plausible match starts at or after the mismatch.
  // If, within the substring pattern[pos+1 .. patternLength-1], lies a prefix
  // of pattern, the next plausible match is here (if there are multiple
  // prefixes in the substring, pick the longest). Otherwise, the
  // next plausible match starts past the TCHAR aligned with
  // pattern[patternLength-1].
  //
  // In case 2:
  // pattern[pos+1] to pattern[patternLength-1] does occur elsewhere in pattern. The
  // mismatch tells us that we are not looking at the end of a match.
  // We may, however, be looking at the middle of a match.
  //
  // The first loop, which takes care of case 1, is analogous to
  // the KMP table, adapted for a 'backwards' scan order with the
  // additional restriction that the substrings it considers as
  // potential prefixes are all suffixes. In the worst case scenario
  // pattern consists of the same letter repeated, so every suffix is
  // a prefix. This loop alone is not sufficient, however:
  // Suppose that pattern is "ABYXCDEYX", and text is ".....ABYXCDEYX".
  // We will match X, Y, and find B != E. There is no prefix of pattern
  // in the suffix "YX", so the first loop tells us to skip forward
  // by 9 characters.
  // Although superficially similar to the KMP table, the KMP table
  // relies on information about the beginning of the partial match
  // that the BM algorithm does not have.
  //
  // The second loop addresses case 2. Since getSuffixLength may not be
  // unique, we want to take the minimum value, which will tell us
  // how far away the closest potential match is.
  void makeDelta2() {
    int lastPrefixIndex = m_plm1;

    for(int p = m_plm1; p >= 0; p--) {
      if(isPrefix(m_pattern, m_patternLength, p+1)) {
        lastPrefixIndex = p+1;
      }
      m_delta2[p] = lastPrefixIndex + (m_plm1 - p);
    }

    for(int p = 0; p < m_plm1; p++) {
      const int slen = getSuffixLength(m_pattern, m_patternLength, p);
      if(!charsEqualC(m_pattern[p - slen], m_pattern[m_plm1 - slen])) {
        m_delta2[m_plm1 - slen] = m_plm1 - p + slen;
      }
    }
  }

  // true if the suffix of word starting from word[pos] is a prefix of word
  bool isPrefix(const Ctype *word, int wordLength, int pos) const {
    const int suffixLength = wordLength - pos;
    for(int i = 0; i < suffixLength; i++) {
      if(!charsEqualC(word[i], word[pos+i])) {
        return false;
      }
    }
    return true;
  }

  // length of the longest suffix of word ending on word[pos].
  // getSuffixLength("dddbcabc", 8, 4) = 2
  int getSuffixLength(const Ctype *word, int wordLength, int pos) const {
    // increment suffix length i to the first mismatch or beginning of the word
    int i;
    for(i = 0; charsEqualC(word[pos-i], word[wordLength-1-i]) && (i < pos); i++);
    return i;
  }
};


class BMAutomateBYTE : public BMAutomateTemplate<BYTE> {
public:
  BMAutomateBYTE() {
  }
  BMAutomateBYTE(const BYTE *pattern, size_t patternLength, bool forwardSearch, const BYTE *translateTable = NULL)
   : BMAutomateTemplate<BYTE>(pattern, patternLength, forwardSearch, translateTable)
  {
  }
};

class BMAutomatewchar_t : public BMAutomateTemplate<wchar_t> {
public:
  BMAutomatewchar_t() {
  }
  BMAutomatewchar_t(const wchar_t *pattern, size_t patternLength, bool forwardSearch, const wchar_t *translateTable = NULL)
   : BMAutomateTemplate<wchar_t>(pattern, patternLength, forwardSearch, translateTable)
  {
  }
};

class BMAutomate : public BMAutomateTemplate<TCHAR> {
public:
  BMAutomate() {
  }
  BMAutomate(const TCHAR *pattern, size_t patternLength, bool forwardSearch, const TCHAR *translateTable = NULL)
   : BMAutomateTemplate<TCHAR>(pattern, patternLength, forwardSearch, translateTable)
  {
  }
  BMAutomate(const String &pattern, bool forwardSearch, const TCHAR *translateTable = NULL)
   : BMAutomateTemplate<TCHAR>(pattern.cstr(), pattern.length(), forwardSearch, translateTable)
  {
  }
  void compilePattern(const String &pattern, bool forwardSearch, const TCHAR *translateTable) {
    __super::compilePattern(pattern.cstr(), pattern.length(), forwardSearch, translateTable);
  }
  intptr_t search(const String &text) const {
    return __super::search(text.cstr(), text.length());
  }
};
