#include "pch.h"
#include <Tokenizer.h>

#if defined(UNICODE)
#define MAXCHARVALUE 0xffff
#else
#define MAXCHARVALUE 0xff
#endif

Tokenizer::Tokenizer(const TCHAR *str, const TCHAR *delimiters, TCHAR textQualifier, UINT flags)
: m_delimiterSet(MAXCHARVALUE)
, m_flags(flags)
{
  init(str,delimiters,textQualifier);
}

Tokenizer::Tokenizer(const String &str, const String &delimiters, TCHAR textQualifier, UINT flags)
: m_delimiterSet(MAXCHARVALUE)
, m_flags(flags)
{
  init(str.cstr(), delimiters.cstr(), textQualifier);
}

Tokenizer::Tokenizer(const Tokenizer &src)
: m_delimiterSet(src.m_delimiterSet)
, m_flags(src.m_flags)
{
  m_str              = src.m_str;
  m_textQualifier    = src.m_textQualifier;
  m_singleDelimiter  = src.m_singleDelimiter;
  m_next             = src.m_next;
  m_stringBuffer     = nullptr;
  m_stringBufferSize = 0;
}

Tokenizer &Tokenizer::operator=(const Tokenizer &src) {
  if(this == &src) {
    return *this;
  }
  m_str             = src.m_str;
  m_flags           = src.m_flags;
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
  SAFEDELETEARRAY(m_stringBuffer);
  m_stringBufferSize = 0;
}

void Tokenizer::init(const TCHAR *str, const TCHAR *delimiters, TCHAR textQualifier) {
  m_str              = str;
  m_textQualifier    = (m_flags & TOK_CSTRING) ? '"' : textQualifier;
  m_stringBuffer     = nullptr;
  m_stringBufferSize = 0;
  for(const TCHAR *cp = (const TCHAR*)delimiters; *cp; cp++) {
    m_delimiterSet += (ULONG)(*cp);                    // m_delimiterSet never contains '\0'
  }

  if(m_flags & TOK_INVERTSET) {
    m_delimiterSet.invert();
    m_delimiterSet.remove(0);                          // m_delimiterSet never contains '\0'
  }
  m_singleDelimiter = (m_flags & TOK_SINGLEDELIMITERS) ? true : false;
  m_next = str;

  if(m_delimiterSet.contains(m_textQualifier)) {
    throwException(_T("TextQualifier '%c' cannot be contained in delimiterset (={%s})"), m_textQualifier, delimiters);
  }
  if(!m_singleDelimiter) {
    while(m_delimiterSet.contains((ULONG)(*m_next))) { // m_delimiterSet never contains '\0'
      m_next++;
    }
  }
}

