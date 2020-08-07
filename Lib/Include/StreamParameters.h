#pragma once

#include "Iterator.h"
#include "NumberInterval.h"

typedef int               FormatFlags; // std::_Iosb<int>::_Fmtflags
typedef std::streamsize   StreamSize;

typedef NumberInterval<StreamSize> StreamSizeInterval;
class StreamParametersIterator;

class StreamParameters {
private:
  StreamSize  m_width, m_prec;
  FormatFlags m_flags;
  TCHAR       m_fill;
public:
  StreamParameters(StreamSize precision=6, StreamSize width=0, FormatFlags flags = 0, TCHAR fill = ' ')
    : m_prec( precision)
    , m_width(width    )
    , m_flags(flags    )
    , m_fill (fill     )
  {
  }

  StreamParameters(const std::ostream  &stream)
    : m_prec (stream.precision())
    , m_width(stream.width()    )
    , m_flags(stream.flags()    )
    , m_fill (stream.fill()     )
  {
  }
  StreamParameters(const std::wostream &stream)
    : m_prec (stream.precision())
    , m_width(stream.width()    )
    , m_flags(stream.flags()    )
    , m_fill (stream.fill()     )
  {
  }

  inline void width(    StreamSize  w    ) { m_width = w;    }
  inline void precision(StreamSize  prec ) { m_prec  = prec; }
  inline void flags(    FormatFlags flg  ) { m_flags = flg;  }
  inline void fill(     TCHAR       ch   ) { m_fill  = ch;   }

  inline StreamSize  precision() const { return m_prec;      }
  inline StreamSize  width()     const { return m_width;     }
  inline FormatFlags flags()     const { return m_flags;     }
  inline TCHAR       fill()      const { return m_fill;      }
  static int         radix(FormatFlags flags) {
    switch(flags & std::ios::basefield) {
    case std::ios::oct:return 8;
    case std::ios::dec:return 10;
    case std::ios::hex:return 16;
    }
    return 10;
  }

  inline int         radix() const {
    return radix(m_flags);
  }

  static TCHAR *flagsToStr(TCHAR *dst, FormatFlags flags);
  static String flagsToString(FormatFlags flags);
  String toString() const;

#define SPECIAL_FORMATFLAGS      (std::ios::_Stdio|std::ios::boolalpha|std::ios::skipws|std::ios::unitbuf)
#define ITERATOR_INTFORMATMASK   (std::ios::_Fmtmask & ~(std::ios::floatfield | std::ios::showpoint | SPECIAL_FORMATFLAGS))
#define ITERATOR_FLOATFORMATMASK (std::ios::_Fmtmask & ~(std::ios::basefield  | std::ios::showbase  | SPECIAL_FORMATFLAGS))

#define ALLOWMANY_ADJUSTFIELDBITS 0x01
#define ALLOWMANY_BASEFIELDBITS   0x02
#define ALLOWMANY_FLOATFIELDBITS  0x04

  // Iterator will generate all possible combinations (set-product) width,precision,flags and filler, where width is contained in widthInterval,
  // precision is contained in precisionInterval, and (((flags|lowMask)&highMask) == flags) (assuming (BitSet)lowMask is a subset of (BitSet)highMask)
  // multibitFieldsFilter can be any combination of constants ALLOWMANY_* combined with |.
  // If a constant is present, the corresponding bit-field, f.ex. adjustfield will contain any combinations of 0 and 1-bits in this field
  // (if it passes the low/highMask-filter as well).
  // If not present, then at most 1 bit in this field is set in a flag-combination, which will reduce the number combinations
  static StreamParametersIterator getIterator(          const StreamSizeInterval &widthInterval, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags highMask, UINT multibitFieldsFilter, const TCHAR *fillers);
  static StreamParametersIterator getFloatParamIterator(             StreamSize   maxWidth     , const StreamSizeInterval &precisionInterval, FormatFlags lowMask = 0, FormatFlags highMask = ITERATOR_FLOATFORMATMASK, UINT multibitFieldsFilter = ALLOWMANY_FLOATFIELDBITS, const TCHAR *fillers = _T(" r"));
  static StreamParametersIterator getFloatParamIterator(             StreamSize   maxWidth     ,              StreamSize   maxPrecision     , FormatFlags lowMask = 0, FormatFlags highMask = ITERATOR_FLOATFORMATMASK, UINT multibitFieldsFilter = ALLOWMANY_FLOATFIELDBITS, const TCHAR *fillers = _T(" r"));

  static StreamParametersIterator getIntParamIterator(  const StreamSizeInterval &widthInterval,                                              FormatFlags lowMask = 0, FormatFlags highMask = ITERATOR_INTFORMATMASK  , UINT multibitFieldsFilter = 0                       , const TCHAR *fillers = _T(" r"));
  static StreamParametersIterator getIntParamIterator(               StreamSize   maxWidth     ,                                              FormatFlags lowMask = 0, FormatFlags highMask = ITERATOR_INTFORMATMASK  , UINT multibitFieldsFilter = 0                       , const TCHAR *fillers = _T(" r"));
};

template<typename StreamType, typename CharType> StreamType &setFormat(StreamType &s, const StreamParameters &p) {
  s.width(         p.width());
  s.precision(     p.precision());
  s.flags(         p.flags());
  s.fill((CharType)p.fill());
  return s;
}

inline std::ostream &operator<<(std::ostream &s, const StreamParameters &p) {
  return setFormat<std::ostream, char>(s, p);
}
inline std::istream &operator<<(std::istream &s, const StreamParameters &p) {
  return setFormat<std::istream, char>(s, p);
}

inline std::wostream &operator<<(std::wostream &s, const StreamParameters &p) {
  return setFormat<std::wostream, wchar_t>(s, p);
}
inline std::wistream &operator<<(std::wistream &s, const StreamParameters &p) {
  return setFormat<std::wistream, wchar_t>(s, p);
}

#define iparam(width) StreamParameters(0   ,  width  , std::ios::fixed      | std::ios::unitbuf)
#define dparam( prec) StreamParameters(prec, (prec)+8, std::ios::scientific | std::ios::showpos | std::ios::unitbuf)
#define udparam(prec) StreamParameters(prec, (prec)+7, std::ios::scientific | std::ios::unitbuf)
#define fparam( prec) StreamParameters(prec, (prec)+2, std::ios::fixed      | std::ios::showpos | std::ios::unitbuf)
#define ufparam(prec) StreamParameters(prec, (prec)+2, std::ios::fixed      | std::ios::unitbuf)

class StreamParametersIterator : public Iterator<StreamParameters> {
public:
  StreamParametersIterator(const StreamSizeInterval &widthInterval, const StreamSizeInterval &precisionInterval, FormatFlags lowMask, FormatFlags higmMask, UINT multibitFieldsFilter, const TCHAR *fillers);
  void reset(); // start all over
  size_t getMaxIterationCount() const;
  void dumpAllFormats() const;
};
