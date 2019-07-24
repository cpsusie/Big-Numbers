#pragma once

#include "BitSet.h"

#define TOK_INVERTSET        0x01  // set this flag to use complement of delimiterset
#define TOK_SINGLEDELIMITERS 0x02  // set this flag to allow only 1 delimiter between tokens, ie empty tokens
#define TOK_CSTRING          0x04  // set textQualifier to " and handle all c string escape sequences as \",\n,\r,\t...
                                   // as defined in https://en.wikipedia.org/wiki/Escape_sequences_in_C
                                   // escape sequences are not translated, and leading/ending " are not removed from string

class StringIndex {
private:
  size_t m_start;
  size_t m_length;
  friend class Tokenizer;
public:
  inline size_t getStart() const {
    return m_start;
  }

  inline size_t getEnd() const {
    return m_start + m_length - 1;
  }

  inline size_t getLength() const {
    return m_length;
  }

  inline bool contains(intptr_t index) const {
    return ((intptr_t)m_start <= index) && (index < (intptr_t)m_start + (intptr_t)m_length);
  }
};

class Tokenizer {
private:
  const TCHAR        *m_str;
  UINT                m_flags;
  BitSet              m_delimiterSet;
  TCHAR               m_textQualifier;
  bool                m_singleDelimiter;
  const TCHAR        *m_next;
  TCHAR              *m_stringBuffer;
  size_t              m_stringBufferSize; // in characters. not bytes
  void init(const TCHAR *str, const TCHAR *delimiters, TCHAR textQualifier);
  StringIndex &nextIndex(StringIndex &tIndex);
  void cleanup();
public:
  Tokenizer(const TCHAR  *str, const TCHAR  *delimiters, TCHAR textQualifier = 0, UINT flags = 0);
  Tokenizer(const String &str, const String &delimiters, TCHAR textQualifier = 0, UINT flags = 0);
  Tokenizer(const Tokenizer &src);
  Tokenizer &operator=(const Tokenizer &src);
  virtual ~Tokenizer();
  String next();
  String getRemaining() const;
  StringIndex nextIndex();
  inline bool hasNext() const {
    return *m_next != 0;
  }
  double           getDouble();
  int              getInt(   bool hex = false);
  UINT             getUint(  bool hex = false);
  __int64          getInt64( bool hex = false);
  unsigned __int64 getUint64(bool hex = false);
  bool             getBool(); // expect "true" or "false"
  // assume *cp == '"', beginning of C-string, scan c-string, returning pointer to TCHAR following terminating "
  // throws exception if invalid escape sequence,newline or not terminated string
  // escape sequences are defined in https://en.wikipedia.org/wiki/Escape_sequences_in_C
  // if result != NULL, it will contain the string without leading/ending " and escape sequnces translated
  static TCHAR *parseCstring(const TCHAR *cp, String *result=NULL);
};
