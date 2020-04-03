#include "stdafx.h"
#include "Tokenizer.h"

#define MAXCHARVALUE 0xff

Tokenizer::Tokenizer(const char *str, const char *delimiters, char textQualifier, uint flags)
: m_delimiterSet(MAXCHARVALUE)
, m_flags(flags)
{
  init(str,delimiters,textQualifier);
}

Tokenizer::Tokenizer(const string &str, const string &delimiters, char textQualifier, uint flags)
: m_delimiterSet(MAXCHARVALUE)
, m_flags(flags)
{
  init(str.c_str(), delimiters.c_str(), textQualifier);
}

Tokenizer::Tokenizer(const Tokenizer &src)
: m_delimiterSet(src.m_delimiterSet)
, m_flags(src.m_flags)
{
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

void Tokenizer::init(const char *str, const char *delimiters, char textQualifier) {
  m_str              = str;
  m_textQualifier    = (m_flags & TOK_CSTRING) ? '"' : textQualifier;
  m_stringBuffer     = NULL;
  m_stringBufferSize = 0;
  for(const char *cp = (const char*)delimiters; *cp; cp++) {
    m_delimiterSet += (ulong)(*cp);                    // m_delimiterSet never contains '\0'
  }

  if(m_flags & TOK_INVERTSET) {
    m_delimiterSet.invert();
    m_delimiterSet.remove(0);                          // m_delimiterSet never contains '\0'
  }
  m_singleDelimiter = (m_flags & TOK_SINGLEDELIMITERS) ? true : false;
  m_next = str;

  if(m_delimiterSet.contains(m_textQualifier)) {
    throwException("TextQualifier '%c' cannot be contained in delimiterset (={%s})", m_textQualifier, delimiters);
  }
  if(!m_singleDelimiter) {
    while(m_delimiterSet.contains((ulong)(*m_next))) { // m_delimiterSet never contains '\0'
      m_next++;
    }
  }
}

string Tokenizer::next() {
  StringIndex tmp;
  nextIndex(tmp);
  if(tmp.m_length >= m_stringBufferSize) {
    SAFEDELETEARRAY(m_stringBuffer);
    m_stringBuffer = new char[m_stringBufferSize = 2 * (tmp.m_length+1)]; TRACE_NEW(m_stringBuffer);
  }
  strncpy(m_stringBuffer,(const char*)m_str+tmp.m_start, tmp.m_length);
  m_stringBuffer[tmp.m_length] = 0;
  return m_stringBuffer;
}

StringIndex Tokenizer::nextIndex() {
  StringIndex result;
  return nextIndex(result);
}

string Tokenizer::getRemaining() const {
  return (char*)m_next;
}

StringIndex &Tokenizer::nextIndex(StringIndex &tIndex) {
  if(!hasNext()) {
    throwException("%s:No such element", __FUNCTION__);
  }

  const char *pos = m_next;

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

static inline bool isodigit(char ch) {
 return ('0' <= ch) && (ch <= '7');
}

static inline uint octalToInt(uint ch) {
  return (ch- '0') & 0x7;
}

static inline uint hexToInt(uint ch) {
  return (iswdigit(ch) ? (ch - '0'): ((toupper(ch)) - 'A') + 10) & 0xf;
}

char *Tokenizer::parseCstring(const char *cp, string *result) { // static
  const char *start = cp++;
  for(;;) {
    switch(*cp) {
    case    0:
    case '\n':
      throwException("Unterminated string:%s", start);
    case '\"':
      cp++;
      return (char*)cp;
    case '\\':
      cp++;
      switch(*cp) {
      case 'a':  if(result) *result += '\a'; cp++; continue;
      case 'b' : if(result) *result += '\b'; cp++; continue;
      case 'f' : if(result) *result += '\f'; cp++; continue;
      case 'n' : if(result) *result += '\n'; cp++; continue;
      case 'r' : if(result) *result += '\r'; cp++; continue;
      case 't' : if(result) *result += '\t'; cp++; continue;
      case 'v' : if(result) *result += '\v'; cp++; continue;
      case '\\': if(result) *result += '\\'; cp++; continue;
      case '\'': if(result) *result += '\''; cp++; continue;
      case 'e' : if(result) *result += '\33'; cp++; continue;
      case '"' : if(result) *result += '\"'; cp++; continue;
      case '?' : if(result) *result += '\?'; cp++; continue;
      case '0' :
      case '1' :
      case '2' :
      case '3' :
      case '4' :
      case '5' :
      case '6' :
      case '7' :
        { uint v = octalToInt(*cp);
          for(cp++; isodigit(*cp) && (v <= 0xff); cp++) {
            v = (v<<3) | octalToInt(*cp);
          }
          if(v > 0xff) {
            cp--; v >>= 3;
          }
          if(result) *result += (char)v;
        }
        continue;
      case 'x' :
        { uint v = hexToInt(*cp);
          for(cp++; isxdigit((uint)*cp) && (v <= 0xff); cp++) {
            v = (v<<4) | hexToInt(*cp);
          }
          if(v > 0xff) {
            cp--;
            v >>= 4;
          }
          if(result) *result += (char)v;
        }
        continue;
      case 'u' :
        { uint v = hexToInt(*cp);
          for(cp++; isxdigit(*cp) && (v <= 0xffff); cp++) {
            v = (v<<4) | hexToInt(*cp);
          }
          if(v > 0xffff) {
            cp--;
            v >>= 4;
          }
          if(result) *result += (char)v;
        }
        continue;
      case 'U' :
        { uint64 v = hexToInt(*cp);
          for(cp++; isxdigit(*cp) && (v <= 0xffffffffu); cp++) {
            v = (v<<4) | hexToInt(*cp);
          }
          if(v > 0xffffffffu) {
            cp--;
            v >>= 4;
          }
          // ups.....v can be 32-bit unsigned
          if(result) *result += (char)v;
        }
        continue;
      default  :
        throwException("Invalid escape sequence in C string:%s", cp);
      } // end case '\\'
    default:
      if(result) *result += *cp;
      cp++;
      break;
    } // switch(*cp)
  } // for(;;)
}

double Tokenizer::getDouble() {
  string s = next();
  double result;
  if(sscanf(s.c_str(), "%le", &result) != 1) {
    throwException("%s:Expected double:<%s>", __FUNCTION__, s.c_str());
  }
  return result;
}

int Tokenizer::getInt(bool hex) {
  string s = next();
  int result;
  if(sscanf(s.c_str(), hex ? "%x" : "%ld", &result) != 1) {
    throwException("%s:Expected int:<%s>", __FUNCTION__, s.c_str());
  }
  return result;
}

uint Tokenizer::getUint(bool hex) {
  string s = next();
  uint result;
  if(sscanf(s.c_str(), hex ? "%x" : "%lu", &result) != 1) {
    throwException("%s:Expected unsigned int:<%s>", __FUNCTION__, s.c_str());
  }
  return result;
}

int64 Tokenizer::getInt64(bool hex) {
  string s = next();
  __int64 result;
  if(sscanf(s.c_str(), hex ? "%I64x" : "%I64d", &result) != 1) {
    throwException("%s:Expected __int64:<%s>", __FUNCTION__, s.c_str());
  }
  return result;
}

uint64 Tokenizer::getUint64(bool hex) {
  string s = next();
  uint64 result;
  if(sscanf(s.c_str(), hex ? "%I64x" : "%I64u", &result) != 1) {
    throwException("%s:Expected uint64:<%s>", __FUNCTION__, s.c_str());
  }
  return result;
}
