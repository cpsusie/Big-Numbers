#pragma once

#include "StreamParameters.h"

namespace OStreamHelper {

using namespace std;

  constexpr char  s_decimalPointChar = '.';
  constexpr char *s_infStr           = "inf";
  constexpr char *s_qNaNStr          = "nan(ind)";
  constexpr char *s_sNaNStr          = "nan(snan)";

  // helper functions when formating various numbertypes
  template<typename StringType> void addDecimalPoint(StringType &dst) {
    dst += s_decimalPointChar;
  }

  inline char getExponentChar(FormatFlags flags) {
    return ((flags & std::ios::floatfield) == std::ios::hexfloat)
         ? ((flags & std::ios::uppercase) ? 'P' : 'p')
         : ((flags & std::ios::uppercase) ? 'E' : 'e');
  }

  template<typename StringType> void addExponentChar(StringType &dst, FormatFlags flags) {
    dst += getExponentChar(flags);
  }

  template<typename StringType> void addCharSeq(StringType &dst, TCHAR ch, StreamSize count) {
    dst.insert(dst.length(), (size_t)count, ch);
  }

  template<typename StringType> void addZeroes(StringType &dst, StreamSize count) {
    addCharSeq(dst, '0', count);
  }

  template<typename StringType> void removeTralingZeroes(StringType &s, bool removeDecimalPoint) {
    while(s.last() == '0') s.removeLast();
    if(removeDecimalPoint && (s.last() == s_decimalPointChar)) s.removeLast();
  }

  inline char *getHexPrefix(FormatFlags flags) {
    return (flags & std::ios::uppercase) ? "0X" : "0x";
  }

  template<typename StringType> void addHexPrefix(StringType &dst, FormatFlags flags) {
    dst += getHexPrefix(flags);
  }

  // Return dst
  template<typename StringType> StringType &formatFilledField(StringType &dst, const StringType &prefix, const String &str, const StreamParameters &param) {
    const intptr_t    fillCount = (intptr_t)param.width() - (intptr_t)str.length() - (intptr_t)prefix.length();
    const FormatFlags flags = param.flags();
    if(fillCount <= 0) {
      dst += prefix;
      dst += str.cstr();
    } else {
      switch(flags & ios::adjustfield) {
      case ios::left:
        dst += prefix;
        dst += str.cstr();
        addCharSeq(dst, param.fill(), fillCount);
        break;
      case ios::internal:
        dst += prefix;
        addCharSeq(dst, param.fill(), fillCount);
        dst += str.cstr();
        break;
      case ios::right: // every other combination is the same as right-adjustment
      default:
        addCharSeq(dst, param.fill(), fillCount);
        dst += prefix;
        dst += str.cstr();
        break;
      }
    }
    return dst;
  }

  // Return dst
  template<typename StringType> StringType &formatFilledNumericField(StringType &dst, const String &numStr, bool negative, const StreamParameters &param) {
    const FormatFlags flags = param.flags();
    StringType        prefixStr;
    if(negative) {
      prefixStr = "-";
    } else if((flags & ios::showpos) && (param.radix() == 10)) {
      prefixStr = "+";
    }
    if(flags & ios::showbase) {
      switch(flags & ios::basefield) {
      case ios::oct:
        return formatFilledField(dst, prefixStr, "0" + numStr, param);
      case ios::hex:
        addHexPrefix(prefixStr, flags);
        break;
      }
    }
    return formatFilledField(dst, prefixStr, numStr, param);
  }

  // Return pointer to first character in str in [0-9]. if not found, NULL is returned
  template<typename CharType> CharType *findFirstDigit(CharType *str) {
    for(; *str; str++) {
      if(iswdigit(*str)) return str;
    }
    return NULL;
  }

  // Assume str consists of characters [0-9]
  // Make 5-rounding of str to specified length. If wantedLength >= str.length(), nothing is changed.
  // If changes propagate all the way to first character, and this is rounded up from 5-9 -> 0, true is returned
  // to indicate carry. else false is returned
  // ex. str=="9994", wantedLength==3, will modify str to "999" , return false
  //     str=="9995", wantedLength==3, will modify str to "000" , return true
  //     str=="9946", wantedLength==3, will modify str to "995" , return false
  //     str=="9946", wantedLength==2, will modify str to "99"  , return false
  //     str=="9946", wantedLength==1, will modify str to "0"   , return true
  //     str=="8946", wantedLength==1, will modify str to "9"   , return false
  //     str=="4"   , wantedLength==0, will modify str to ""    , return false
  //     str=="5"   , wantedLength==0, will modify str to ""    , return true
  bool round5DigitString(String &str, intptr_t wantedLength);

