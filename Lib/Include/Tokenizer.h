#pragma once

#include "MyUtil.h"
#include "BitSet.h"

#define TOK_INVERTSET        1  // set this flag to use complement of delimiterset
#define TOK_SINGLEDELIMITERS 2  // set this flag to allow only 1 delimiter between tokens, ie empty tokens

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
  DECLARECLASSNAME;
  const TCHAR        *m_str;
  BitSet              m_delimiterSet;
  TCHAR               m_textQualifier;
  bool                m_singleDelimiter;
  const TCHAR        *m_next;
  TCHAR              *m_stringBuffer;
  size_t              m_stringBufferSize; // in characters. not bytes
  void init(const TCHAR *str, const TCHAR *delimiters, TCHAR textQualifier, int flags);
  StringIndex &nextIndex(StringIndex &tIndex);
  void cleanup();
public:
  Tokenizer(const TCHAR  *str, const TCHAR  *delimiters, TCHAR textQualifier = 0, int flags = 0);
  Tokenizer(const String &str, const String &delimiters, TCHAR textQualifier = 0, int flags = 0);
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
  int              getInt();
  unsigned int     getUint();
  __int64          getInt64();
  unsigned __int64 getUint64();

};
