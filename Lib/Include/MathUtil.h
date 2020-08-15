#pragma once

#include <Math.h>
#include <RegexIStream.h>

inline bool   isChar(   char   v) { return true;              }
inline bool   isChar(   UCHAR  v) { return v <= CHAR_MAX;     }
inline bool   isChar(   short  v) { return v == (char)v;      }
inline bool   isChar(   USHORT v) { return v == (char)v;      }
inline bool   isChar(   int    v) { return v == (char)v;      }
inline bool   isChar(   UINT   v) { return v == (char)v;      }
inline bool   isChar(   long   v) { return v == (char)v;      }
inline bool   isChar(   ULONG  v) { return v == (char)v;      }
inline bool   isChar(   INT64  v) { return v == (char)v;      }
inline bool   isChar(   UINT64 v) { return v == (char)v;      }
inline bool   isChar(   float  v) { return isfinite(v) && (v == (char)v); }
inline bool   isChar(   double v) { return isfinite(v) && (v == (char)v); }

inline bool   isUchar(  char   v) { return v >= 0;            }
inline bool   isUchar(  UCHAR  v) { return true;              }
inline bool   isUchar(  short  v) { return v == (UCHAR)v;     }
inline bool   isUchar(  USHORT v) { return v == (UCHAR)v;     }
inline bool   isUchar(  int    v) { return v == (UCHAR)v;     }
inline bool   isUchar(  UINT   v) { return v == (UCHAR)v;     }
inline bool   isUchar(  long   v) { return v == (UCHAR)v;     }
inline bool   isUchar(  ULONG  v) { return v == (UCHAR)v;     }
inline bool   isUchar(  INT64  v) { return v == (UCHAR)v;     }
inline bool   isUchar(  UINT64 v) { return v == (UCHAR)v;     }
inline bool   isUchar(  float  v) { return isfinite(v) && (v == (UCHAR)v); }
inline bool   isUchar(  double v) { return isfinite(v) && (v == (UCHAR)v); }

inline bool   isShort(  short  v) { return true;              }
inline bool   isShort(  USHORT v) { return v <= SHRT_MAX;     }
inline bool   isShort(  int    v) { return v == (short)v;     }
inline bool   isShort(  UINT   v) { return v == (short)v;     }
inline bool   isShort(  long   v) { return v == (short)v;     }
inline bool   isShort(  ULONG  v) { return v == (short)v;     }
inline bool   isShort(  INT64  v) { return v == (short)v;     }
inline bool   isShort(  UINT64 v) { return v == (short)v;     }
inline bool   isShort(  float  v) { return isfinite(v) && (v == (short)v); }
inline bool   isShort(  double v) { return isfinite(v) && (v == (short)v); }

inline bool   isUshort( short  v) { return v >= 0;            }
inline bool   isUshort( USHORT v) { return true;              }
inline bool   isUshort( int    v) { return v == (USHORT)v;    }
inline bool   isUshort( UINT   v) { return v == (USHORT)v;    }
inline bool   isUshort( long   v) { return v == (USHORT)v;    }
inline bool   isUshort( ULONG  v) { return v == (USHORT)v;    }
inline bool   isUshort( INT64  v) { return v == (USHORT)v;    }
inline bool   isUshort( UINT64 v) { return v == (USHORT)v;    }
inline bool   isUshort( float  v) { return isfinite(v) && (v == (USHORT)v); }
inline bool   isUshort( double v) { return isfinite(v) && (v == (USHORT)v); }

inline bool   isInt(    int    v) { return true;              }
inline bool   isInt(    UINT   v) { return v <= INT_MAX;      }
inline bool   isInt(    long   v) { return true;              }
inline bool   isInt(    ULONG  v) { return v <= INT_MAX;      }
inline bool   isInt(    INT64  v) { return v == (int)v;       }
inline bool   isInt(    UINT64 v) { return v == (int)v;       }
inline bool   isInt(    float  v) { return isfinite(v) && (v == (int)v); }
inline bool   isInt(    double v) { return isfinite(v) && (v == (int)v); }

inline bool   isUint(   int    v) { return v >= 0;            }
inline bool   isUint(   UINT   v) { return true;              }
inline bool   isUint(   long   v) { return v >= 0;            }
inline bool   isUint(   ULONG  v) { return true;              }
inline bool   isUint(   INT64  v) { return v == (UINT)v;      }
inline bool   isUint(   UINT64 v) { return v == (UINT)v;      }
inline bool   isUint(   float  v) { return isfinite(v) && (v == (UINT)v); }
inline bool   isUint(   double v) { return isfinite(v) && (v == (UINT)v); }

inline bool   isInt64(  INT64  v) { return true;              }
inline bool   isInt64(  UINT64 v) { return v <= INT64_MAX;    }
inline bool   isInt64(  float  v) { return isfinite(v) && (v == (INT64)v);  }
inline bool   isInt64(  double v) { return isfinite(v) && (v == (INT64)v);  }

