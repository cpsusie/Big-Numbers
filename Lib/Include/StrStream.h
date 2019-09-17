#pragma once

#include "StreamParameters.h"

// Used instead of standardclass strstream, which is slow!!! (at least in windows)
class StrStream : public StreamParameters, public String {
private:
  StrStream &appendFill(size_t count);
public:
  static constexpr TCHAR s_decimalPointChar = _T('.');
  static const     TCHAR *s_infStr;
  static const     TCHAR *s_qNaNStr;
  static const     TCHAR *s_sNaNStr;

  inline StrStream(StreamSize precision = 6, StreamSize width = 0, FormatFlags flags = 0) : StreamParameters(precision, width, flags) {
  }
  inline StrStream(const StreamParameters &param) : StreamParameters(param) {
  }
  inline StrStream(std::ostream &stream) : StreamParameters(stream) {
  }
  inline StrStream(std::wostream &stream) : StreamParameters(stream) {
  }

  inline void clear() {
    String::operator=(EMPTYSTRING);
  }

  inline TCHAR getLast() const {
    return last();
  }

  // helper functions when formating various numbertypes
  static inline void addDecimalPoint(String &s) {
    s += s_decimalPointChar;
  }
  static inline TCHAR getExponentChar(FormatFlags flags) {
    return ((flags & std::ios::floatfield) == std::ios::hexfloat)
         ? ((flags & std::ios::uppercase)  ? _T('P') : _T('p'))
         : ((flags & std::ios::uppercase)  ? _T('E') : _T('e'));
    }
  static inline void addExponentChar(String &s, FormatFlags flags) {
    s += getExponentChar(flags);
  }
  static inline void addCharSeq(String &s, TCHAR ch, size_t count) {
    s.insert(s.length(), count, ch);
  }
  static inline void addZeroes(String &s, size_t count) {
    addCharSeq(s, _T('0'), count);
  }
  static void removeTralingZeroDigits(String &s) {
    while(s.last() == '0') s.removeLast();
    if(s.last() == s_decimalPointChar) s.removeLast();
  }
  static inline TCHAR *getHexPrefix(FormatFlags flags) {
    return (flags & std::ios::uppercase) ? _T("0X") : _T("0x");
  }
  static inline void addHexPrefix(String &s, FormatFlags flags) {
    s += getHexPrefix(flags);
  }
  StrStream &formatFilledField(const String &prefix, const String &str, int flags = -1);
  StrStream &formatFilledNumericField(const String &str, bool negative, int flags = -1);

  // format str, left,right,internal aligned, width filler character/inserted if getWidth() > str.length()
  // prefix containing sign and radix sequence (0x/X) is added
  // if flags == -1 (default), then stream.flags() will be used
  // return *this
  StrStream &formatFilledFloatField(const String &str, bool negative, int flags=-1);

  static String &formatZero(String &result, StreamSize precision, FormatFlags flags, StreamSize maxPrecision = 0);
  static TCHAR *formatqnan(TCHAR *dst, bool uppercase = false);
  static TCHAR *formatsnan(TCHAR *dst, bool uppercase = false);
  static TCHAR *formatpinf(TCHAR *dst, bool uppercase = false);
  static TCHAR *formatninf(TCHAR *dst, bool uppercase = false);

  // fpclass is assumed to be one of _FPCLASS_PINF: _FPCLASS_NINF, _FPCLASS_SNAN : _FPCLASS_QNAN, return value from _fpclass(NumbeType)
  static TCHAR *formatUndefined(TCHAR *dst, int fpclass, bool uppercase = false, bool formatNinfAsPinf = false);

  TCHAR unputc();
  inline StrStream &append(const String &str) { // append str to stream without using any format-specifiers
    *this += str;
    return *this;
  }
  inline StrStream &append(const char *str) {
    *this += str;
    return *this;
  }
  inline StrStream &append(const wchar_t *str) {
    *this += str;
    return *this;
  }

  inline StrStream &operator<<(wchar_t ch) {
    return append(format(getCharFormat().cstr(), (TCHAR)ch));
  }
  inline StrStream &operator<<(char ch) {
    return append(format(getCharFormat().cstr(), (TCHAR)ch));
  }
  StrStream &operator<<(      BYTE ch) {
    return append(format(getCharFormat().cstr(), (_TUCHAR)ch));
  }
  inline StrStream &operator<<(const BYTE *str) {
    USES_ACONVERSION;
    return append(format(getStringFormat().cstr(), ASTR2TSTR((char*)str)));
  }
  inline StrStream &operator<<(const char *str) {
    USES_ACONVERSION;
    return append(format(getStringFormat().cstr(), ASTR2TSTR(str)));
  }
  inline StrStream &operator<<(const wchar_t *str) {
    USES_WCONVERSION;
    return append(format(getStringFormat().cstr(), WSTR2TSTR(str)));
  }
  inline StrStream &operator<<(const String &str) {
    return append(format(getStringFormat().cstr(), str.cstr()));
  }
  inline StrStream &operator<<(int n) {
    return append(format(getIntFormat().cstr(), n));
  }
  inline StrStream &operator<<(UINT n) {
    return append(format(getUIntFormat().cstr(), n));
  }
  inline StrStream &operator<<(long n) {
    return append(format(getLongFormat().cstr(), n));
  }
  inline StrStream &operator<<(ULONG n) {
    return append(format(getULongFormat().cstr(), n));
  }
  inline StrStream &operator<<(INT64 n) {
    return append(format(getInt64Format().cstr(), n));
  }
  inline StrStream &operator<<(UINT64 n) {
    return append(format(getUInt64Format().cstr(), n));
  }
  inline StrStream &operator<<(float f) {
    return append(format(getFloatFormat().cstr(), f));
  }
  inline StrStream &operator<<(double d) {
    return append(format(getDoubleFormat().cstr(), d));
  }
  inline StrStream &operator<<(const StrStream &s) {
    return append(s.cstr());
  }
  inline StrStream &operator<<(const StreamParameters &param) {
    ((StreamParameters&)(*this)) = param;
    return *this;
  }
};

