#include "stdafx.h"
#include <InputValue.h>

class ShiftAnd {
private:
#ifndef UNICODE
#define MASKLENGTH (0xff+1)
#else
#define MASKLENGTH (0xffff+1)
#endif

  UINT64 m_mask[MASKLENGTH];
  UINT64 m_s;
  int    m_patternLen;
public:
  ShiftAnd() : m_patternLen(0) {
  }
  ShiftAnd(             const String &pattern, bool ignoreCase=false);
  void     compile(     const String &pattern, bool ignoreCase=false);
  intptr_t search(      const String &str                           ) const;
  intptr_t searchApprox(const String &str, UINT maxErrors           ) const;
};

void ShiftAnd::compile(const String &pattern, bool ignoreCase) {
  m_patternLen = (int)pattern.length();
  if(m_patternLen >= 64) {
    throwException(_T("<%s> too long for shiftand-search. max length is 63"), pattern.cstr());
  }
  memset(m_mask, -1, sizeof(m_mask));
  for(int i = 0; i < m_patternLen; i++) {
    const _TUCHAR ch = pattern[i];
    m_mask[ch] &= ~((UINT64)1 << i);
    if (ignoreCase) {
      if (_istlower(ch)) {
        m_mask[_toupper(ch)] &= ~((UINT64)1 << i);
      } else if(_istupper(ch)) {
        m_mask[_tolower(ch)] &= ~((UINT64)1 << i);
      }
    }
  }
  m_s = (UINT64)1 << m_patternLen;
#ifdef _TEST_CLASS
  for(int i = 0; i < ARRAYSIZE(m_mask); i++) {
    const UINT64 mask = m_mask[i];
    if(mask != -1) {
      _tprintf(_T("mask[%c]:%s\n"), i, sprintbin(mask).cstr());
    }
  }
#endif
}

ShiftAnd::ShiftAnd(const String &pattern, bool ignoreCase) {
  compile(pattern, ignoreCase);
}

intptr_t ShiftAnd::search(const String &str) const {
  const UINT64 maskEnd  = m_s;
  UINT64       s        = ~1;
#ifdef _TEST_CLASS
  _tprintf(_T("m_s:%s\n"), sprintbin(maskEnd).cstr());
#endif
  for(const _TUCHAR *cp = str.cstr(), *last = cp + str.length(); cp < last;) {
    s = (s | m_mask[*(cp++)]) << 1;
#ifdef _TEST_CLASS
    _tprintf(_T("c:%c:%s\n"), *(cp-1), sprintbin(s).cstr());
#endif
    if((s & maskEnd) == 0) {
      return cp - str.cstr() - m_patternLen;
    }
  }
  return -1;
}

intptr_t ShiftAnd::searchApprox(const String &str, UINT maxErrors) const {
  if (maxErrors == 0) {
    return search(str);
  }
  const UINT64         maskEnd  = m_s;
  CompactArray<UINT64> s;
  s.add(0,(UINT64)~1,maxErrors+1);
  UINT64 *sfirst = &s.first(), *slast = &s.last();
#ifdef _TEST_CLASS
  _tprintf(_T("m_s:%s\n"), sprintbin(maskEnd).cstr());
#endif
  for(const _TUCHAR *cp = str.cstr(), *last = cp + str.length(); cp < last;) {
    const UINT64 mask = m_mask[*cp++];
    UINT64      *sp   = sfirst, olds = *sfirst;
    *sp = (olds | mask) << 1;

    while(sp++ < slast) {
      const UINT64 tmp = *sp;
      /* Substitution is all we care about */
      *sp = (olds & (tmp | mask)) << 1;
      olds = tmp;
    }
#ifdef _TEST_CLASS
    for(sp = sfirst; sp <= slast; sp++) {
      _tprintf(_T("c:%c:%s\n"), *(cp-1), sprintbin(*sp).cstr());
    }
#endif
    if((*slast & maskEnd) == 0) {
      return cp - str.cstr() - m_patternLen;
    }
  }
  return -1;
}

int _tmain(int argc, TCHAR **argv) {
  for(;;) {
    const String   pattern    = inputValue<String>(_T("Enter pattern:"));
    const bool     ignoreCase = inputValue<String>(_T("Ignore case[yn]:")) == _T("y");
    const ShiftAnd A(pattern, ignoreCase);
    const int      maxErrors  = inputValue<int>(_T("Enter maxErrors:"));
    for(;;) {
      const String text = inputValue<String>(_T("Enter text:"));
      if((text.length() > 0) && text[0] == _T('!')) break;
      const intptr_t i = A.searchApprox(text,maxErrors);
      _tprintf(_T("result:%zd\n"), i);
    }
  }
  return 0;
}
