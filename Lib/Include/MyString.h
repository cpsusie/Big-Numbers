#pragma once

#include <stdarg.h>
#include "Unicode.h"
#include "PragmaLib.h"
#include <comdef.h>
#include <atlconv.h>

#define EMPTYSTRING _T("")

#define __TFILE__                   _T(__FILE__    )
#define __TFUNCTION__               _T(__FUNCTION__)
#define __TFUNCSIG__                _T(__FUNCSIG__ )

#define DEFINEMETHODNAME            static TCHAR const* const method = __TFUNCTION__
#define DECLARECLASSNAME            static TCHAR const* const s_className
#define DEFINECLASSNAME(className)  TCHAR const* const className::s_className = _T(#className)

#if !defined(UNICODE)

#define USES_ACONVERSION
#define USES_WCONVERSION USES_CONVERSION

#define ASTR2TSTR(s)     (s)
#define TSTR2ASTR(s)     (s)
#define WSTR2TSTR(s)  W2T(s)
#define TSTR2WSTR(s)  T2W(s)

#else // UNICODE

#define USES_ACONVERSION USES_CONVERSION
#define USES_WCONVERSION

#define ASTR2TSTR(s)  A2T(s)
#define TSTR2ASTR(s)  T2A(s)
#define WSTR2TSTR(s)     (s)
#define TSTR2WSTR(s)     (s)

#endif // UNICODE

#define A2TNULL(s) ((s)?ASTR2TSTR(s):_T("null"))
#define W2TNULL(s) ((s)?WSTR2TSTR(s):_T("null"))

ULONG strHash(const TCHAR * const &s);
inline int strHashCmp(   const TCHAR * const &s1, const TCHAR * const &s2) {
  return _tcscmp(s1, s2);
}
ULONG striHash(     const TCHAR * const &s                          );
inline int striHashCmp(const TCHAR * const &s1, const TCHAR * const &s2) {
  return _tcsicmp(s1, s2);
}
ULONG strnHash(     const TCHAR * const &s                          , size_t n);
int   strnHashCmp(  const TCHAR * const &s1, const TCHAR * const &s2, size_t n1, size_t n2);
ULONG strniHash(    const TCHAR * const &s                          , size_t n);
int   strniHashCmp( const TCHAR * const &s1, const TCHAR * const &s2, size_t n1, size_t n2);

template<typename DstCharType, typename SrcCharType> DstCharType *strCpy(DstCharType *dst, const SrcCharType *src) {
  DstCharType *ret = dst;
  while(*(dst++) = (DstCharType)(*(src++)));
  return ret;
}

template<typename DstCharType, typename SrcCharType> DstCharType *strnCpy(DstCharType *dst, const SrcCharType *src, size_t num) {
  DstCharType *ret = dst, *maxDst = dst+num;
  while((dst < maxDst) && (*(dst++) = (DstCharType)(*(src++))));
  while(dst < maxDst) *(dst++) = 0;
  return ret;
}

template<typename CharType> CharType *strUpr(CharType *str) {
  return (sizeof(CharType) == 1) ? (CharType*)_strupr((char   *)((void*)str))
                                 : (CharType*)_wcsupr((wchar_t*)((void*)str));
}

template<typename CharType> size_t strLen(const CharType *str) {
  return (sizeof(CharType) == 1) ? strlen((char   *)((void*)str))
                                 : wcslen((wchar_t*)((void*)str));
}

template<typename CharType> CharType *strChr(CharType *str, CharType ch) {
  return (sizeof(CharType) == 1) ? (CharType*)strchr((char   *)((void*)str), (char   )ch)
                                 : (CharType*)wcschr((wchar_t*)((void*)str), (wchar_t)ch);
}

template<typename CharType> const CharType *strChr(const CharType *str, CharType ch) {
  return (sizeof(CharType) == 1) ? (const CharType*)strchr((const char   *)str, (char   )ch)
                                 : (const CharType*)wcschr((const wchar_t*)str, (wchar_t)ch);
}

template<typename CharType> CharType *strDup(const CharType *str) {
  return (sizeof(CharType) == 1) ? (CharType*)_strdup((const char   *)str)
                                 : (CharType*)_wcsdup((const wchar_t*)str);
}

template<typename CharType> CharType *strRev(CharType *str) {
  return (sizeof(CharType) == 1) ? (CharType*)_strrev((char   *)str)
                                 : (CharType*)_wcsrev((wchar_t*)str);
}