// ----------------------------------------------------------------------------------------------------

template<class STREAM> String streamStateToString(STREAM &s) {
  String result;
  if(s) {
    result = _T("ok ");
  }
  if(s.fail()) result += _T("fail ");
  if(s.bad()) result += _T("bad ");
  if(s.eof()) {
    result += _T("eof ");
  } else {
    result += format(_T("next:'%c'"), s.peek());
  }
  return result;
}

//#define DEBUG_ISTREAMSCANNER

template<class IStreamType, class CharType> class IStreamScanner {
private:
  IStreamType       &m_in;
  const FormatFlags  m_flags;
  const CharType     m_fill;
  intptr_t           m_startIndex, m_fillStartIndex, m_fillLength;
  String             m_buf;
  mutable String     m_tmp;
  CharType           m_ch;
#ifdef DEBUG_ISTREAMSCANNER
  mutable String     m_stateString;
  void updateStateString() const {
    m_stateString = streamStateToString(m_in);
  }
#define UPDSTSTR() updateStateString();
#else
#define UPDSTSTR()
#endif // DEBUG_ISTREAMSCANNER

  // if m_in.eof(), return 0, else { int ch = m_in.peek(). return (CharType)ch); }
  CharType peekInput() const {
    if(m_in.eof()) {
      return 0;
    }
    const int ch = m_in.peek();
    UPDSTSTR()
    return (CharType)ch;
  }

  CharType skipleadingwhite() {
    if(iswspace(m_ch = peekInput())) {
      do {
        next();
      } while(iswspace(m_ch));
    }
    return m_ch;
  }
  const String &getBufferExlusiveWhiteAndFill() const {
    if(m_fillLength == 0) {
      if(m_startIndex == 0) {
        return m_buf;
      } else {
        return m_tmp = substr(m_buf, m_startIndex, m_buf.length() - m_startIndex);
      }
    }
    const intptr_t headLength = m_fillStartIndex - m_startIndex;
    if(headLength > 0) {
      m_tmp = substr(m_buf, m_startIndex, headLength);
    }
    const size_t lastIndex = m_fillStartIndex + m_fillLength;
    const size_t tailLength = m_buf.length() - lastIndex;
    if(tailLength) {
      m_tmp += substr(m_buf, lastIndex, tailLength);
    }
    return m_tmp;
  }
  void ungetAll() {
    for(intptr_t count = m_buf.length(); count--;) {
      m_in.unget();
      UPDSTSTR()
    }
    m_buf = EMPTYSTRING;
    m_ch = peekInput();
  }

public:
  IStreamScanner(IStreamType &in)
    : m_in(in)
    , m_flags(in.flags())
    , m_fillStartIndex(0)
    , m_fillLength(0)
    , m_fill(m_in.fill())
  {
    m_in.setf(0, std::ios::skipws);
    if(m_flags & std::ios::skipws) {
      m_ch = skipleadingwhite();
    } else {
      m_ch = peekInput();
    }
    m_startIndex = m_buf.length();
  }

  // if(!ok) all characters read, is unget, and m_in.setstate(ios::failbit)
  void endScan(bool ok = true) {
    UPDSTSTR()
    if(ok) {
      m_in.flags(m_flags);
    } else {
      if(!m_in.eof()) {
        ungetAll();
      }
      UPDSTSTR()
      m_in.flags(m_flags);
      m_in.setstate(std::ios::failbit);
      UPDSTSTR()
    }
  }

  inline FormatFlags flags() const {
    return m_flags;
  }
  inline int radix() const {
    return StreamParameters::radix(flags());
  }

  // Add current character to buf, and set current character = peekInput()
  CharType next() {
    m_buf += m_ch;
    m_in.get();
    UPDSTSTR()
    return m_ch = peekInput();
  }

  CharType skipInternalFill() {
    if(((m_flags & std::ios::adjustfield) == std::ios::internal) && !iswspace(m_fill) && (m_buf.length() > (size_t)m_startIndex)) {
      m_fillStartIndex = m_buf.length();
      while(peek() == m_fill) {
        next();
      }
      m_fillLength = m_buf.length() - m_fillStartIndex;
    }
    return peek();
  }

  CharType skipwhite() {
    while(iswspace(peek())) {
      next();
    }
    return peek();
  }

  inline CharType peek() const {
    return m_ch;
  }

  const String &getBuffer() const {
    return getBufferExlusiveWhiteAndFill();
  }
};

template<class STREAM> STREAM &setFormat(STREAM &stream, const StreamParameters &param) {
  stream.width(    param.width());
  stream.precision(param.precision());
  stream.flags(    param.flags());
  stream.fill((char)param.fill());
  return stream;
}

template<class STREAM> STREAM &skipfill(STREAM &in) {
  const wchar_t fillchar = in.fill();
  if(!iswspace(fillchar)) {
    const FormatFlags flg = in.flags();
    in.flags(flg | ios::skipws);
    wchar_t ch;
    while((ch = in.peek()) == fillchar) {
      in.get();
    }
    in.flags(flg);
  }
  return in;
}

template<class STREAM> STREAM &skipspace(STREAM &in) {
  const FormatFlags flg = in.flags();
  in.flags(flg | ios::skipws);
  wchar_t ch;
  while(iswspace(ch = in.peek())) {
    in.get();
  }
  in.flags(flg);
  return in;
}