  // format str, left,right,internal aligned, width filler character/inserted if getWidth() > str.length()
  // prefix containing sign and radix sequence (0x/X) is added
  // if flags == -1 (default), then stream.flags() will be used
  // Return dst
  template<typename StringType> StringType &formatFilledFloatField(StringType &dst, const String &str, bool negative, const StreamParameters &param) {
    const FormatFlags flags = param.flags();
    StringType        prefixStr;
    if(negative) {
      prefixStr = "-";
    } else if(flags & ios::showpos) {
      prefixStr = "+";
    }
    if((flags & ios::floatfield) == ios::hexfloat) {
      addHexPrefix(prefixStr, flags);
    }
    return formatFilledField(dst, prefixStr, str, param);
  }

  class FloatStringFields {
  public:
    String   m_ciphers;       // digits before comma
    String   m_decimals;      // digits after comma
    intptr_t m_expo10;        // base 10 exponent
    UINT     m_minExpoLength;
  protected:
    void init(TCHAR *str);
    // Adjust m_ciphers,m_decimals,m_expo10, so exactly 1 decimal digit (!= '0') is in m_ciphers and tail in m_decimals.
    // If any digits are moved between m_ciphers and m_decimals, m_expo10 is adjusted accordingly.
    void normalize();
    inline FloatStringFields(UINT minExpoLength = 3)
      : m_minExpoLength(minExpoLength)
      , m_expo10(0)
    {
    }
    // Return index of first character in s not equal to '0', -1 of none found
    static intptr_t findFirstNonZero(const String &s);

    // Remove leading zeroes ('0') from s if any.
    // Return the number of zeroes removed
    static size_t   trimLeadingZeroes(  String &s);

    // Remove trailing zeroes ('0') from s if any.
    // Return s
    static String  &trimTrailingZeroes( String &s);

    static size_t   countLeadingZeroes( const String &s);
    static size_t   countTrailingZeroes(const String &s);
  public:
    // Assume str contains a numeric field of decimal digits in the form
    // "[0-9]+(.[0-9]*)?(e[+-]?[0-9]+)?" or "[0-9]*.[0-9]+(e[+-]?[0-9]+)?"
    // If exponent part is not specified, an exponent of 0 is assumed
    inline FloatStringFields(TCHAR *str, UINT minExpoLength = 3)
      : m_minExpoLength(minExpoLength)
    {
      init(str);
    }
    inline String getAllDigits() const {
      return m_ciphers + m_decimals;
    }
    inline size_t getTotalDigitCount() const {
      return m_ciphers.length() + m_decimals.length();
    }
    // Return formatted exponent, without leading 'e'
    inline String getExponentStr() const {
#if defined(IS64BIT)
      return format(_T("%+0*lld"), m_minExpoLength+1, m_expo10);
#else // IS32BIT
      return format(_T("%+0*d"), m_minExpoLength+1, m_expo10);
#endif // IS64BIT
    }
    template<typename StringType> StringType &formatFixed(     StringType &dst, StreamSize prec, FormatFlags flags);
    template<typename StringType> StringType &formatScientific(StringType &dst, StreamSize prec, FormatFlags flags);
    template<typename StringType> StringType &formatFloat(     StringType &dst, StreamSize prec, FormatFlags flags);
  };

