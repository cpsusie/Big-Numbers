#pragma once

// Knuth-Morris-Pratt string search algorithm
class KMPAutomate {
private:
  TCHAR *m_pattern;
  int       *m_next;
  int        m_patternLength;
  int        m_tableSize;
  bool       m_forwardSearch;
  const unsigned char *m_translateTable;

  void  allocate(int patternLength);
  void  deallocate();
  void  initPointers();
  void  copyFrom(const KMPAutomate &src);
  void  compileForward();
  void  compileBackward();
  int   searchForward( const TCHAR *text, int textLength) const;
  int   searchBackward(const TCHAR *text, int textLength) const;
public:
  KMPAutomate();
  KMPAutomate(const String    &pattern,                         bool forwardSearch = true, const unsigned char *translateTable = NULL);
  KMPAutomate(const TCHAR *pattern, int patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  KMPAutomate(const KMPAutomate &src);
  ~KMPAutomate();
  KMPAutomate &operator=(const KMPAutomate &src);
  void compilePattern(const String    &pattern,                         bool forwardSearch = true, const unsigned char *translateTable = NULL);
  void compilePattern(const TCHAR *pattern, int patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  int  search(        const String    &text   ) const;
  int  search(        const TCHAR *text   , int textLength    = -1) const;
};