class String {
private:
  size_t m_len, m_capacity;
  TCHAR *m_buf;
  static TCHAR *newCharBuffer(const TCHAR *s, size_t &length, size_t &capacity);
  void indexError(size_t index) const;
  inline bool needReallocate(size_t newSize) const {
    return (newSize >= m_capacity) || (newSize + 100 < m_capacity);
  }

public:
  String();
  String(const String  &s);
  String(const wchar_t *s);
  String(const char    *s);
  explicit String(char    ch);
  explicit String(SHORT   n );
  explicit String(USHORT  n );
  explicit String(INT     n );
  explicit String(UINT    n );
  explicit String(LONG    n );
  explicit String(ULONG   n );
  explicit String(INT64   n );
  explicit String(UINT64  n );
  explicit String(float   x );
  explicit String(double  x );

  ~String();
  String      &operator=(const String  &rhs);
  String      &operator=(const char    *rhs);
  String      &operator=(const wchar_t *rhs);

  // Returns TCHAR at position index
  inline TCHAR &operator[](size_t index) {
    if(index >= m_len) indexError(index);
    return m_buf[index];
  }

  // Returns TCHAR at position index
  inline const TCHAR &operator[](size_t index) const {
    if(index >= m_len) indexError(index);
    return m_buf[index];
  }
  // Returns the last character in *this. _T('\0') is isEmpty()
  inline TCHAR        last() const { return m_len ? m_buf[m_len-1] : 0; }
  inline TCHAR       *cstr()       { return m_buf; }
  inline const TCHAR *cstr() const { return m_buf; }

  // Remove characters at position pos, pos+1, ...pos+count-1
  String &remove(size_t pos, size_t count = 1);

  // Remove the last character if any. return *this
  inline String &removeLast() {
    return (m_len > 0) ? remove(m_len-1,1) : *this;
  }

  // Insert ch into String at position pos. return *this
  String &insert(size_t pos, TCHAR ch);

  // Insert count occurrences of ch into String at position pos. return *this
  String &insert(size_t pos, size_t count, TCHAR ch);

  // Insert s into String at position pos. return *this
  String &insert(size_t pos, const String &s);

  inline int compare(const String &s) const {
    return strnHashCmp(m_buf, s.m_buf, length(), s.length());
  }
  inline int compareIgnoreCase(const String &s) const {
    return strniHashCmp(m_buf, s.m_buf, length(), s.length());
  }

  inline bool equalsIgnoreCase(const String &s) const {
    return compareIgnoreCase(s) == 0;
  }

  friend String operator+(const String &lhs, const String &rhs);

  // Append rhs to this. return *this
  String &operator+=(const String &rhs);

  // Append rhs to this. return *this
  String &operator+=(const char    *rhs);
  String &operator+=(const wchar_t *rhs);

  // Append ch to this. return *this
  inline String &operator+=(TCHAR ch) {
    const TCHAR tmp[] = { ch, 0 };
    return *this += tmp;
  }

  // Substitute every occurrence of from in s with to. Return this
  String &replace(  TCHAR         from, TCHAR         to);

  // Substitute every occurrence of from in s with to. Return this
  template<typename ToType> String &replace(TCHAR from, const ToType *to) {
    return replace(from, String(to));
  }

  // Substitute every occurrence of from in s with to. Return this
  template<typename FromType> String &replace(const FromType *from, TCHAR to) {
    return replace(String(from), to);
  }

  // Substitute every occurrence of from in s with to. Return this
  template<typename FromType, typename ToType> String &replace(const FromType *from, const ToType *to) {
    return replace(String(from), String(to));
  }

  // Substitute every occurrence of from in s with to. Return this
  String &replace(  TCHAR         from, const String &to);

  // Substitute every occurrence of from in s with to. Return this
  String &replace(  const String &from, TCHAR         to);

  // Substitute every occurrence of from in s with to. Return this
  String &replace(  const String &from, const String &to);

  // Remove leading and trailing spaces. Return this
  String &trim();

  // Remove leading spaces. Return this
  String &trimLeft();

  // Remove trailing spaces. Return this
  String &trimRight();

  // Return index of first occurrence of str starting at position from, -1 if not found
  intptr_t      find(const TCHAR  *str, size_t from = 0) const;
  intptr_t      find(const String &str, size_t from = 0) const;

