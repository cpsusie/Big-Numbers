#pragma once

#include <stdarg.h>
#include "Unicode.h"
#include "PragmaLib.h"

#define __TFUNCTION__               _T(__FUNCTION__)
#define DEFINEMETHODNAME            static TCHAR const* const method = __TFUNCTION__
#define DECLARECLASSNAME            static TCHAR const* const s_className
#define DEFINECLASSNAME(className)  TCHAR const* const className::s_className = _T(#className)

class String {
private:
  size_t m_len, m_capacity;
  TCHAR *m_buf;
  static TCHAR *newCharBuffer(const TCHAR *s, size_t &length, size_t &capacity);
  void indexError(size_t index) const;
public:
  String();
  String(const String &s);
  String(const TCHAR  *s);
#ifdef UNICODE
  String(            const char *s);
  String &operator=( const char *s);
  String &operator+=(const char *rhs);
#endif
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
  String      &operator=(const String &rhs);
  String      &operator=(const TCHAR *s);
  TCHAR       &operator[](size_t index);                    // Returns TCHAR at position index
  const TCHAR &operator[](size_t index) const;              // Returns TCHAR at position index
  inline TCHAR        last() const { return m_len ? m_buf[m_len-1] : 0; }
  inline TCHAR       *cstr()       { return m_buf; }
  inline const TCHAR *cstr() const { return m_buf; }
  String &remove(size_t pos, size_t count = 1);                      // Remove characters at position pos, pos+1, ...pos+count-1
  String &removeLast();                                              // Remove the last character if any.
  String &insert(size_t pos, TCHAR ch);                              // Insert ch into String at position pos
  String &insert(size_t pos, const String &s);                       // Insert s into String at position pos

  inline int compare(const String &s) const {
    return _tcscmp(m_buf, s.m_buf);
  }
  inline int compareIgnoreCase(const String &s) const {
    return _tcsicmp(m_buf, s.m_buf);
  }
  inline bool equalsIgnoreCase(const String &s) const {
    return compareIgnoreCase(s) == 0;
  }

  friend String  operator+ ( const String &lhs, const String &rhs);
         String &operator+=( const String &rhs);                  // Append rhs to this
         String &operator+=( const TCHAR  *rhs);                  // Append rhs to this
         String &operator+=(       TCHAR   ch );
  String &replace(  TCHAR         from, TCHAR         to);        // Substitute every occurrence of from in s with to. Return this
  String &replace(  TCHAR         from, const TCHAR  *to);        // Substitute every occurrence of from in s with to. Return this
  String &replace(  const TCHAR  *from, TCHAR         to);
  String &replace(  const TCHAR  *from, const TCHAR  *to);        // Substitute every occurrence of from in s with to. Return this
  String &replace(  TCHAR         from, const String &to);
  String &replace(  const String &from, TCHAR         to);
  String &replace(  const String &from, const String &to);

  String &trim();                                                           // Remove leading and trailing spaces. Return this
  String &trimLeft();                                                       // Remove leading spaces. Return this
  String &trimRight();                                                      // Remove trailing spaces. Return this
  intptr_t     find(const TCHAR  *str, size_t from = 0) const;              // Return index of first occurrence of str starting at position from, -1 if not found
  intptr_t     find(const String &str, size_t from = 0) const;
  intptr_t     find(TCHAR         ch , size_t from = 0) const;              // Return index of first occurrence of ch starting at position from, -1 if not found
  intptr_t    rfind(TCHAR         ch                  ) const;              // Return index of last occurrence of ch, -1 if not found

  inline size_t length()  const { return m_len; }                           // Return length og String
  inline bool   isEmpty() const { return m_len == 0; }
  friend String left(  const String &str, intptr_t length);                 // Return substring "s[0]s[1]...s[len-1]".                          ex. left("abc"  ,2)   = "ab"
  friend String right( const String &str, intptr_t length);                 // Return substring s[s.len-length],s[s.len-length+1]...s[s.len-1]. ex. right("abc" ,2)   = "bc"
  friend String substr(const String &str, intptr_t from, intptr_t length);  // Return substring "str[from]str[from+1]...str[from+length-1]"     ex. substr(abcd",1,2) = "bc"
  friend String rev(   const String &str);                                  // Return reverse String
  friend String spaceString(std::streamsize length, TCHAR ch = _T(' '));    // Return String with length length, filled with ch. return "" if length <= 0

