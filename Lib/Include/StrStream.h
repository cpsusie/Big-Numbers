#pragma once

#include "StreamParameters.h"

namespace OStreamHelper {

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

  template<typename StringType> void addCharSeq(StringType &dst, TCHAR ch, size_t count) {
    dst.insert(dst.length(), count, ch);
  }

  template<typename StringType> void addZeroes(StringType &dst, size_t count) {
    addCharSeq(dst, '0', count);
  }

  template<typename StringType> void removeTralingZeroDigits(StringType &s) {
    while(s.last() == '0') s.removeLast();
    if(s.last() == s_decimalPointChar) s.removeLast();
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
  template<typename StringType> StringType &formatFilledNumericField(StringType &dst, const String &str, bool negative, const StreamParameters &param) {
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
        return formatFilledField(dst, prefixStr, "0" + str, param);
      case ios::hex:
        addHexPrefix(prefixStr, flags);
        break;
      }
    }
    return formatFilledField(dst, prefixStr, str, param);
  }

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

  // Return dst
  template<typename StringType> StringType &formatZero(StringType &dst, StreamSize precision, FormatFlags flags, StreamSize maxPrecision = 0) {
    switch(flags & ios::floatfield) {
    case 0:
      dst += '0';
      if(flags & ios::showpoint) {
        addDecimalPoint(dst);
        precision = max(precision, 1);
        addZeroes(dst, (size_t)precision);
      }
      break;
    case ios::scientific:
      dst += '0';
      if((flags & ios::showpoint) || (precision > 0)) {
        addDecimalPoint(dst);
        if(precision > 0) {
          addZeroes(dst, (size_t)((maxPrecision <= 0) ? precision : min(precision, maxPrecision)));
        }
      }
      addExponentChar(dst, flags);
      dst += "+00";
      break;
    case ios::fixed:
      dst += _T('0');
      if((flags & ios::showpoint) || (precision > 0)) {
        addDecimalPoint(dst);
        if(precision > 0) {
          addZeroes(dst, (size_t)precision);
        }
      }
      break;
    case ios::hexfloat:
      dst += '0';
      if(precision == 0) {
        precision = 6;
      }
      addDecimalPoint(dst);
      addZeroes(dst, (size_t)precision);
      addExponentChar(dst, flags);
      dst += "+0";
      break;
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
