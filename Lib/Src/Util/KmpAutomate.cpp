#include "pch.h"
#include <KMPAutomate.h>

KMPAutomate::KMPAutomate() {
  initPointers();
  compilePattern(_T(""));
}

KMPAutomate::KMPAutomate(const String &pattern, bool forwardSearch, const unsigned char *translateTable) {
  initPointers();
  compilePattern(pattern.cstr(), pattern.length(), forwardSearch, translateTable);
}

KMPAutomate::KMPAutomate(const TCHAR *pattern, intptr_t patternLength, bool forwardSearch, const unsigned char *translateTable) {
  initPointers();
  compilePattern(pattern, patternLength, forwardSearch, translateTable);
}

KMPAutomate::KMPAutomate(const KMPAutomate &src) {
  allocate(src.m_patternLength);
  copyFrom(src);
}

KMPAutomate::~KMPAutomate() {
  deallocate();
}

KMPAutomate &KMPAutomate::operator=(const KMPAutomate &src) {
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

void KMPAutomate::compilePattern(const String &pattern, bool forwardSearch, const unsigned char *translateTable) {
  compilePattern(pattern.cstr(), pattern.length(), forwardSearch, translateTable);
}

void KMPAutomate::compilePattern(const TCHAR *pattern, intptr_t patternLength, bool forwardSearch, const unsigned char *translateTable) {
  if(patternLength < 0) {
    patternLength = _tcsclen(pattern);
  }
  if(patternLength != m_patternLength) {
    deallocate();
    allocate(patternLength);
  }
  MEMCPY(m_pattern, pattern, m_tableSize); // NOT strncpy or strcpy here. '\0' may occur in pattern
  m_forwardSearch  = forwardSearch;
  m_translateTable = translateTable;

  if(m_forwardSearch) {
    compileForward(); 
  } else {
    compileBackward();
  }
}

#define CHAREQUALS(c1,c2) (m_translateTable ? (m_translateTable[(unsigned char)(c1)] == m_translateTable[(unsigned char)(c2)]) : (c1==c2))

void KMPAutomate::compileForward() {
  m_next[0] = -1;
  int j = -1;
  size_t i = 0;
  do { 
    if(j == -1 || CHAREQUALS(m_pattern[i], m_pattern[j])) {
      i++;
      j++;
      m_next[i] = CHAREQUALS(m_pattern[j], m_pattern[i]) ? m_next[j] : j;
    } else {
      j = m_next[j]; 
    }
  }
  while(i < m_patternLength);
}

static TCHAR *reversebuf(TCHAR *s, size_t size) {
  TCHAR tmp,*l = s, *r = s + size - 1;
  while(l < r) {
    tmp = *l; *(l++) = *r; *(r--) = tmp;
  }
  return s;
}

void KMPAutomate::compileBackward() {
  reversebuf(m_pattern, m_patternLength);
  compileForward();
}

intptr_t KMPAutomate::search(const String &text) const {
  return search(text.cstr(), text.length());
}

intptr_t KMPAutomate::search(const TCHAR *text, intptr_t textLength) const {
  if(textLength < 0) {
    textLength = _tcsclen(text);
  }
  return m_forwardSearch ? searchForward(text, textLength) : searchBackward(text, textLength);
}

// text points to start of String, textLength is the length of the String.
// if length is < 0 the length is determined by strlen(text)
// searchForward finds the first occurence of the pattern in text
// searchBackward finds the last occurence of the pattern in text
intptr_t KMPAutomate::searchForward(const TCHAR *text, size_t textLength) const {
  for(intptr_t i = 0, j = 0; i < (intptr_t)textLength; ) {
    if(j == -1 || CHAREQUALS(m_pattern[j], text[i])) { 
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

intptr_t KMPAutomate::searchBackward(const TCHAR *text, size_t textLength) const {
  for(intptr_t i = textLength - 1, j = 0; i >= 0;) {
    if(j == -1 || CHAREQUALS(m_pattern[j], text[i])) { 
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

void KMPAutomate::allocate(size_t patternLength) {
  m_tableSize = (m_patternLength = patternLength) + 1;
  m_next      = new int[  m_tableSize];
  m_pattern   = new TCHAR[m_tableSize];
}

void KMPAutomate::deallocate() {
  if(m_pattern) {
    delete[] m_pattern;
  }
  if(m_next) {
    delete[] m_next;
  }
  initPointers();
}

void KMPAutomate::initPointers() {
  m_pattern       = NULL;
  m_next          = NULL;
  m_patternLength = -1;
  m_tableSize     =  0;
}

void KMPAutomate::copyFrom(const KMPAutomate &src) {
  if(m_tableSize > 0) {
    memcpy(m_next   , src.m_next   , sizeof(m_next[0]    )*m_tableSize);
    memcpy(m_pattern, src.m_pattern, sizeof(m_pattern[0] )*m_tableSize);
  }
  m_forwardSearch  = src.m_forwardSearch;
  m_translateTable = src.m_translateTable;
}
