#pragma once

// Knuth-Morris-Pratt string search algorithm
class KMPAutomate {
private:
  TCHAR    *m_pattern;
  int      *m_next;
  size_t    m_patternLength;
  size_t    m_tableSize;
  bool      m_forwardSearch;
  const unsigned char *m_translateTable;

  void     allocate(size_t patternLength);
  void     deallocate();
  void     initPointers();
  void     copyFrom(const KMPAutomate &src);
  void     compileForward();
  void     compileBackward();
  intptr_t searchForward( const TCHAR *text, size_t textLength) const;
  intptr_t searchBackward(const TCHAR *text, size_t textLength) const;
public:
  KMPAutomate();
  KMPAutomate(const String &pattern,                              bool forwardSearch = true, const unsigned char *translateTable = NULL);
  KMPAutomate(const TCHAR  *pattern, intptr_t patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  KMPAutomate(const KMPAutomate &src);
  ~KMPAutomate();
  KMPAutomate &operator=(const KMPAutomate &src);
  void compilePattern(const String &pattern,                              bool forwardSearch = true, const unsigned char *translateTable = NULL);
  void compilePattern(const TCHAR  *pattern, intptr_t patternLength = -1, bool forwardSearch = true, const unsigned char *translateTable = NULL);
  intptr_t search(const String &text   ) const;
  intptr_t search(const TCHAR  *text   , intptr_t textLength    = -1) const;
};

