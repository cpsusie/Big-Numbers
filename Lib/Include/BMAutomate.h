#pragma once

// Boyer-Moore string search algorithm
class BMAutomate {
private:

typedef intptr_t (BMAutomate::*SearchFunction)(const TCHAR *text, size_t textLength) const;

  TCHAR     *m_pattern;
  size_t     m_delta1[256];
  intptr_t  *m_delta2;
  size_t     m_patternLength;
  size_t     m_plm1; // = m_patternLength-1
  bool       m_forwardSearch;
  const unsigned char *m_translateTable;
  SearchFunction m_search;
  void  allocate(size_t patternLength);
  void  deallocate();
  void  initPointers();
  void  makeDelta1();
  void  makeDelta2();
  bool  isPrefix(       const TCHAR *word, intptr_t wordLength, intptr_t pos) const;
  size_t getSuffixLength(const TCHAR *word, intptr_t wordLength, intptr_t pos) const;
  void  copyFrom(const BMAutomate &src);
  void  compileForward();
  void  compileBackward();
  intptr_t searchForwardTranslate(   const TCHAR *text, size_t textLength) const;
  intptr_t searchBackwardTranslate(  const TCHAR *text, size_t textLength) const;
  intptr_t searchForwardNoTranslate( const TCHAR *text, size_t textLength) const;
  intptr_t searchBackwardNoTranslate(const TCHAR *text, size_t textLength) const;
public:
  BMAutomate();
  BMAutomate(const String &pattern                                    , bool forwardSearch = true, const unsigned char *translateTable = NULL);
  BMAutomate(const TCHAR  *pattern,          size_t patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  BMAutomate(const BMAutomate &src);
  ~BMAutomate();
  BMAutomate &operator=(const BMAutomate &rsc);
  void compilePattern(const String &pattern,                            bool forwardSearch = true, const unsigned char *translateTable = NULL);
  void compilePattern(const TCHAR  *pattern, size_t patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  intptr_t search(    const String &text   ) const;
  intptr_t search(    const TCHAR  *text   , size_t textLength    = -1) const;
};