String Tokenizer::next() {
  StringIndex tmp;
  nextIndex(tmp);
  if(tmp.m_length >= m_stringBufferSize) {
    SAFEDELETEARRAY(m_stringBuffer);
    m_stringBuffer = new TCHAR[m_stringBufferSize = 2 * (tmp.m_length+1)]; TRACE_NEW(m_stringBuffer);
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
    throwException(_T("%s:No such element"), __TFUNCTION__);
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
      if(m_flags&TOK_CSTRING) {
        m_next = parseCstring(m_next);
        continue;
      } else {
        inString = !inString;
      }
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

static inline bool isodigit(TCHAR ch) {
 return ('0' <= ch) && (ch <= '7');
}

static inline UINT octalToInt(UINT ch) {
  return (ch- '0') & 0x7;
}

static inline UINT hexToInt(UINT ch) {
  return (iswdigit(ch) ? (ch - '0'): ((toupper(ch)) - 'A') + 10) & 0xf;
}

TCHAR *Tokenizer::parseCstring(const TCHAR *cp, String *result) { // static
  const TCHAR *start = cp++;
  for(;;) {
    switch(*cp) {
    case    0:
    case '\n':
      throwException(_T("Unterminated string:%s"), start);
    case '\"':
      cp++;
      return (TCHAR*)cp;
    case '\\':
      cp++;
      switch(*cp) {
      case 'a':  if(result) *result += _T('\a'); cp++; continue;
      case 'b' : if(result) *result += _T('\b'); cp++; continue;
      case 'f' : if(result) *result += _T('\f'); cp++; continue;
      case 'n' : if(result) *result += _T('\n'); cp++; continue;
      case 'r' : if(result) *result += _T('\r'); cp++; continue;
      case 't' : if(result) *result += _T('\t'); cp++; continue;
      case 'v' : if(result) *result += _T('\v'); cp++; continue;
      case '\\': if(result) *result += _T('\\'); cp++; continue;
      case '\'': if(result) *result += _T('\''); cp++; continue;
      case 'e' : if(result) *result += _T('\33'); cp++; continue;
      case '"' : if(result) *result += _T('\"'); cp++; continue;
      case '?' : if(result) *result += _T('\?'); cp++; continue;
      case '0' :
      case '1' :
      case '2' :
      case '3' :
      case '4' :
      case '5' :
      case '6' :
      case '7' :
        { UINT v = octalToInt(*cp);
          for(cp++; isodigit(*cp) && (v <= 0xff); cp++) {
            v = (v<<3) | octalToInt(*cp);
          }
          if(v > 0xff) {
            cp--; v >>= 3;
          }
          if(result) *result += (TCHAR)v;
        }
        continue;
      case 'x' :
        { UINT v = hexToInt(*cp);
          for(cp++; isxdigit((UINT)*cp) && (v <= 0xff); cp++) {
            v = (v<<4) | hexToInt(*cp);
          }
          if(v > 0xff) {
            cp--;
            v >>= 4;
          }
          if(result) *result += (TCHAR)v;
        }
        continue;
      case 'u' :
        { UINT v = hexToInt(*cp);
          for(cp++; isxdigit(*cp) && (v <= 0xffff); cp++) {
            v = (v<<4) | hexToInt(*cp);
          }
          if(v > 0xffff) {
            cp--;
            v >>= 4;
          }
          if(result) *result += (TCHAR)v;
        }
        continue;
      case 'U' :
        { UINT64 v = hexToInt(*cp);
          for(cp++; isxdigit(*cp) && (v <= 0xffffffffu); cp++) {
            v = (v<<4) | hexToInt(*cp);
          }
          if(v > 0xffffffffu) {
            cp--;
            v >>= 4;
          }
          // ups.....v can be 32-bit unsigned
          if(result) *result += (TCHAR)v;
        }
        continue;
      default  :
        throwException(_T("Invalid escape sequence in C string:%s"), cp);
      } // end case '\\'
    default:
      if(result) *result += *cp;
      cp++;
      break;
    } // switch(*cp)
  } // for(;;)
}

double Tokenizer::getDouble() {
  String s = next();
  double result;
  if(_stscanf(s.cstr(), _T("%le"), &result) != 1) {
    throwException(_T("%s:Expected double:<%s>"), __TFUNCTION__, s.cstr());
  }
  return result;
}

int Tokenizer::getInt(bool hex) {
  String s = next();
  int result;
  if(_stscanf(s.cstr(), hex ? _T("%x") : _T("%ld"), &result) != 1) {
    throwException(_T("%s:Expected int:<%s>"), __TFUNCTION__, s.cstr());
  }
  return result;
}

UINT Tokenizer::getUint(bool hex) {
  String s = next();
  UINT result;
  if(_stscanf(s.cstr(), hex ? _T("%x") : _T("%lu"), &result) != 1) {
    throwException(_T("%s:Expected unsigned int:<%s>"), __TFUNCTION__, s.cstr());
  }
  return result;
}

__int64 Tokenizer::getInt64(bool hex) {
  String s = next();
  __int64 result;
  if(_stscanf(s.cstr(), hex ? _T("%I64x") : _T("%I64d"), &result) != 1) {
    throwException(_T("%s:Expected __int64:<%s>"), __TFUNCTION__, s.cstr());
  }
  return result;
}

unsigned __int64 Tokenizer::getUint64(bool hex) {
  String s = next();
  unsigned __int64 result;
  if(_stscanf(s.cstr(), hex ? _T("%I64x") : _T("%I64u"), &result) != 1) {
    throwException(_T("%s:Expected unsigned __int64:<%s>"), __TFUNCTION__, s.cstr());
  }
  return result;
}

size_t Tokenizer::getSizet(bool hex) {
#if defined(IS64BIT)
  return getUint64(hex);
#else
  return getUint(hex);
#endif // IS64BIT
}

bool Tokenizer::getBool() { // expect "true" or "false"
  String s = next();
  return strToBool(s.cstr());
}
