#pragma once

typedef int FormatFlags; // std::_Iosb<int>::_Fmtflags
typedef std::streamsize            StreamSize;

class StreamParameters {
private:
  StreamSize  m_precision, m_width;
  FormatFlags m_flags;
  TCHAR       m_filler;
  TCHAR  *addModifier(      TCHAR *dst)                     const;
  TCHAR  *addWidth(         TCHAR *dst)                     const;
  TCHAR  *addPrecision(     TCHAR *dst)                     const;
  TCHAR  *addPrefix(        TCHAR *dst, bool withPrecision) const;
  TCHAR  *addIntSpecifier(  TCHAR *dst, bool isSigned)      const;
  TCHAR  *addFloatSpecifier(TCHAR *dst)                     const;
public:
  StreamParameters(StreamSize precision=6, StreamSize width=0, FormatFlags flags = 0, TCHAR filler = _T(' '));

  StreamParameters(const std::ostream  &stream);
  StreamParameters(const std::wostream &stream);
  friend std::ostream  &operator<<(std::ostream  &out, const StreamParameters &p);
  friend std::wostream &operator<<(std::wostream &out, const StreamParameters &p);

  inline void setPrecision(StreamSize  precision) { m_precision = precision; }
  inline void setWidth(    StreamSize  width    ) { m_width     = width;     }
  inline void setFlags(    FormatFlags flags    ) { m_flags     = flags;     }
  inline void setFiller(   TCHAR       filler   ) { m_filler    = filler;    }

  inline StreamSize  getPrecision() const { return m_precision;      }
  inline StreamSize  getWidth()     const { return m_width;          }
  inline FormatFlags getFlags()     const { return m_flags;          }
  inline TCHAR       getFiller()    const { return m_filler;         }
  static int         getRadix(FormatFlags flags);
  inline int         getRadix() const {
    return getRadix(m_flags);
  }

  String getStringFormat() const;
  String getCharFormat()   const;
  String getUCharFormat()  const;
  String getShortFormat()  const;
  String getUShortFormat() const;
  String getIntFormat()    const;
  String getUIntFormat()   const;
  String getLongFormat()   const;
  String getULongFormat()  const;
  String getInt64Format()  const;
  String getUInt64Format() const;
  String getFloatFormat()  const;
  String getDoubleFormat() const;
};

#define iparam(width) StreamParameters(0   ,  width  , std::ios::fixed      | std::ios::unitbuf)
#define dparam( prec) StreamParameters(prec, (prec)+8, std::ios::scientific | std::ios::showpos | std::ios::unitbuf)
#define udparam(prec) StreamParameters(prec, (prec)+7, std::ios::scientific | std::ios::unitbuf)
#define fparam( prec) StreamParameters(prec, (prec)+2, std::ios::fixed      | std::ios::showpos | std::ios::unitbuf)
#define ufparam(prec) StreamParameters(prec, (prec)+2, std::ios::fixed      | std::ios::unitbuf)