inline bool   isUint64( INT64  v) { return v >= 0;            }
inline bool   isUint64( UINT64 v) { return true;              }
inline bool   isUint64( float  v) { return isfinite(v) && (v == (UINT64)v); }
inline bool   isUint64( double v) { return isfinite(v) && (v == (UINT64)v); }

inline bool   isFloat(  float  v) { return true;              }
inline bool   isFloat(  double v) { return !isnormal(v) || (v == (float)v);  }


inline bool   isnan(    float  v) { return std::isnan(v);    }
inline bool   isnan(    double v) { return std::isnan(v);    }
inline bool   isinf(    float  v) { return std::isinf(v);    }
inline bool   isinf(    double v) { return std::isinf(v);    }

inline bool   isPInfinity(float  v) { return _fpclass(v) == _FPCLASS_PINF; }
inline bool   isPInfinity(double v) { return _fpclass(v) == _FPCLASS_PINF; }
inline bool   isNInfinity(float  v) { return _fpclass(v) == _FPCLASS_NINF; }
inline bool   isNInfinity(double v) { return _fpclass(v) == _FPCLASS_NINF; }

inline bool   isPow2(   int    v) { return (v & -v) == v;    }
inline bool   isPow2(   INT64  v) { return (v & -v) == v;    }

inline bool   isOdd(    int    v) { return (v & 1) != 0;     }
inline bool   isEven(   int    v) { return (v & 1) == 0;     }
inline bool   isOdd(    UINT   v) { return (v & 1) != 0;     }
inline bool   isEven(   UINT   v) { return (v & 1) == 0;     }
inline bool   isOdd(    long   v) { return (v & 1) != 0;     }
inline bool   isEven(   long   v) { return (v & 1) == 0;     }
inline bool   isOdd(    ULONG  v) { return (v & 1) != 0;     }
inline bool   isEven(   ULONG  v) { return (v & 1) == 0;     }
inline bool   isOdd(    INT64  v) { return (v & 1) != 0;     }
inline bool   isEven(   INT64  v) { return (v & 1) == 0;     }
inline bool   isOdd(    UINT64 v) { return (v & 1) != 0;     }
inline bool   isEven(   UINT64 v) { return (v & 1) == 0;     }

// 5-rounding
double round(   double v, int dec);
double trunc(   double v, int dec=0);
double fraction(double v);
float  fraction(float  v);

// dst must point to memory with at least 26 free char
char    *flttoa(char    *dst, float x);
// dst must point to memory with at least 26 free wchar_t
wchar_t *flttow(wchar_t *dst, float x);

// dst must point to memory with at least 26 free char
char    *dbltoa(char    *dst, double x);
// dst must point to memory with at least 26 free wchar_t
wchar_t *dbltow(wchar_t *dst, double x);

#if defined(_UNICODE)
#define flttot flttow
#define dbltot dbltow
#else
#define flttot flttoa
#define dbltot dbltoa
#endif

// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(int           v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }
// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(INT64         v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }
// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(float         v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }
// return (v > 0) ? +1 : (v < 0) ? -1 : 0
inline int sign(double        v) { return v < 0 ? -1 : v > 0 ? 1 : 0; }

// return v*v
inline UINT  sqr( int           v) { return v * v; }
// return v*v
inline float sqr( float         v) { return v * v; }
// return v*v
inline double sqr(double        v) { return v * v; }

extern const double M_PI;
extern const double M_E;

#define RAD2GRAD(r) ((r) / M_PI * 180.0)
#define GRAD2RAD(g) ((g) / 180.0 * M_PI)

#define PERCENT( n,total) ((total)?((double)(n)*100 /(total)):100.0 )
#define PERMILLE(n,total) ((total)?((double)(n)*1000/(total)):1000.0)

#if !defined(max)
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif
#if !defined(min)
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

// return x if x is in I = [min(x1, x2); max(x1, x2)] else the endpoint of I nearest to x
// assume MIN <= MAX
template<typename T> T minMax1(const T &x, const T &MIN, const T &MAX) {
  return (x < MIN) ? MIN : (x > MAX) ? MAX : x;
}

template<typename T> T minMax(T x, T x1, T x2) {
  return (x1 <= x2) ? minMax1(x, x1, x2) : minMax1(x, x2, x1);
}

template<typename T> T dsign(T x) {
  return (x < 0) ? -1 : (x > 0) ? 1 : 0;
}

template<typename T> T dmin(T x1, T x2) {
  return (x1 < x2) ? x1 : x2;
}

template<typename T> T dmax(T x1, T x2) {
  return (x1 > x2) ? x1 : x2;
}

// 23 fraction bits exlcusive leading 1-bit
inline UINT   getSignificandField(float  x) {
  return ((*(UINT  *)&x) & 0x7fffff);
}
// 24 bit significand inlcusive leading 1-bit
inline UINT   getSignificand(float  x) {
  return getSignificandField(x) | (1 << 23);
}

