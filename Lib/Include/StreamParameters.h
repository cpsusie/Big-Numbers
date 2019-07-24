#pragma once

using namespace std;

class StreamParameters {
private:
  streamsize m_precision, m_width;
  int        m_flags;
  TCHAR      m_filler;
  TCHAR  *addModifier(      TCHAR *dst)                     const;
  TCHAR  *addWidth(         TCHAR *dst)                     const;
  TCHAR  *addPrecision(     TCHAR *dst)                     const;
  TCHAR  *addPrefix(        TCHAR *dst, bool withPrecision) const;
  TCHAR  *addIntSpecifier(  TCHAR *dst, bool isSigned)      const;
  TCHAR  *addFloatSpecifier(TCHAR *dst)                     const;
public:
  StreamParameters(streamsize precision=6, streamsize width=0, int flags=0, TCHAR filler = _T(' '));

  StreamParameters(const ostream  &stream);
  StreamParameters(const wostream &stream);
  friend ostream  &operator<<(ostream  &out, const StreamParameters &p);
  friend wostream &operator<<(wostream &out, const StreamParameters &p);

  inline void setPrecision(streamsize precision) { m_precision = precision; }
  inline void setWidth(    streamsize width    ) { m_width     = width;     }
  inline void setFlags(    int      flags      ) { m_flags     = flags;     }
  inline void setFiller(   TCHAR    filler     ) { m_filler    = filler;    }

  inline streamsize getPrecision() const         { return m_precision;      }
  inline streamsize getWidth()     const         { return m_width;          }
  inline int        getFlags()     const         { return m_flags;          }
  inline TCHAR      getFiller()    const         { return m_filler;         }
  static int getRadix(int flags);
  inline int        getRadix() const {
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