  // Return dst
  template<typename StringType> StringType &FloatStringFields::formatFixed(StringType &dst, StreamSize prec, FormatFlags flags) {
    const bool     defaultFormat      = (flags & ios::floatfield) == 0;
    const bool     forceDecimalPoint  = (flags & ios::showpoint ) != 0;
    const bool     trimTrailingZeroes = defaultFormat && !forceDecimalPoint;

    const intptr_t decimalsAfterComma = defaultFormat ? max(0, (intptr_t)prec - ((intptr_t)m_ciphers.length() - countLeadingZeroes(m_ciphers)))
                                                      : max(0, (intptr_t)prec);

    String     allDigits  = getAllDigits();
    intptr_t   digitCount = allDigits.length();
    intptr_t   commaPos   = m_ciphers.length() + m_expo10;

    if(commaPos <= digitCount) {
      if(commaPos <= 0) {
        allDigits.insert(0, 1-commaPos, '0');
        commaPos = 1;
      }
      if(round5DigitString(allDigits, commaPos + decimalsAfterComma)) {
        allDigits.insert(0, '1'); // carry propagated all the way up
        commaPos++;
      }
      dst = substr(allDigits, 0, commaPos);
      if(forceDecimalPoint || (decimalsAfterComma > 0)) {
        addDecimalPoint(dst);
        const intptr_t len1 = dst.length();
        if(decimalsAfterComma > 0) {
          dst += substr(allDigits, commaPos, decimalsAfterComma);
        }
        const intptr_t currentDecimalsAfterComma = (intptr_t)dst.length() - len1;
        if(trimTrailingZeroes) {
          removeTralingZeroes(dst, !forceDecimalPoint);
        } else if(decimalsAfterComma > currentDecimalsAfterComma) {
          addZeroes(dst, decimalsAfterComma - currentDecimalsAfterComma);
        }
      }
    } else { // commaPos > digitCount
      const intptr_t zeroCount = commaPos - digitCount;
      dst = allDigits;
      addZeroes(dst, zeroCount);
      if(forceDecimalPoint || (decimalsAfterComma > 0)) {
        addDecimalPoint(dst);
        if((decimalsAfterComma > 0) && !defaultFormat) {
          addZeroes(dst, decimalsAfterComma);
        }
      }
    }
    return dst;
  }

  // Return dst
  template<typename StringType> StringType &FloatStringFields::formatScientific(StringType &dst, StreamSize prec, FormatFlags flags) {
    const bool     defaultFormat      = (flags & ios::floatfield) == 0;
    const bool     forceDecimalPoint  = (flags & ios::showpoint ) != 0;
    const bool     trimTrailingZeroes = defaultFormat && !forceDecimalPoint;
    const intptr_t decimalsAfterComma = defaultFormat ? max(0, (intptr_t)prec - 1) : (intptr_t)prec;

    normalize();
    if(decimalsAfterComma < (intptr_t)m_decimals.length()) {
      if(round5DigitString(m_decimals, decimalsAfterComma)) {
        if(m_ciphers[0] < '9') {
          m_ciphers[0]++;
        } else {
          m_ciphers[0] = _T('1');
          m_expo10++;
        }
      }
    }
    if(trimTrailingZeroes) {
      removeTralingZeroes(m_decimals, false);
    } else {
      const intptr_t zeroCount = decimalsAfterComma - (intptr_t)m_decimals.length();
      if(zeroCount > 0) addZeroes(m_decimals, zeroCount);
    }
    dst = m_ciphers;

    if(forceDecimalPoint || (m_decimals.length() > 0)) {
      addDecimalPoint(dst);
      if(m_decimals.length() > 0) {
        dst += m_decimals;
      }
    }
    addExponentChar(dst, flags);
    dst += getExponentStr();
    return dst;
  }

  // Format floating point. (flags & ios::floatfield) must be in {0,ios:scientific,ios::fixed}
  // return dst
  template<typename StringType> StringType &FloatStringFields::formatFloat(StringType &dst, StreamSize prec, FormatFlags flags) {
    switch(flags & ios::floatfield) {
    case 0: // No float-format is specified. Format depends on e10 and precision
      if((flags & ios::showpoint) && (prec == 0)) prec = 6;
      if(prec == 0) {
        if((m_expo10 < -4) || (m_expo10 >= 6)) {
          formatScientific(dst, 6, flags);
        } else {
          formatFixed(dst, 6 - m_expo10, flags);
        }
      } else if((m_expo10 < -4) || (m_expo10 > 14) || ((m_expo10 > 0) && (m_expo10 >= prec)) || (m_expo10 > prec)) {
        formatScientific(dst, prec, flags);
      } else {
        formatFixed(dst, max(0, prec - m_expo10), flags);
      }
      break;
    case ios::scientific: // Use scientific format
      formatScientific(dst, prec ? prec : 6, flags);
      break;
    case ios::fixed: // Use fixed format
      formatFixed(dst, prec, flags);
      break;
    default:
      throwInvalidArgumentException(__TFUNCTION__, _T("flags.floatField must be 0,fixed,scientific"));
    }
    return dst;
  }