// 52 fraction bits exclusive leading 1-bit
inline UINT64 getSignificandField(double x) {
  return ((*(UINT64*)&x) & 0xfffffffffffffui64);
}
// 53 bit significand inlcusive leading 1-bit
inline UINT64 getSignificand(double  x) {
  return getSignificandField(x) | (1ui64 << 52);
}

// 8 bit exponent field
inline UINT   getExponent(   float  x) {
  return ((*(UINT  *)&x) >> 23) & 0xff;
}

// 11 bit exponent field
inline UINT   getExponent(   double x) {
  return ((*(UINT64*)&x) >> 52) & 0x7ff;
}

// real integer power of 2
inline int    getExpo2(      float  x) {
  return (int)getExponent(x) - 0x7f;
}

// real integer power of 2
inline int    getExpo2(      double x) {
  return (int)getExponent(x) - 0x3ff;
}

inline BYTE   getSign(       float  x) {
  return (((BYTE  *)&x)[3]) & 0x80;
}

inline BYTE   getSign(       double x) {
  return (((BYTE  *)&x)[7]) & 0x80;
}

class UndefNumericStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  UndefNumericStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static UndefNumericStreamScanner s_instance;
    return s_instance;
  }
};

class DecFloatValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  DecFloatValueStreamScanner() : RegexIStream(getRegexLines(), true) {
}
public:
  static const RegexIStream &getInstance() {
    static DecFloatValueStreamScanner s_instance;
    return s_instance;
  }
};

class HexFloatValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  HexFloatValueStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static HexFloatValueStreamScanner s_instance;
    return s_instance;
  }
};

class DecIntValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  DecIntValueStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static DecIntValueStreamScanner s_instance;
    return s_instance;
  }
};

class HexIntValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  HexIntValueStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static HexIntValueStreamScanner s_instance;
    return s_instance;
  }
};

class OctIntValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  OctIntValueStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static OctIntValueStreamScanner s_instance;
    return s_instance;
  }
};

class DecRationalValueStreamScanner : public RegexIStream {
private:
  static StringArray getRegexLines();
  DecRationalValueStreamScanner() : RegexIStream(getRegexLines(), true) {
  }
public:
  static const RegexIStream &getInstance() {
    static DecRationalValueStreamScanner s_instance;
    return s_instance;
  }
};

// radix must be 8,10, 16
const RegexIStream &getIntegerStreamScanner(int radix);

typedef enum {
   _UNDEFREG_PINF
  ,_UNDEFREG_NINF
  ,_UNDEFREG_SNAN
  ,_UNDEFREG_QNAN
} _UndefNumericValue;

template<typename IStreamType, typename CharType, typename NumericType> class NumericStreamAcceptUndef {
private:
  IStreamType &m_in;
  void parseOnFail(NumericType &x, bool neg) const {
    const _UndefNumericValue index = (_UndefNumericValue)UndefNumericStreamScanner::getInstance().match(m_in);
    switch(index) {
    case _UNDEFREG_PINF    :
      if(!neg) {
        x = numeric_limits<NumericType>::infinity();
        break;
      }
      // NB continue case
    case _UNDEFREG_NINF    :
      x = -numeric_limits<NumericType>::infinity();
      break;
    case _UNDEFREG_SNAN    :
      x =  numeric_limits<NumericType>::signaling_NaN();
      break;
    case _UNDEFREG_QNAN    :
      x =  numeric_limits<NumericType>::quiet_NaN();
      break;
    default                :
      m_in.setstate(ios_base::failbit);
      break;
    }
  }

public:
  inline NumericStreamAcceptUndef(IStreamType &in) : m_in(in) {
  }
  NumericStreamAcceptUndef &operator>>(NumericType &x) {
    if(m_in.good()) {
      CharType c = 0;
      while(iswspace(c = m_in.peek())) {
        m_in.get();
      }
      m_in >> x;
      if(m_in.fail()) {
        m_in.clear();
        parseOnFail(x, c=='-');
      }
    }
    return *this;
  }
};


template<typename IStreamType, typename CharType, typename NumericType> class NumericManipulatorStream {
public:
  mutable IStreamType *m_in;
  inline const NumericManipulatorStream &operator>>(NumericType &x) const {
    NumericStreamAcceptUndef<IStreamType, CharType, NumericType>(*m_in) >> x;
    return *this;
  }
  inline IStreamType &operator>>(const NumericManipulatorStream &) const {
    return *m_in;
  }
};

template<typename NumericType> NumericManipulatorStream<std::istream, char, NumericType> CharManip {
};

template<typename NumericType> NumericManipulatorStream<std::wistream, wchar_t, NumericType> WcharManip {
};

template<typename NumericType> NumericManipulatorStream<std::istream,char,NumericType> &operator>>(std::istream &in, NumericManipulatorStream<std::istream, char, NumericType> &dm) {
  dm.m_in = &in;
  return dm;
}

template<typename NumericType> NumericManipulatorStream<std::wistream, wchar_t, NumericType> &operator>>(std::wistream &in, NumericManipulatorStream<std::wistream, wchar_t, NumericType> &dm) {
  dm.m_in = &in;
  return dm;
}
