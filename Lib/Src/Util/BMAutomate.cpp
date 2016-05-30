#include "pch.h"
#include <BMAutomate.h>

BMAutomate::BMAutomate() {
  initPointers();
  compilePattern("");
}

BMAutomate::BMAutomate(const String &pattern, bool forwardSearch, const unsigned char *translateTable) {
  initPointers();
  compilePattern(pattern.cstr(), pattern.length(), forwardSearch, translateTable);
}

BMAutomate::BMAutomate(const TCHAR *pattern, int patternLength, bool forwardSearch, const unsigned char *translateTable) {
  initPointers();
  compilePattern(pattern, patternLength, forwardSearch, translateTable);
}

BMAutomate::BMAutomate(const BMAutomate &src) {
  allocate(src.m_patternLength);
  copyFrom(src);
}

BMAutomate::~BMAutomate() {
  deallocate();
}

BMAutomate &BMAutomate::operator=(const BMAutomate &src) {
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

void BMAutomate::compilePattern(const String &pattern, bool forwardSearch, const unsigned char *translateTable) {
  compilePattern(pattern.cstr(), pattern.length(), forwardSearch, translateTable);
}

void BMAutomate::compilePattern(const TCHAR *pattern, int patternLength, bool forwardSearch, const unsigned char *translateTable) {
  if(patternLength < 0) {
    patternLength = _tcsclen(pattern);
  }
  if(patternLength != m_patternLength) {
    deallocate();
    allocate(patternLength);
  }
  MEMCPY(m_pattern, pattern, m_patternLength+1); // NOT strncpy or strcpy here. '\0' may occur in pattern
  m_forwardSearch  = forwardSearch;
  m_translateTable = translateTable;

  if(m_forwardSearch) {
    compileForward();
    m_search = m_translateTable ? &BMAutomate::searchForwardTranslate  : &BMAutomate::searchForwardNoTranslate;
  } else {
    compileBackward();
    m_search = m_translateTable ? &BMAutomate::searchBackwardTranslate : &BMAutomate::searchBackwardNoTranslate;
  }
}

void BMAutomate::compileForward() {
  makeDelta1();
  makeDelta2();
}

static TCHAR *reversebuf(TCHAR *s, int size) {
  TCHAR tmp,*l = s, *r = s + size - 1;
  while(l < r) {
    tmp = *l; *(l++) = *r; *(r--) = tmp;
  }
  return s;
}

void BMAutomate::compileBackward() {
  reversebuf(m_pattern, m_patternLength);
  compileForward();
}

int BMAutomate::search(const String &text) const {
  return (this->*m_search)(text.cstr(), text.length());
}

int BMAutomate::search(const TCHAR *text, int textLength) const {
  if(textLength < 0) {
    textLength = _tcsclen(text);
  }
  return (this->*m_search)(text, textLength);
}

int BMAutomate::searchForwardNoTranslate(const TCHAR *text, int textLength) const {
  int d1,d2;
  for(int i = m_plm1; i < textLength; i += max(d1, d2)) {
    int j;
    for(j = m_plm1; (j >= 0) && (text[i] == m_pattern[j]); i--, j--);
    if(j < 0) {
      return i + 1;
    }
    d1 = m_delta1[(unsigned char)text[i]];
    d2 = m_delta2[j];
  }
  return -1;
}

int BMAutomate::searchBackwardNoTranslate(const TCHAR *text, int textLength) const {
  int d1,d2;
  for(int i = textLength - m_patternLength; i >= 0; i -= max(d1, d2)) {
    int j;
    for(j = m_plm1; (j >= 0) && (text[i] == m_pattern[j]); i++, j--);
    if(j < 0) {
      return i - m_patternLength;
    }
    d1 = m_delta1[(unsigned char)text[i]];
    d2 = m_delta2[j];
  }
  return -1;
}

#define TRANSLATE(  c)      m_translateTable[(unsigned char)(c)] 
#define TRANSLATE1( c)     (m_translateTable ? TRANSLATE(c) : (unsigned char)(c))
#define CHAREQUALS( c1,c2) (TRANSLATE(c1) == TRANSLATE(c2))
#define CHAREQUALS1(c1,c2) (m_translateTable ? CHAREQUALS(c1, c2) : (c1 == c2))

int BMAutomate::searchForwardTranslate(const TCHAR *text, int textLength) const {
  int d1,d2;
  for(int i = m_plm1; i < textLength; i += max(d1, d2)) {
    int j;
    for(j = m_plm1; (j >= 0) && CHAREQUALS(text[i], m_pattern[j]); i--, j--);
    if(j < 0) {
      return i + 1;
    }
    d1 = m_delta1[TRANSLATE(text[i])];
    d2 = m_delta2[j];
  }
  return -1;
}

int BMAutomate::searchBackwardTranslate(const TCHAR *text, int textLength) const {
  int d1,d2;
  for(int i = textLength - m_patternLength; i >= 0; i -= max(d1, d2)) {
    int j;
    for(j = m_plm1; (j >= 0) && CHAREQUALS(text[i], m_pattern[j]); i++, j--);
    if(j < 0) {
      return i - m_patternLength;
    }
    d1 = m_delta1[TRANSLATE(text[i])];
    d2 = m_delta2[j];
  }
  return -1;
}

void BMAutomate::allocate(int patternLength) {
  m_plm1    = (m_patternLength = patternLength) - 1;
  m_pattern = new TCHAR[patternLength+1];
  m_delta2  = new int[ patternLength];
}

void BMAutomate::deallocate() {
  if(m_pattern) {
    delete[] m_pattern;
  }
  if(m_delta2) {
    delete[] m_delta2;
  }
  initPointers();
}

void BMAutomate::initPointers() {
  m_pattern = NULL;
  m_delta2  = NULL;
  m_plm1    = (m_patternLength = -1) - 1;
}

void BMAutomate::copyFrom(const BMAutomate &src) {
  memcpy(m_pattern  , src.m_pattern, sizeof(m_pattern[0]    )*(m_patternLength+1));
  memcpy(m_delta1   , src.m_delta1 , sizeof(m_delta1));
  memcpy(m_delta2   , src.m_delta2 , sizeof(m_delta2[0]     )*m_patternLength);
  m_forwardSearch  = src.m_forwardSearch;
  m_translateTable = src.m_translateTable;
  m_search         = src.m_search;
}

// delta1 table: delta1[c] contains the distance between the last
// TCHAR of pattern and the rightmost occurence of c in pattern.
// If c does not occur in pattern, then delta1[c] = patternLength.
// If c is at string[i] and c != pattern[patternLength-1], we can
// safely shift i over by delta1[c], which is the minimum distance
// needed to shift pattern forward to get string[i] lined up 
// with some TCHAR in pattern.
// this algorithm runs in alphabet_len+patternLength time.
void BMAutomate::makeDelta1() {
  for(int i = 0; i < 256; i++) {
    m_delta1[i] = m_patternLength;
  }
  for(int i = 0; i < m_patternLength-1; i++) {
    m_delta1[TRANSLATE1(m_pattern[i])] = m_plm1 - i;
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
void BMAutomate::makeDelta2() {
  int lastPrefixIndex = m_plm1;

  for(int p = m_plm1; p >= 0; p--) {
    if(isPrefix(m_pattern, m_patternLength, p+1)) {
      lastPrefixIndex = p+1;
    }
    m_delta2[p] = lastPrefixIndex + (m_plm1 - p);
  }

  for(int p = 0; p < m_plm1; p++) {
    const int slen = getSuffixLength(m_pattern, m_patternLength, p);
    if(!CHAREQUALS1(m_pattern[p - slen], m_pattern[m_plm1 - slen])) {
      m_delta2[m_plm1 - slen] = m_plm1 - p + slen;
    }
  }
}

// true if the suffix of word starting from word[pos] is a prefix of word
bool BMAutomate::isPrefix(const TCHAR *word, int wordLength, int pos) const {
  const int suffixLength = wordLength - pos;
  // could also use the strncmp() library function here
  for(int i = 0; i < suffixLength; i++) {
    if(!CHAREQUALS1(word[i], word[pos+i])) {
      return false;
    }
  }
  return true;
}
 
// length of the longest suffix of word ending on word[pos].
// getSuffixLength("dddbcabc", 8, 4) = 2
int BMAutomate::getSuffixLength(const TCHAR *word, int wordLength, int pos) const {
  // increment suffix length i to the first mismatch or beginning of the word
  int i;
  for(i = 0; CHAREQUALS1(word[pos-i], word[wordLength-1-i]) && (i < pos); i++);
  return i;
}
