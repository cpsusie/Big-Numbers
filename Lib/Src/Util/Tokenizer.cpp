#include "pch.h"
#include <Tokenizer.h>

#ifdef UNICODE
#define MAXCHARVALUE 0xffff
#else
#define MAXCHARVALUE 0xff
#endif

Tokenizer::Tokenizer(const TCHAR *str, const TCHAR *delimiters, TCHAR textQualifier, int flags) : m_delimiterSet(MAXCHARVALUE) {
  init(str,delimiters,textQualifier,flags);
}

Tokenizer::Tokenizer(const String &str, const String &delimiters, TCHAR textQualifier, int flags) : m_delimiterSet(MAXCHARVALUE) {
  init(str.cstr(), delimiters.cstr(), textQualifier, flags);
}

Tokenizer::Tokenizer(const Tokenizer &src) : m_delimiterSet(src.m_delimiterSet) {
  m_str              = src.m_str;
  m_textQualifier    = src.m_textQualifier;
  m_singleDelimiter  = src.m_singleDelimiter;
  m_next             = src.m_next;
  m_stringBuffer     = NULL;
  m_stringBufferSize = 0;
}

Tokenizer &Tokenizer::operator=(const Tokenizer &src) {
  if(this == &src) {
    return *this;
  }
  m_str             = src.m_str;
  m_delimiterSet    = src.m_delimiterSet;
  m_textQualifier   = src.m_textQualifier;
  m_singleDelimiter = src.m_singleDelimiter;
  m_next            = src.m_next;
  return *this;
}

Tokenizer::~Tokenizer() {
  cleanup();
}

void Tokenizer::cleanup() {
  delete[] m_stringBuffer;
  m_stringBuffer     = NULL;
  m_stringBufferSize = 0;
}

void Tokenizer::init(const TCHAR *str, const TCHAR *delimiters, TCHAR textQualifier, int flags) {
  m_str              = str;
  m_textQualifier    = textQualifier;
  m_stringBuffer     = NULL;
  m_stringBufferSize = 0;
  for(const TCHAR *cp = (const TCHAR*)delimiters; *cp; cp++) {
    m_delimiterSet += (unsigned long)(*cp);                    // m_delimiterSet never contains '\0'
  }

  if(flags & TOK_INVERTSET) { 
    m_delimiterSet.invert();
    m_delimiterSet.remove(0);                                  // m_delimiterSet never contains '\0'
  }
  m_singleDelimiter = (flags & TOK_SINGLEDELIMITERS) ? true : false;
  m_next = str;

  if(!m_singleDelimiter) {
    while(m_delimiterSet.contains((unsigned long)(*m_next))) { // m_delimiterSet never contains '\0'
      m_next++;
    }
  }
}

String Tokenizer::next() {
  StringIndex tmp;
  nextIndex(tmp);
  if(tmp.m_length >= m_stringBufferSize) {
    if(m_stringBuffer) {
      delete[] m_stringBuffer;
    }
    m_stringBuffer = new TCHAR[m_stringBufferSize = 2 * (tmp.m_length+1)];
  }
  _tcsncpy(m_stringBuffer,(const TCHAR*)m_str+tmp.m_start, tmp.m_length);
  m_stringBuffer[tmp.m_length] = 0;
  return m_stringBuffer;
}

StringIndex Tokenizer::nextIndex() {
  StringIndex result;
  return nextIndex(result);
}

String Tokenizer::getRemaining() const {
  return (TCHAR*)m_next;
}

StringIndex &Tokenizer::nextIndex(StringIndex &tIndex) {
  if(!hasNext()) {
    throwException(_T("%s:No such element"), _T(__FUNCTION__));
  }

  const TCHAR *pos = m_next;

  if(!m_singleDelimiter) {
    if(*m_next != m_textQualifier) {
      m_next++;
    }
  }

  bool inString = false;
  while(*m_next) {
    if(*m_next == m_textQualifier) {
      inString = !inString;
    } else {
      if(!inString && m_delimiterSet.contains(*m_next)) {
        break;
      }
    }
    m_next++;
  }

  tIndex.m_start  = pos - m_str;
  tIndex.m_length = m_next - pos;

  if(*m_next == 0) {
    return tIndex;
  }

  m_next++;
  if(!m_singleDelimiter) {
    while(m_delimiterSet.contains(*m_next)) {
      m_next++; // m_delimiterSet never contains '\0'
    }
  }
  return tIndex;
}

double Tokenizer::getDouble() {
  String s = next();
  double result;
  if(_stscanf(s.cstr(), _T("%le"), &result) != 1) {
    throwException(_T("%s:Expected double:<%s>"), _T(__FUNCTION__), s.cstr());
  }
  return result;
}

int Tokenizer::getInt() {
  String s = next();
  int result;
  if(_stscanf(s.cstr(), _T("%ld"), &result) != 1) {
    throwException(_T("%s:Expected int:<%s>"), _T(__FUNCTION__), s.cstr());
  }
  return result;
}

unsigned int Tokenizer::getUint() {
  String s = next();
  unsigned int result;
  if(_stscanf(s.cstr(), _T("%lu"), &result) != 1) {
    throwException(_T("%s:Expected unsigned int:<%s>"), _T(__FUNCTION__), s.cstr());
  }
  return result;
}

__int64 Tokenizer::getInt64() {
  String s = next();
  __int64 result;
  if(_stscanf(s.cstr(), _T("%I64d"), &result) != 1) {
    throwException(_T("%s:Expected __int64:<%s>"), _T(__FUNCTION__), s.cstr());
  }
  return result;
}

unsigned __int64 Tokenizer::getUint64() {
  String s = next();
  unsigned __int64 result;
  if(_stscanf(s.cstr(), _T("%I64u"), &result) != 1) {
    throwException(_T("%s:Expected unsigned __int64:<%s>"), _T(__FUNCTION__), s.cstr());
  }
  return result;
}