  // Return dst
  template<typename StringType> StringType &formatZero(StringType &dst, StreamSize prec, FormatFlags flags) {
    if((flags & ios::floatfield) == ios::hexfloat) {
      addZeroes(dst, 1);
      addDecimalPoint(dst);
      addZeroes(dst, prec?prec:6);
      addExponentChar(dst, flags);
      dst += "+0";
    } else {
      String tmp(_T("0.0e0"));
      FloatStringFields(tmp.cstr(), 2).formatFloat(dst, prec, flags);
    }
    return dst;
  }

  // Return dst
  template<typename CharType> CharType *formatpinf(CharType *dst, bool uppercase) {
    strCpy(dst, s_infStr);
    return uppercase ? strUpr(dst) : dst;
  }

  // Return dst
  template<typename CharType> CharType *formatninf(CharType *dst, bool uppercase) {
    *dst = '-';
    formatpinf(dst + 1, uppercase);
    return dst;
  }

  // Return dst
  template<typename CharType> CharType *formatqnan(CharType *dst, bool uppercase) {
    strCpy(dst, s_qNaNStr);
    return uppercase ? strUpr(dst) : dst;
  }

  // Return dst
  template<typename CharType> CharType *formatsnan(CharType *dst, bool uppercase) {
    strCpy(dst, s_sNaNStr);
    return uppercase ? strUpr(dst) : dst;
  }

  // fpclass is assumed to be one of _FPCLASS_PINF: _FPCLASS_NINF, _FPCLASS_SNAN : _FPCLASS_QNAN, return value from _fpclass(NumbeType)
  // Return dst
  template<typename CharType> CharType *formatUndefined(CharType *dst, int fpclass, bool uppercase = false, bool formatNinfAsPinf = false) {
    switch(fpclass) {
    case _FPCLASS_SNAN:  // signaling NaN
      return formatsnan(dst, uppercase);
    case _FPCLASS_QNAN:  // quiet NaN
      return formatqnan(dst, uppercase);
    case _FPCLASS_NINF:  // negative infinity
      if(!formatNinfAsPinf) {
        return formatninf(dst, uppercase);
      } // else Continue case
    case _FPCLASS_PINF:  // positive infinity
      return formatpinf(dst, uppercase);
    default:
      throwInvalidArgumentException(__TFUNCTION__, _T("fpclass=%08X"), fpclass);
      break;
    }
    return dst;
  }

  class TostringStream : public std::ostringstream {
  public:
    inline TostringStream(const StreamParameters &param) {
      *this << param;
    }
    inline TostringStream(StreamSize prec, StreamSize w, FormatFlags flg) {
      precision(prec);
      width(w);
      flags(flg);
    }
    inline TostringStream(StreamSize w, FormatFlags flg) {
      width(w);
      flags(flg);
    }
  };

  class TowstringStream : public std::wostringstream {
  public:
    inline TowstringStream(const StreamParameters &param) {
      *this << param;
    }
    inline TowstringStream(StreamSize prec, StreamSize w, FormatFlags flg) {
      precision(prec);
      width(w);
      flags(flg);
    }
    inline TowstringStream(StreamSize w, FormatFlags flg) {
      width(w);
      flags(flg);
    }
  };

}; // namespace OStreamHelper

// ----------------------------------------------------------------------------------------------------

namespace IStreamHelper {

  template<typename StreamType> String streamStateToString(StreamType &s) {
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

  template<typename IStreamType, typename CharType> class IStreamScanner {
  private:
    IStreamType       &m_in;
    const FormatFlags  m_flags;
    const CharType     m_fill;
    intptr_t           m_startIndex;
    String             m_buf;
    mutable String     m_tmp;
    CharType           m_ch;
#if defined(DEBUG_ISTREAMSCANNER)
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
      if(m_startIndex == 0) {
        return m_buf;
      } else {
        return m_tmp = substr(m_buf, m_startIndex, m_buf.length() - m_startIndex);
      }
    }
  };

  template<typename IStreamType> IStreamType &skipfill(IStreamType &in) {
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

  template<typename IStreamType> IStreamType &skipspace(IStreamType &in) {
    const FormatFlags flg = in.flags();
    in.flags(flg | ios::skipws);
    wchar_t ch;
    while(iswspace(ch = in.peek())) {
      in.get();
    }
    in.flags(flg);
    return in;
  }

}; // namespace IStreamHelper