   // Return index of first occurrence of ch starting at position from, -1 if not found
  intptr_t      find(TCHAR         ch , size_t from = 0) const;

  // Return index of last occurrence of ch, -1 if not found
  intptr_t     rfind(TCHAR         ch                  ) const;

  // Return length of String
  inline size_t length()  const { return m_len; }
  // Return true if length() == 0
  inline bool   isEmpty() const { return m_len == 0; }

  // Return substring "s[0]s[1]...s[len-1]".                          ex. left("abc"  ,2)   = "ab"
  friend String left(  const String &str, intptr_t length);

  // Return substring s[s.len-length],s[s.len-length+1]...s[s.len-1]. ex. right("abc" ,2)   = "bc"
  friend String right( const String &str, intptr_t length);

  // Return substring "str[from]str[from+1]...str[from+length-1]"     ex. substr("abcd",1,2) = "bc"
  friend String substr(const String &str, intptr_t from, intptr_t length);

  // Return reverse String
  friend String rev(   const String &str);

  // Return String with length length, filled with ch. return "" if length <= 0
  friend String spaceString(std::streamsize length, TCHAR ch = _T(' '));

  // Same as vsprintf. Return *this
  String &vprintf(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

  // Same as sprintf. Return *this
  String &printf( _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

  inline const String &toString() const {
    return *this;
  }

  inline ULONG hashCode() const {
    return strnHash(cstr(),length());
  }

  inline bool isAscii() const {
    for(const _TUCHAR *cp = (_TUCHAR*)m_buf, *endp = cp + m_len; cp < endp;) {
      if(!isascii(*(cp++))) {
        return false;
      }
    }
    return true;
  }

  // works only for characters [0..255]
  static const TCHAR  upperCaseTranslate[256];
  static const TCHAR *lowerCaseTranslate;
  static const TCHAR  upperCaseAccentTranslate[256];
};

std::ostream  &operator<<(std::ostream  &out, const String &str);
std::istream  &operator>>(std::istream  &in,        String &str);
std::wostream &operator<<(std::wostream &out, const String &str);
std::wistream &operator>>(std::wistream &in,        String &str);

inline bool operator==(const String &lhs, const String &rhs) {
  return lhs.compare(rhs) == 0;
}
inline bool operator==(const String &lhs, const TCHAR  *rhs) {
  return strnHashCmp(lhs.cstr(), rhs, lhs.length(), _tcslen(rhs)) == 0;
}
inline bool operator==(const TCHAR  *lhs, const String &rhs) {
  return strnHashCmp(lhs, rhs.cstr(), _tcslen(lhs), rhs.length()) == 0;
}

inline bool operator!=(const String &lhs, const String &rhs) {
  return lhs.compare(rhs) != 0;
}
inline bool operator!=(const String &lhs, const TCHAR  *rhs) {
  return strnHashCmp(lhs.cstr(), rhs, lhs.length(), _tcslen(rhs)) != 0;
}
inline bool operator!=(const TCHAR  *lhs, const String &rhs) {
  return strnHashCmp(lhs, rhs.cstr(), _tcslen(lhs), rhs.length()) != 0;
}

inline bool operator> (const String &lhs, const String &rhs) {
  return lhs.compare(rhs) > 0;
}
inline bool operator< (const String &lhs, const String &rhs) {
  return lhs.compare(rhs) < 0;
}
inline bool operator>=(const String &lhs, const String &rhs) {
  return lhs.compare(rhs) >= 0;
}
inline bool operator<=(const String &lhs, const String &rhs) {
  return lhs.compare(rhs) <= 0;
}

// Return copy of str, without leading and trailing spaces
inline String trim(const String &str) {
  return String(str).trim();
}

// Return copy og str, without leading spaces
inline String trimLeft(const String &str) {
  return String(str).trimLeft();
}

// Return copy of str, without trailing spaces
inline String trimRight(const String &str) {
  return String(str).trimRight();
}

// Return an uppercase copy of str
String toUpperCase(const String &str);

// Return a lowercase copy of str
String toLowerCase(const String &str);
// Return a copy of str, with first non-space letter changed to uppercase.
String firstLetterToUpperCase(const String &str);

String format1000(INT              n);
String format1000(UINT             n);
String format1000(LONG             n);
String format1000(ULONG            n);
String format1000(INT64            n);
String format1000(UINT64           n);
String formatSize(size_t           n);
String indentString(const String &s, UINT n);

ULONG         stringHash(    const String &s);                        // Very common used hashfunction
int           stringHashCmp( const String &s1, const String &s2);
ULONG         stringiHash(   const String &s);                        // Very common used hashfunction
int           stringiHashCmp(const String &s1, const String &s2);

// Same arguments as vprintf,vsprintf
inline String vformat(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  String result;
  result.vprintf(format, argptr);
  return result;
}

// Same arguments as printf,sprintf
String format(_In_z_ _Printf_format_string_ TCHAR const * const Format, ...);

template<typename T> class AbstractStringifier {
public:
  virtual String toString(const T &e) = 0;
};

class IntStringifier : public AbstractStringifier<int> {
public:
  String toString(const int &e);
};

class UIntStringifier : public AbstractStringifier<UINT> {
public:
  String toString(const UINT &e);
};

class Int64Stringifier : public AbstractStringifier<INT64> {
public:
  String toString(const INT64 &e);
};

class UInt64Stringifier : public AbstractStringifier<UINT64> {
public:
  String toString(const UINT64 &e);
};

class SizeTStringifier : public AbstractStringifier<size_t> {
public:
  String toString(const size_t &e);
};

class CharacterFormater : public AbstractStringifier<size_t> {
public:
  static CharacterFormater *stdAsciiFormater;
  static CharacterFormater *extendedAsciiFormater;
  static CharacterFormater *octalEscapedAsciiFormater;
  static CharacterFormater *hexEscapedAsciiFormater;
  static CharacterFormater *hexEscapedExtendedAsciiFormater;
};

TCHAR *strRemove(       TCHAR *s  , TCHAR ch);              // Remove any occurence of ch in s
TCHAR *strReplace(      TCHAR *s  , TCHAR from, TCHAR to);  // Substitute every occurence of from in s with to, return s.
TCHAR *strReplace(      TCHAR *dst, const TCHAR *src,       TCHAR  from, const TCHAR *to);
TCHAR *strReplace(      TCHAR *dst, const TCHAR *src, const TCHAR *from, const TCHAR *to);
TCHAR *strTrimRight(    TCHAR *s); // Remove any trailing white characters from s. Return s
TCHAR *strTrimLeft(     TCHAR *s); // Remove any leading  white characters from s. Return s
TCHAR *strTrim(         TCHAR *s); // Remove any leading and trailing white characters from s. Return s
TCHAR *strToLowerCase(  TCHAR *s);
TCHAR *strToUpperCase(  TCHAR *s);
TCHAR *strTabExpand(    TCHAR *dst, const TCHAR *src, int tabSize, TCHAR subst = _T(' '));
int    findMatchingpParanthes(const TCHAR *str, int pos);

String sprintbin(char    c);
String sprintbin(UCHAR   c);
String sprintbin(short   s);
String sprintbin(USHORT  s);
String sprintbin(int     i);
String sprintbin(UINT    i);
String sprintbin(long    l);
String sprintbin(ULONG   l);
String sprintbin(INT64   i);
String sprintbin(UINT64  i);

inline const TCHAR *boolToStr(bool b) {
  return b ? _T("true") : _T("false");
}

inline const TCHAR *boolToStr(BOOL b) {
  return b ? _T("true") : _T("false");
}

bool strToBool(const TCHAR *s);

int    strtabcmp(      const TCHAR *s1, const TCHAR *s2,           const TCHAR translateTable[256]);
int    strntabcmp(     const TCHAR *s1, const TCHAR *s2, size_t n, const TCHAR translateTable[256]);
TCHAR *streToUpperCase( TCHAR *s);
TCHAR *streToLowerCase( TCHAR *s);
int    streicmp(       const TCHAR *s1, const TCHAR *s2);
int    strneicmp(      const TCHAR *s1, const TCHAR *s2, size_t n);
int    streaicmp(      const TCHAR *s1, const TCHAR *s2);           // ignore case and accents
int    strneaicmp(     const TCHAR *s1, const TCHAR *s2, size_t n); // ignore case and accents

String loadString(int id);
String loadString(int id, const String &defaultValue);

int eIsalpha( int c);
int eIsupper( int c);
int eIslower( int c);
int eIsdigit( int c);
int eIsxdigit(int c);
int eIsspace( int c);
int eIspunct( int c);
int eIsalnum( int c);
int eIsprint( int c);
int eIsgraph( int c);
int eIscntrl( int c);
