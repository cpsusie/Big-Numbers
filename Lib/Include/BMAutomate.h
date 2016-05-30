#pragma once

// Boyer-Moore string search algorithm
class BMAutomate {
private:

typedef int (BMAutomate::*SearchFunction)(const TCHAR *text, int textLength) const;

  TCHAR     *m_pattern;
  int        m_delta1[256];
  int       *m_delta2;
  int        m_patternLength;
  int        m_plm1; // = m_patternLength-1
  bool       m_forwardSearch;
  const unsigned char *m_translateTable;
  SearchFunction m_search;
  void  allocate(int patternLength);
  void  deallocate();
  void  initPointers();
  void  makeDelta1();
  void  makeDelta2();
  bool  isPrefix(       const TCHAR *word, int wordLength, int pos) const;
  int   getSuffixLength(const TCHAR *word, int wordLength, int pos) const;
  void  copyFrom(const BMAutomate &src);
  void  compileForward();
  void  compileBackward();
  int   searchForwardTranslate(   const TCHAR *text, int textLength) const;
  int   searchBackwardTranslate(  const TCHAR *text, int textLength) const;
  int   searchForwardNoTranslate( const TCHAR *text, int textLength) const;
  int   searchBackwardNoTranslate(const TCHAR *text, int textLength) const;
public:
  BMAutomate();
  BMAutomate(const String &pattern                                 , bool forwardSearch = true, const unsigned char *translateTable = NULL);
  BMAutomate(const TCHAR  *pattern,          int patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  BMAutomate(const BMAutomate &src);
  ~BMAutomate();
  BMAutomate &operator=(const BMAutomate &rsc);
  void compilePattern(const String &pattern,                         bool forwardSearch = true, const unsigned char *translateTable = NULL);
  void compilePattern(const TCHAR  *pattern, int patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  int  search(        const String &text   ) const;
  int  search(        const TCHAR  *text   , int textLength    = -1) const;
};
