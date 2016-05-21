#include "pch.h"
#include <Tokenizer.h>

#ifdef UNICODE
#define MAXCHARVALUE 0xffff
#else
#define MAXCHARVALUE 0xff
#endif

DEFINECLASSNAME(Tokenizer);

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
    throwMethodException(s_className, _T("nextIndex"), _T("No such element"));
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
    throwMethodException(s_className, _T("getDouble"), _T("Expected double:<%s>"), s.cstr());
  }
  return result;
}

int Tokenizer::getInt() {
  String s = next();
  int result;
  if(_stscanf(s.cstr(), _T("%ld"), &result) != 1) {
    throwMethodException(s_className, _T("getInt"), _T("Expected int:<%s>"), s.cstr());
  }
  return result;
}

unsigned int Tokenizer::getUint() {
  String s = next();
  unsigned int result;
  if(_stscanf(s.cstr(), _T("%lu"), &result) != 1) {
    throwMethodException(s_className, _T("getUint"), _T("Expected unsigned int:<%s>"), s.cstr());
  }
  return result;
}

__int64 Tokenizer::getInt64() {
  String s = next();
  __int64 result;
  if(_stscanf(s.cstr(), _T("%I64d"), &result) != 1) {
    throwMethodException(s_className, _T("getInt64"), _T("Expected __int64:<%s>"), s.cstr());
  }
  return result;
}

unsigned __int64 Tokenizer::getUint64() {
  String s = next();
  unsigned __int64 result;
  if(_stscanf(s.cstr(), _T("%I64u"), &result) != 1) {
    throwMethodException(s_className, _T("getUint64"), _T("Expected unsigned __int64:<%s>"), s.cstr());
  }
  return result;
}