  friend tostream &operator<<(tostream &f, const String &str);
  friend tistream &operator>>(tistream &f, String &str);
  String &vprintf(const TCHAR *format, va_list argptr);                     // Same as vsprintf. Return *this
  String &printf( _In_z_ _Printf_format_string_ const TCHAR *format, ...);  // Same as sprintf. Return *this

  const String &toString() const {
    return *this;
  }

  ULONG hashCode() const;

  // works only for characters [0..255] 
  static const TCHAR  upperCaseTranslate[256];
  static const TCHAR *lowerCaseTranslate;
  static const TCHAR  upperCaseAccentTranslate[256];
};

inline bool operator==(const String &lhs, const String &rhs) {
  return lhs.compare(rhs) == 0;
}
inline bool operator==(const String &lhs, const TCHAR  *rhs) {
  return _tcscmp(lhs.cstr(), rhs) == 0;
}
inline bool operator==(const TCHAR  *lhs, const String &rhs) {
  return _tcscmp(lhs, rhs.cstr()) == 0;
}

inline bool operator!=(const String &lhs, const String &rhs) {
  return lhs.compare(rhs) != 0;
}
inline bool operator!=(const String &lhs, const TCHAR  *rhs) {
  return _tcscmp(lhs.cstr(), rhs) != 0;
}
inline bool operator!=(const TCHAR  *lhs, const String &rhs) {
  return _tcscmp(lhs, rhs.cstr()) != 0;
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

String trim(       const String &str);                         // Return copy of str, without leading and trailing spaces
String trimLeft(   const String &str);                         // Return copy og str, without leading spaces
String trimRight(  const String &str);                         // Return copy of str, without trailing spaces
String toUpperCase(const String &str);                         // Return an uppercase copy of str
String toLowerCase(const String &str);                         // Return a lowercase copy of str
String firstLetterToUpperCase(const String &str);              // Return a copy of str, with first non-space letter changed to uppercase.

String format1000(INT              n);
String format1000(UINT             n);
String format1000(LONG             n);
String format1000(ULONG            n);
String format1000(INT64            n);
String format1000(UINT64           n);
String formatSize(size_t           n);
String indentString(const String &s, UINT n);

ULONG         stringHash(    const String &str);                        // Very common used hashfunction
int           stringHashCmp( const String &key, const String &elem);
ULONG         stringiHash(   const String &str);                        // Very common used hashfunction
int           stringiHashCmp(const String &key, const String &elem);
String vformat(const TCHAR *format, va_list argptr);                    // Same arguments as vprintf,vsprintf
String format( _In_z_ _Printf_format_string_ const TCHAR *format, ...); // Same arguments as printf,sprintf

String toString(char             ch, int width = 0, int flags = 0  );
String toString(SHORT            n , int precision = 0, int width = 0, int flags = 0);
String toString(USHORT           n , int precision = 0, int width = 0, int flags = 0);
String toString(INT              n , int precision = 0, int width = 0, int flags = 0);
String toString(UINT             n , int precision = 0, int width = 0, int flags = 0);
String toString(LONG             n , int precision = 0, int width = 0, int flags = 0);
String toString(ULONG            n , int precision = 0, int width = 0, int flags = 0);
String toString(INT64            n , int precision = 0, int width = 0, int flags = 0);
String toString(UINT64           n , int precision = 0, int width = 0, int flags = 0);
String toString(float            x , int precision = 6, int width = 0, int flags = 0);
String toString(double           x , int precision = 6, int width = 0, int flags = 0);

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


int    strtabcmp(      const TCHAR *s1, const TCHAR *s2,           const TCHAR translateTable[256]);
int    strntabcmp(     const TCHAR *s1, const TCHAR *s2, size_t n, const TCHAR translateTable[256]);
TCHAR *streToUpperCase( TCHAR *s);
TCHAR *streToLowerCase( TCHAR *s);
int    streicmp(       const TCHAR *s1, const TCHAR *s2);
int    strneicmp(      const TCHAR *s1, const TCHAR *s2, size_t n);
int    streaicmp(      const TCHAR *s1, const TCHAR *s2);        // ignore case and accents
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
