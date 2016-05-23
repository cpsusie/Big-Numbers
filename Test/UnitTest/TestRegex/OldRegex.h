#pragma once

#include "MyUtil.h"

#define RE_NREGS 10

class OldRegexRegisters {
public:
  int start[RE_NREGS];
  int end[RE_NREGS];
};

// This data structure is used to represent a compiled pattern.

class OldRegex {
private:
  char *m_buffer;                        // Space holding the compiled pattern commands.
  int   m_allocated;                     // Size of space that  buffer  points to
  int   m_used;                          // Length of portion of buffer actually occupied
  bool  m_fastMap[256];                  // re_search uses the fastmap, to skip quickly over totally 
                                         // implausible characters

  const unsigned char *m_translateTable; // Translate table to apply to all characters before comparing.
                                         // Or zero for no translation.
                                         // The translation is applied to a pattern when it is compiled
                                         // and to data when it is matched.

  bool  m_canBeNull;                     // Set to true in createFastmap
                                         // if this pattern might match the null String.
                                         // It does not necessarily match the null String
                                         // in that case, but if this is false, it cannot.

  bool  m_hasCompiled;
  mutable int m_resultLength;

  void compilePattern1(   const char     *pattern);

  void createFastMap();
  int  search1(           const char     *sring,
                          int             size,
                          int             startPos,
                          int             range,
                          OldRegexRegisters *registers) const;

  int  search2(           const char     *string1,
                          int             size1,
                          const char     *string2,
                          int             size2,
                          int             startPos,
                          register int    range,
                          OldRegexRegisters *registers,
                          int             mstop) const;

  int  match(             const char     *string,
                          int             size,
                          int             pos,
                          OldRegexRegisters *registers) const;

  int  match2(            const char     *string1,
                          int             size1,
                          const char     *string2,
                          int             size2,
                          int             pos,
                          OldRegexRegisters *registers,
                          int             mstop) const;
  int  compareBytes(const char *s1, const char *s2, register int length) const;
  void init(const OldRegex *src = NULL);
  void cleanup();
public:
  OldRegex();
  OldRegex(const String &pattern, const unsigned char *translateTable = NULL);
  OldRegex(const char   *pattern, const unsigned char *translateTable = NULL);
  OldRegex(const OldRegex &rhs);
  OldRegex &operator=(const OldRegex &rhs);
  virtual ~OldRegex();
  void compilePattern(const String &pattern, const unsigned char *translateTable = NULL);
  void compilePattern(const char   *pattern, const unsigned char *translateTable = NULL);
  int  search( const String &text, bool forward = true, int startPos = -1, OldRegexRegisters *registers = NULL) const; // search for the compiled expression in text
  int  search( const char   *text, bool forward = true, int startPos = -1, OldRegexRegisters *registers = NULL) const;
  bool match(  const String &text, OldRegexRegisters *registers = NULL) const;  // check for exact match
  bool match(  const char   *text, OldRegexRegisters *registers = NULL) const;

  int getResultLength() const {
    return m_resultLength;
  }
  
  const bool *getFastMap() const {
    return m_fastMap;
  }
  
  bool getMatchEmpty() const {
    return m_canBeNull;
  }
  
  void dump(FILE *f=stdout) const;
};


