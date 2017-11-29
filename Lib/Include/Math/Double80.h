#pragma once

#include <MyUtil.h>
#include <StrStream.h>
#include <Random.h>
#include "PragmaLib.h"

#pragma check_stack(off)

typedef enum {
  FPU_LOW_PRECISION            // 32-bits floatingpoint
 ,FPU_NORMAL_PRECISION         // 64-bits floatingpoint
 ,FPU_HIGH_PRECISION           // 80-bits floatingpoint
} FPUPrecisionMode;

typedef enum {
  FPU_ROUNDCONTROL_ROUND       // Round to nearest, or to even if equidistant
 ,FPU_ROUNDCONTROL_ROUNDDOWN   // Round down (toward -infinity)
 ,FPU_ROUNDCONTROL_ROUNDUP     // Round up (toward +infinity)
 ,FPU_ROUNDCONTROL_TRUNCATE    // Truncate (toward 0)
} FPURoundMode;

#define FPU_INVALID_OPERATION_EXCEPTION 0x01
#define FPU_DENORMALIZED_EXCEPTION      0x02
#define FPU_DIVIDE_BY_ZERO_EXCEPTION    0x04
#define FPU_OVERFLOW_EXCEPTION          0x08
#define FPU_UNDERFLOW_EXCEPTION         0x10
#define FPU_PRECISION_EXCEPTION         0x20

#define FPU_ALL_EXCEPTIONS              0x3f

#ifdef IS64BIT
extern "C" {
void FPUinit();
void FPUgetStatusWord(WORD &dst);
void FPUgetControlWord(WORD &dst);
void FPUsetControlWord(const WORD &flags);
void FPUgetTagsWord(WORD *buffer); // WORD[14]
void FPUclearExceptions();
};
#endif

class FPU {
private:
  FPU() {} // Cannot be instatiated
public:
#ifdef IS32BIT
  static inline void      init() {
    __asm {
      fninit
    }
  }
  static inline USHORT    getStatusWord() {
    USHORT sw;
    __asm {
      fstsw sw
    }
    return sw;
  }
  static inline USHORT    getControlWord() {
    USHORT cw;
    __asm {
      fnstcw cw
    }
    return cw;
  }
  static inline void      setControlWord(USHORT flags) {
    __asm {
      fldcw flags
    }
  }
  static inline USHORT    getTagsWord() {
    USHORT buffer[14];
    __asm {
      fstenv buffer
    }
    return buffer[4];
  }
  static inline void      clearExceptions() {
    __asm {
      fclex
    }
  }

#else
  static inline void      init() {
    FPUinit();
  }
  static inline USHORT    getStatusWord() {
    WORD tmp;
    FPUgetStatusWord(tmp);
    return tmp;
  }
  static inline USHORT    getControlWord() {
    WORD tmp;
    FPUgetControlWord(tmp);
    return tmp;
  }
  static inline void      setControlWord(USHORT flags) {
    FPUsetControlWord(flags);
  }
  static inline USHORT    getTagsWord() {
    WORD buffer[14];
    FPUgetTagsWord(buffer);
    return buffer[4];
  }
  static inline void      clearExceptions() {
    FPUclearExceptions();
  }
#endif // IS32BIT

  static inline void      clearStatusWord() {
    const USHORT cw = getControlWord();
    init();
    setControlWord(cw);
  }
  // returns current FPU controlwoed
  static USHORT           setPrecisionMode(FPUPrecisionMode p);
  static FPUPrecisionMode getPrecisionMode();
  // returns current FPU controlword
  static USHORT           setRoundMode(FPURoundMode mode);
  static FPURoundMode     getRoundMode();
  static inline void      restoreControlWord(USHORT ctrlWord) {
    setControlWord(ctrlWord);
  }
  static inline int       getStackHeight() {
    const int TOP = ((getStatusWord() >> 11) & 7);
    return (TOP != 0) ? (8 - TOP) : (getTagsWord() == 0xffff) ? 0 : 8;
  }
  static inline bool      stackOverflow() {
    return (getStatusWord() & 0x240) == 0x240;
  }
  static inline bool      stackUnderflow() {
    return (getStatusWord() & 0x240) == 0x040;
  }
  static inline bool      stackFault() {
    return (getStatusWord() & 0x40) == 0x40;
  }
  static void             enableExceptions(bool enable, USHORT flags);
};

class Double80;

#ifdef IS64BIT
extern "C" {
void   D80FromI16(        Double80 &s  , const short    &x  );
void   D80FromUI16(       Double80 &s  , USHORT          x  );
void   D80FromI32(        Double80 &s  , const int      &x  );
void   D80FromUI32(       Double80 &s  , UINT            x  );
void   D80FromI64(        Double80 &s  , const INT64    &x  );
void   D80FromUI64(       Double80 &s  , UINT64          x  );
void   D80FromFlt(        Double80 &s  , const float    &x  );
void   D80FromDbl(        Double80 &s  , const double   &x  );
int    D80ToI32(          const Double80 &x);
UINT   D80ToUI32(         const Double80 &x);
INT64  D80ToI64(          const Double80 &x);
UINT64 D80ToUI64(         const Double80 &x);
float  D80ToFlt(          const Double80 &x);
double D80ToDbl(          const Double80 &x);
int    D80cmpI16(         const Double80 &x, const short    &y);
int    D80cmpUI16(        const Double80 &x, USHORT          y);
int    D80cmpI32(         const Double80 &x, const int      &y);
int    D80cmpUI32(        const Double80 &x, UINT            y);
int    D80cmpI64(         const Double80 &x, const INT64    &y);
int    D80cmpUI64(        const Double80 &x, UINT64          y);
int    D80cmpFlt(         const Double80 &x, const float    &y);
int    D80cmpDbl(         const Double80 &x, const double   &y);
int    D80cmpD80(         const Double80 &x, const Double80 &y);
int    D80isZero(         const Double80 &x);
void   D80addI16(         Double80 &dst, const short    &x);
void   D80subI16(         Double80 &dst, const short    &x);
void   D80subrI16(        Double80 &dst, const short    &x);
void   D80mulI16(         Double80 &dst, const short    &x);
void   D80divI16(         Double80 &dst, const short    &x);
void   D80divrI16(        Double80 &dst, const short    &x);
void   D80addUI16(        Double80 &dst, USHORT          x);
void   D80subUI16(        Double80 &dst, USHORT          x);
void   D80subrUI16(       Double80 &dst, USHORT          x);
void   D80mulUI16(        Double80 &dst, USHORT          x);
void   D80divUI16(        Double80 &dst, USHORT          x);
void   D80divrUI16(       Double80 &dst, USHORT          x);
void   D80addI32(         Double80 &dst, const int      &x);
void   D80subI32(         Double80 &dst, const int      &x);
void   D80subrI32(        Double80 &dst, const int      &x);
void   D80mulI32(         Double80 &dst, const int      &x);
void   D80divI32(         Double80 &dst, const int      &x);
void   D80divrI32(        Double80 &dst, const int      &x);
void   D80addUI32(        Double80 &dst, UINT            x);
void   D80subUI32(        Double80 &dst, UINT            x);
void   D80subrUI32(       Double80 &dst, UINT            x);
void   D80mulUI32(        Double80 &dst, UINT            x);
void   D80divUI32(        Double80 &dst, UINT            x);
void   D80divrUI32(       Double80 &dst, UINT            x);
void   D80addI64(         Double80 &dst, const INT64    &x);
void   D80subI64(         Double80 &dst, const INT64    &x);
void   D80subrI64(        Double80 &dst, const INT64    &x);
void   D80mulI64(         Double80 &dst, const INT64    &x);
void   D80divI64(         Double80 &dst, const INT64    &x);
void   D80divrI64(        Double80 &dst, const INT64    &x);
void   D80addUI64(        Double80 &dst, UINT64          x);
void   D80subUI64(        Double80 &dst, UINT64          x);
void   D80subrUI64(       Double80 &dst, UINT64          x);
void   D80mulUI64(        Double80 &dst, UINT64          x);
void   D80divUI64(        Double80 &dst, UINT64          x);
void   D80divrUI64(       Double80 &dst, UINT64          x);
void   D80addFlt(         Double80 &dst, const float    &x);
void   D80subFlt(         Double80 &dst, const float    &x);
void   D80subrFlt(        Double80 &dst, const float    &x);
void   D80mulFlt(         Double80 &dst, const float    &x);
void   D80divFlt(         Double80 &dst, const float    &x);
void   D80divrFlt(        Double80 &dst, const float    &x);
void   D80addDbl(         Double80 &dst, const double   &x);
void   D80subDbl(         Double80 &dst, const double   &x);
void   D80subrDbl(        Double80 &dst, const double   &x);
void   D80mulDbl(         Double80 &dst, const double   &x);
void   D80divDbl(         Double80 &dst, const double   &x);
void   D80divrDbl(        Double80 &dst, const double   &x);
void   D80addD80(         Double80 &dst, const Double80 &x);
void   D80subD80(         Double80 &dst, const Double80 &x);
void   D80mulD80(         Double80 &dst, const Double80 &x);
void   D80divD80(         Double80 &dst, const Double80 &x);
void   D80rem(            Double80 &dst, const Double80 &x);
void   D80neg(            Double80 &x);
void   D80inc(            Double80 &x);
void   D80dec(            Double80 &x);
int    D80getExpo2(       const Double80 &x);
int    D80getExpo10(      const Double80 &x);
void   D80fabs(           Double80 &x);
void   D80sqr(            Double80 &x);
void   D80sqrt(           Double80 &x);
void   D80sin(            Double80 &x);
void   D80cos(            Double80 &x);
void   D80tan(            Double80 &x);
void   D80atan(           Double80 &x);
void   D80atan2(          Double80 &y, const Double80 &x);
// inout is c, out s
void   D80sincos(         Double80 &c, Double80       &s);
void   D80exp(            Double80 &x);
void   D80exp10(          Double80 &x);
void   D80exp2(           Double80 &x);
void   D80log(            Double80 &x);
void   D80log10(          Double80 &x);
void   D80log2(           Double80 &x);
// x = pow(x,y)
void   D80pow(            Double80 &x, const Double80 &y);
void   D80floor(          Double80 &x);
void   D80ceil(           Double80 &x);
void   D80ToBCD(BYTE bcd[10], const Double80 &src);
void   D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);
}
#endif // IS64BIT

#ifdef IS32BIT
extern const double   _Dmaxi16P1;
extern const double   _Dmaxi32P1;
extern const Double80 _D80maxi64P1;
#endif // IS32BIT

class Double80 {
private:
  BYTE m_value[10]; // Must be the first field in the class
public:
#ifdef IS32BIT
  // Assume x > _I16_MAX
  static inline void loadBigUINT16(USHORT x) {
    x &= _I16_MAX;
    __asm {
      fild x
      fadd _Dmaxi16P1
    }
  }
  // Assume x > _I32_MAX
  static inline void loadBigUINT32(UINT x) {
    x &= _I32_MAX;
    __asm {
      fild x
      fadd _Dmaxi32P1
    }
  }
  // Assume x > _I64_MAX. load x into fpu
  static inline void loadBigUINT64(UINT64 x) {
    x &= _I64_MAX;
    __asm {
      fild x
      fld _D80maxi64P1
      fadd
    }
  }

  inline Double80(short x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(USHORT x) {
    if(x <= _I16_MAX) {
      __asm {
        fild x
        mov eax, this
        fstp TBYTE PTR [eax]
      }
    } else {
      loadBigUINT16(x);
      __asm {
        mov eax, this
        fstp TBYTE PTR [eax]
      }
    }
  }
  inline Double80(int x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(UINT x) {
    if(x <= _I32_MAX) {
      __asm {
        fild x
        mov eax, this
        fstp TBYTE PTR [eax]
      }
    } else {
      loadBigUINT32(x);
      __asm {
        mov eax, this
        fstp TBYTE PTR [eax]
      }
    }
  }
  inline Double80(INT64 x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  Double80(UINT64 x) {
    if(x <= _I64_MAX) {
      __asm {
        fild x
        mov eax, this
        fstp TBYTE PTR [eax]
      }
    } else {
      loadBigUINT64(x);
      __asm {
        mov eax, this
        fstp TBYTE PTR [eax]
      }
    }
  }
  inline Double80(float x) {
    __asm {
      fld x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(double x) {
    __asm {
      fld x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
#else // IS64BIT
  inline Double80(short  x) { D80FromI16(  *this, x); }
  inline Double80(USHORT x) { D80FromUI16( *this, x); }
  inline Double80(int    x) { D80FromI32(  *this, x); }
  inline Double80(UINT   x) { D80FromUI32( *this, x); }
  inline Double80(INT64  x) { D80FromI64(  *this, x); }
  inline Double80(UINT64 x) { D80FromUI64( *this, x); }
  inline Double80(float  x) { D80FromFlt(  *this, x); }
  inline Double80(double x) { D80FromDbl(  *this, x); }
#endif // IS64BIT

  inline Double80() {
  }

  inline Double80(long x) {
    *this = (Double80)(int)(x);
  }

  inline Double80(ULONG x) {
    *this = (Double80)(UINT)(x);
  }

  explicit inline Double80(const BYTE *bytes) {
    memcpy(&m_value,bytes,sizeof(m_value));
  }

#ifdef IS32BIT
  static int    getExpo2( const Double80 &x);
  // x == 0 ? 0 : floor(log10(|x|))
  static int    getExpo10(const Double80 &x);

  // prefix-form
  inline Double80 &operator++() {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fadd
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  // prefix-form
  inline Double80 &operator--() {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fsub
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  // postfix-form
  inline Double80 operator++(int) {
    Double80 result(*this);
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fadd
      fstp TBYTE PTR [eax]
    }
    return result;
  }
  // postfix-form
  inline Double80 operator--(int) {
    Double80 result(*this);
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fsub
      fstp TBYTE PTR [eax]
    }
    return result;
  }

  inline bool isZero() const {
    bool result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      ftst
      fstsw ax
      sahf
      fstp    st(0)
      sete result;                  // result = (*this == 0) ? 1 : 0
    }
    return result;
  }

#else // IS64BIT

  static inline int getExpo2( const Double80 &x) { return D80getExpo2(x);  }
  // x == 0 ? 0 : floor(log10(|x|))
  static inline int getExpo10(const Double80 &x) { return D80getExpo10(x); }
  // prefix-form
  inline Double80 &operator++()   {  D80inc(*this);    return *this;  }
  inline Double80 &operator--()   {  D80dec(*this);    return *this;  }
  // postfix-form
  inline Double80 operator++(int) {  Double80 result(*this);  D80inc(*this);  return result;  }
  inline Double80 operator--(int) {  Double80 result(*this);  D80dec(*this);  return result;  }

  inline bool isZero() const      {  return D80isZero(*this) ? true : false;  }

#endif // IS64BIT

  inline bool isPositive() const { return (m_value[9] & 0x80) == 0; }
  inline bool isNegative() const { return (m_value[9] & 0x80) != 0; }

  ULONG hashCode() const;

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)m_value, sizeof(m_value));
  }

  inline void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)m_value, sizeof(m_value));
  }

  friend inline Packer &operator<<(Packer &p, const Double80 &d) {
    return p.addElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
  }

  friend inline Packer &operator>>(Packer &p, Double80 &d) {
    return p.getElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
  }

  static const Double80 zero;          // = 0
  static const Double80 one;           // = 1
  static const Double80 M_PI;          // = 3.1415926535897932384626433
  static const Double80 DBL80_EPSILON; // Smallest such that 1.0+DBL80_EPSILON != 1.0 (=1.08420217248550443e-019)
  static const Double80 DBL80_MIN;     // Min positive value (=3.36210314311209209e-4932)
  static const Double80 DBL80_MAX;     // Max value          (=1.18973149535723227e+4932)
  static const Double80 DBL80_NAN;     // nan (undefined)
  static const Double80 DBL80_PINF;    // +infinity;
  static const Double80 DBL80_NINF;    // -infinity;

  static const int      DBL80_DIG;     // # of decimal digits of precision

  static void initClass();
};

#ifdef IS32BIT

int    getInt(   const Double80 &x);
UINT   getUint(  const Double80 &x);
INT64  getInt64( const Double80 &x);
UINT64 getUint64(const Double80 &x);
inline float getFloat(const Double80 &x) {
  float result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fstp result
  }
  return result;
}







inline double getDouble(const Double80 &x) {
  double result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fstp result
  }
  return result;
}







inline Double80 operator+(const Double80 &x, short y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fiadd y
    fstp result
  }
  return result;
}






inline Double80 operator-(const Double80 &x, short y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fisub y
    fstp result
  }
  return result;
}






inline Double80 operator*(const Double80 &x, short y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fimul y
    fstp result
  }
  return result;
}






inline Double80 operator/(const Double80 &x, short y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fidiv y
    fstp result
  }
  return result;
}






inline Double80 operator+(const Double80 &x, USHORT y) {
  if(y <= _I16_MAX) {
    return x + (short)y;
  }
  Double80::loadBigUINT16(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}


inline Double80 operator-(const Double80 &x, USHORT y) {
  if(y <= _I16_MAX) {
    return x - (short)y;
  }
  Double80::loadBigUINT16(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsubrp st(1),st
    fstp result
  }
  return result;
}


inline Double80 operator*(const Double80 &x, USHORT y) {
  if(y <= _I16_MAX) {
    return x * (short)y;
  }
  Double80::loadBigUINT16(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}


inline Double80 operator/(const Double80 &x, USHORT y) {
  if(y <= _I16_MAX) {
    return x / (short)y;
  }
  Double80::loadBigUINT16(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp result
  }
  return result;
}


inline Double80 operator+(const Double80 &x, int y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fiadd y
    fstp result
  }
  return result;
}






inline Double80 operator-(const Double80 &x, int y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fisub y
    fstp result
  }
  return result;
}






inline Double80 operator*(const Double80 &x, int y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fimul y
    fstp result
  }
  return result;
}






inline Double80 operator/(const Double80 &x, int y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fidiv y
    fstp result
  }
  return result;
}






inline Double80 operator+(const Double80 &x, UINT y) {
  if(y <= _I32_MAX) {
    return x + (int)y;
  }
  Double80::loadBigUINT32(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}


inline Double80 operator-(const Double80 &x, UINT y) {
  if(y <= _I32_MAX) {
    return x - (int)y;
  }
  Double80::loadBigUINT32(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsubrp st(1),st
    fstp result
  }
  return result;
}


inline Double80 operator*(const Double80 &x, UINT y) {
  if(y <= _I32_MAX) {
    return x * (int)y;
  }
  Double80::loadBigUINT32(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}


inline Double80 operator/(const Double80 &x, UINT y) {
  if(y <= _I32_MAX) {
    return x / (int)y;
  }
  Double80::loadBigUINT32(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp result
  }
  return result;
}


inline Double80 operator+(const Double80 &x, long  y) { return x + (int )y; }
inline Double80 operator-(const Double80 &x, long  y) { return x - (int )y; }
inline Double80 operator*(const Double80 &x, long  y) { return x * (int )y; }
inline Double80 operator/(const Double80 &x, long  y) { return x / (int )y; }
inline Double80 operator+(const Double80 &x, ULONG y) { return x + (UINT)y; }
inline Double80 operator-(const Double80 &x, ULONG y) { return x - (UINT)y; }
inline Double80 operator*(const Double80 &x, ULONG y) { return x * (UINT)y; }
inline Double80 operator/(const Double80 &x, ULONG y) { return x / (UINT)y; }








inline Double80 operator+(const Double80 &x, INT64 y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fadd
    fstp result
  }
  return result;
}





inline Double80 operator-(const Double80 &x, INT64 y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fsub
    fstp result
  }
  return result;
}





inline Double80 operator*(const Double80 &x, INT64 y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fmul
    fstp result
  }
  return result;
}





inline Double80 operator/(const Double80 &x, INT64 y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fdiv
    fstp result
  }
  return result;
}





inline Double80 operator+(const Double80 &x, const UINT64 &y) {
  if(y <= _I64_MAX) {
    return x + (INT64)y;
  }
  Double80::loadBigUINT64(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}


inline Double80 operator-(const Double80 &x, const UINT64 &y) {
  if(y <= _I64_MAX) {
    return x - (INT64)y;
  }
  Double80::loadBigUINT64(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsubrp st(1),st
    fstp result
  }
  return result;
}


inline Double80 operator*(const Double80 &x, const UINT64 &y) {
  if(y <= _I64_MAX) {
    return x * (INT64)y;
  }
  Double80::loadBigUINT64(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}


inline Double80 operator/(const Double80 &x, const UINT64 &y) {
  if(y <= _I64_MAX) {
    return x / (INT64)y;
  }
  Double80::loadBigUINT64(y);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp result
  }
  return result;
}


inline Double80 operator+(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd y
    fstp result
  }
  return result;
}






inline Double80 operator-(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsub y
    fstp result
  }
  return result;
}






inline Double80 operator*(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul y
    fstp result
  }
  return result;
}






inline Double80 operator/(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdiv y
    fstp result
  }
  return result;
}






inline Double80 operator+(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd y
    fstp result
  }
  return result;
}






inline Double80 operator-(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsub y
    fstp result
  }
  return result;
}






inline Double80 operator*(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul y
    fstp result
  }
  return result;
}






inline Double80 operator/(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdiv y
    fstp result
  }
  return result;
}






inline Double80 operator+(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}




inline Double80 operator-(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsub
    fstp result
  }
  return result;
}




inline Double80 operator-(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fchs
    fstp result
  }
  return result;
}






inline Double80 operator*(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}




inline Double80 operator/(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdiv
    fstp result
  }
  return result;
}




inline Double80 operator+(short           x, const Double80 &y) { return y + x; }
inline Double80 operator-(short           x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fisubr x
    fstp result
  }
  return result;
}





inline Double80 operator*(short           x, const Double80 &y) { return y * x; }
inline Double80 operator/(short           x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fidivr x
    fstp result
  }
  return result;
}





inline Double80 operator+(USHORT          x, const Double80 &y) { return y + x; }
inline Double80 operator-(USHORT          x, const Double80 &y) {
  if(x <= _I16_MAX) {
    return (short)x - y;
  }
  Double80::loadBigUINT16(x);
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubp st(1), st
    fstp result
  }
  return result;
}

inline Double80 operator*(USHORT          x, const Double80 &y) { return y * x; }
inline Double80 operator/(USHORT          x, const Double80 &y) {
  if(x <= _I16_MAX) {
    return (short)x / y;
  }
  Double80::loadBigUINT16(x);
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivp st(1), st
    fstp result
  }
  return result;
}

inline Double80 operator+(int             x, const Double80 &y) { return y + x; }
inline Double80 operator-(int             x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fisubr x
    fstp result
  }
  return result;
}





inline Double80 operator*(int             x, const Double80 &y) { return y * x; }
inline Double80 operator/(int             x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fidivr x
    fstp result
  }
  return result;
}





inline Double80 operator+(UINT            x, const Double80 &y) { return y + x; }
inline Double80 operator-(UINT            x, const Double80 &y) {
  if(x <= _I32_MAX) {
    return (int)x - y;
  }
  Double80::loadBigUINT32(x);
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubp st(1), st
    fstp result
  }
  return result;
}

inline Double80 operator*(UINT            x, const Double80 &y) { return y * x; }
inline Double80 operator/(UINT            x, const Double80 &y) {
  if(x <= _I32_MAX) {
    return (int)x / y;
  }
  Double80::loadBigUINT32(x);
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivp st(1), st
    fstp result
  }
  return result;
}

inline Double80 operator+(long            x, const Double80 &y) { return y + x; }
inline Double80 operator-(long            x, const Double80 &y) { return (int)x - y; }
inline Double80 operator*(long            x, const Double80 &y) { return y * x; }
inline Double80 operator/(long            x, const Double80 &y) { return (int)x / y; }
inline Double80 operator+(ULONG           x, const Double80 &y) { return y + x; }
inline Double80 operator-(ULONG           x, const Double80 &y) { return (UINT)x - y; }
inline Double80 operator*(ULONG           x, const Double80 &y) { return y * x; }
inline Double80 operator/(ULONG           x, const Double80 &y) { return (UINT)x / y; }








inline Double80 operator+(const INT64     x, const Double80 &y) { return y + x; }
inline Double80 operator-(const INT64     x, const Double80 &y) {
  Double80 result;
  __asm {
    fild x
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsub
    fstp result
  }
  return result;
}




inline Double80 operator*(const INT64     x, const Double80 &y) { return y * x; }
inline Double80 operator/(const INT64     x, const Double80 &y) {
  Double80 result;
  __asm {
    fild x
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdiv
    fstp result
  }
  return result;
}




inline Double80 operator+(UINT64          x, const Double80 &y) { return y + x; }
inline Double80 operator-(UINT64          x, const Double80 &y) {
  if(x <= _I64_MAX) {
    return (INT64)x - y;
  }
  Double80::loadBigUINT64(x);
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubp st(1), st
    fstp result
  }
  return result;
}

inline Double80 operator*(UINT64          x, const Double80 &y) { return y * x; }
inline Double80 operator/(UINT64          x, const Double80 &y) {
  if(x <= _I64_MAX) {
    return (INT64)x / y;
  }
  Double80::loadBigUINT64(x);
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivp st(1), st
    fstp result
  }
  return result;
}

inline Double80 operator+(const float     x, const Double80 &y) { return y + x; }
inline Double80 operator-(const float     x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubr x
    fstp result
  }
  return result;
}





inline Double80 operator*(const float     x, const Double80 &y) { return y * x; }
inline Double80 operator/(const float     x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivr x
    fstp result
  }
  return result;
}





inline Double80 operator+(const double    x, const Double80 &y) { return y + x; }
inline Double80 operator-(const double    x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubr x
    fstp result
  }
  return result;
}





inline Double80 operator*(const double    x, const Double80 &y) { return y * x; }
inline Double80 operator/(const double    x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivr x
    fstp result
  }
  return result;
}





inline Double80 &operator+=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fiadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator-=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fisub y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator*=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fimul y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator/=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fidiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator+=(Double80 &x, USHORT            y) {
  if(y <= _I16_MAX) {
    return x += (short)y;
  }
  Double80::loadBigUINT16(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator-=(Double80 &x, USHORT            y) {
  if(y <= _I16_MAX) {
    return x -= (short)y;
  }
  Double80::loadBigUINT16(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsubrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator*=(Double80 &x, USHORT            y) {
  if(y <= _I16_MAX) {
    return x *= (short)y;
  }
  Double80::loadBigUINT16(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator/=(Double80 &x, USHORT            y) {
  if(y <= _I16_MAX) {
    return x /= (short)y;
  }
  Double80::loadBigUINT16(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator+=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fiadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator-=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fisub y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator*=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fimul y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator/=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fidiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator+=(Double80 &x, UINT              y) {
  if(y <= _I32_MAX) {
    return x += (int)y;
  }
  Double80::loadBigUINT32(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator-=(Double80 &x, UINT              y) {
  if(y <= _I32_MAX) {
    return x -= (int)y;
  }
  Double80::loadBigUINT32(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsubrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator*=(Double80 &x, UINT              y) {
  if(y <= _I32_MAX) {
    return x *= (int)y;
  }
  Double80::loadBigUINT32(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator/=(Double80 &x, UINT              y) {
  if(y <= _I32_MAX) {
    return x /= (int)y;
  }
  Double80::loadBigUINT32(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator+=(Double80 &x, long  y) { return x += (int )y; }
inline Double80 &operator-=(Double80 &x, long  y) { return x -= (int )y; }
inline Double80 &operator*=(Double80 &x, long  y) { return x *= (int )y; }
inline Double80 &operator/=(Double80 &x, long  y) { return x /= (int )y; }
inline Double80 &operator+=(Double80 &x, ULONG y) { return x += (UINT)y; }
inline Double80 &operator-=(Double80 &x, ULONG y) { return x -= (UINT)y; }
inline Double80 &operator*=(Double80 &x, ULONG y) { return x *= (UINT)y; }
inline Double80 &operator/=(Double80 &x, ULONG y) { return x /= (UINT)y; }








inline Double80 &operator+=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}






inline Double80 &operator-=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fsub
    fstp TBYTE PTR [eax]
  }
  return x;
}






inline Double80 &operator*=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}






inline Double80 &operator/=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fdiv
    fstp TBYTE PTR [eax]
  }
  return x;
}






inline Double80 &operator+=(Double80 &x, const UINT64     &y) {
  if(y <= _I64_MAX) {
    return x += (INT64)y;
  }
  Double80::loadBigUINT64(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator-=(Double80 &x, const UINT64     &y) {
  if(y <= _I64_MAX) {
    return x -= (INT64)y;
  }
  Double80::loadBigUINT64(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsubrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator*=(Double80 &x, const UINT64     &y) {
  if(y <= _I64_MAX) {
    return x *= (INT64)y;
  }
  Double80::loadBigUINT64(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator/=(Double80 &x, const UINT64     &y) {
  if(y <= _I64_MAX) {
    return x /= (INT64)y;
  }
  Double80::loadBigUINT64(y);
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdivrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}



inline Double80 &operator+=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator-=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsub y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator*=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator/=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator+=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator-=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsub y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator*=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator/=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}







inline Double80 &operator+=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}





inline Double80 &operator-=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fsub
    fstp TBYTE PTR [eax]
  }
  return x;
}





inline Double80 &operator*=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}





inline Double80 &operator/=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fdiv
    fstp TBYTE PTR [eax]
  }
  return x;
}





inline bool operator==(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}




inline bool operator!=(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}




inline bool operator<=(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}




inline bool operator>=(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}




inline bool operator< (const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}




inline bool operator> (const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}




inline bool operator==(const Double80 &x, USHORT           y) {
  bool result;
  if(y <= _I16_MAX) {
    return x == (short)y;
  }
  Double80::loadBigUINT16(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator!=(const Double80 &x, USHORT           y) {
  bool result;
  if(y <= _I16_MAX) {
    return x != (short)y;
  }
  Double80::loadBigUINT16(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator<=(const Double80 &x, USHORT           y) {
  bool result;
  if(y <= _I16_MAX) {
    return x <= (short)y;
  }
  Double80::loadBigUINT16(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator>=(const Double80 &x, USHORT           y) {
  bool result;
  if(y <= _I16_MAX) {
    return x >= (short)y;
  }
  Double80::loadBigUINT16(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator< (const Double80 &x, USHORT           y) {
  bool result;
  if(y <= _I16_MAX) {
    return x <  (short)y;
  }
  Double80::loadBigUINT16(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator> (const Double80 &x, USHORT           y) {
  bool result;
  if(y <= _I16_MAX) {
    return x >  (short)y;
  }
  Double80::loadBigUINT16(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator==(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}




inline bool operator!=(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}




inline bool operator<=(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}




inline bool operator>=(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}




inline bool operator< (const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}




inline bool operator> (const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}




inline bool operator==(const Double80 &x, UINT             y) {
  bool result;
  if(y <= _I32_MAX) {
    return x == (int)y;
  }
  Double80::loadBigUINT32(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator!=(const Double80 &x, UINT             y) {
  bool result;
  if(y <= _I32_MAX) {
    return x != (int)y;
  }
  Double80::loadBigUINT32(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator<=(const Double80 &x, UINT             y) {
  bool result;
  if(y <= _I32_MAX) {
    return x <= (int)y;
  }
  Double80::loadBigUINT32(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator>=(const Double80 &x, UINT             y) {
  bool result;
  if(y <= _I32_MAX) {
    return x >= (int)y;
  }
  Double80::loadBigUINT32(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator< (const Double80 &x, UINT             y) {
  bool result;
  if(y <= _I32_MAX) {
    return x <  (int)y;
  }
  Double80::loadBigUINT32(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator> (const Double80 &x, UINT             y) {
  bool result;
  if(y <= _I32_MAX) {
    return x >  (int)y;
  }
  Double80::loadBigUINT32(y);   // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator==(const Double80 &x, long     y) { return x == (int )y; }
inline bool operator!=(const Double80 &x, long     y) { return x != (int )y; }
inline bool operator<=(const Double80 &x, long     y) { return x <= (int )y; }
inline bool operator>=(const Double80 &x, long     y) { return x >= (int )y; }
inline bool operator< (const Double80 &x, long     y) { return x <  (int )y; }
inline bool operator> (const Double80 &x, long     y) { return x >  (int )y; }
inline bool operator==(const Double80 &x, ULONG    y) { return x == (UINT)y; }
inline bool operator!=(const Double80 &x, ULONG    y) { return x != (UINT)y; }
inline bool operator<=(const Double80 &x, ULONG    y) { return x <= (UINT)y; }
inline bool operator>=(const Double80 &x, ULONG    y) { return x >= (UINT)y; }
inline bool operator< (const Double80 &x, ULONG    y) { return x <  (UINT)y; }
inline bool operator> (const Double80 &x, ULONG    y) { return x >  (UINT)y; }




inline bool operator==(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator!=(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator<=(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator>=(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator< (const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator> (const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator==(const Double80 &x, const UINT64    &y) {
  bool result;
  if(y <= _I64_MAX) {
    return x == (INT64&)y;
  }
  Double80::loadBigUINT64(y);   // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator!=(const Double80 &x, const UINT64    &y) {
  bool result;
  if(y <= _I64_MAX) {
    return x != (INT64&)y;
  }
  Double80::loadBigUINT64(y);   // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator<=(const Double80 &x, const UINT64    &y) {
  bool result;
  if(y <= _I64_MAX) {
    return x <= (INT64&)y;
  }
  Double80::loadBigUINT64(y);   // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator>=(const Double80 &x, const UINT64    &y) {
  bool result;
  if(y <= _I64_MAX) {
    return x >= (INT64&)y;
  }
  Double80::loadBigUINT64(y);   // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator< (const Double80 &x, const UINT64    &y) {
  bool result;
  if(y <= _I64_MAX) {
    return x <  (INT64&)y;
  }
  Double80::loadBigUINT64(y);   // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator> (const Double80 &x, const UINT64    &y) {
  bool result;
  if(y <= _I64_MAX) {
    return x >  (INT64&)y;
  }
  Double80::loadBigUINT64(y);   // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}

inline bool operator==(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}




inline bool operator!=(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}




inline bool operator<=(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}




inline bool operator>=(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}




inline bool operator< (const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}




inline bool operator> (const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}




inline bool operator==(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}



inline bool operator!=(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}



inline bool operator<=(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}



inline bool operator>=(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}



inline bool operator< (const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}



inline bool operator> (const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}



inline bool operator==(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator!=(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator>=(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator<=(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator> (const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline bool operator< (const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}



inline Double80 fabs(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fabs
    fstp result
  }
  return result;
}

Double80 fmod(const Double80 &x, const Double80 &y);
inline Double80 sqr(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}
inline Double80 sqrt(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsqrt
    fstp result
  }
  return result;
}
Double80 sin(   const Double80 &x);
Double80 cos(   const Double80 &x);
Double80 tan(   const Double80 &x);
inline Double80 atan(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fld1
    fpatan
    fstp result
  }
  return result;
}
inline Double80 atan2(const Double80 &y, const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fpatan
    fstp result
  }
  return result;
}

// calculate both cos and sin. c:inout c, s:out
void     sincos(Double80 &c, Double80 &s);
Double80 exp(  const Double80 &x);
Double80 exp10(const Double80 &x);
Double80 exp2( const Double80 &x);
Double80 log(  const Double80 &x);
Double80 log10(const Double80 &x);
Double80 log2( const Double80 &x);
Double80 pow(  const Double80 &x, const Double80 &y);
inline Double80 floor(const Double80 &x) {
  const USHORT cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    frndint
    fstp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}
inline Double80 ceil(const Double80 &x) {
  const USHORT cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDUP);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    frndint
    fstp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

inline void D80ToBCD(BYTE bcd[10], const Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR bcd
    fbstp TBYTE PTR [eax]
  }
}
void D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);

#else // IS64BIT
inline int    getInt(   const Double80 &x) { return D80ToI32(   x); }
inline UINT   getUint(  const Double80 &x) { return D80ToUI32(  x); }
inline INT64  getInt64( const Double80 &x) { return D80ToI64(   x); }
inline UINT64 getUint64(const Double80 &x) { return D80ToUI64(  x); }
inline float  getFloat( const Double80 &x) { return D80ToFlt(   x); }
inline double getDouble(const Double80 &x) { return D80ToDbl(   x); }

inline Double80 operator+(const Double80 &x, const short    &y) { Double80 t(x); D80addI16(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const short    &y) { Double80 t(x); D80subI16(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const short    &y) { Double80 t(x); D80mulI16(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const short    &y) { Double80 t(x); D80divI16(    t, y); return t;}
inline Double80 operator+(const Double80 &x, USHORT          y) { Double80 t(x); D80addUI16(   t, y); return t;}
inline Double80 operator-(const Double80 &x, USHORT          y) { Double80 t(x); D80subUI16(   t, y); return t;}
inline Double80 operator*(const Double80 &x, USHORT          y) { Double80 t(x); D80mulUI16(   t, y); return t;}
inline Double80 operator/(const Double80 &x, USHORT          y) { Double80 t(x); D80divUI16(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const int      &y) { Double80 t(x); D80addI32(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const int      &y) { Double80 t(x); D80subI32(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const int      &y) { Double80 t(x); D80mulI32(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const int      &y) { Double80 t(x); D80divI32(    t, y); return t;}
inline Double80 operator+(const Double80 &x, UINT            y) { Double80 t(x); D80addUI32(   t, y); return t;}
inline Double80 operator-(const Double80 &x, UINT            y) { Double80 t(x); D80subUI32(   t, y); return t;}
inline Double80 operator*(const Double80 &x, UINT            y) { Double80 t(x); D80mulUI32(   t, y); return t;}
inline Double80 operator/(const Double80 &x, UINT            y) { Double80 t(x); D80divUI32(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const long     &y) { Double80 t(x); D80addI32(    t, (int&)y); return t;}
inline Double80 operator-(const Double80 &x, const long     &y) { Double80 t(x); D80subI32(    t, (int&)y); return t;}
inline Double80 operator*(const Double80 &x, const long     &y) { Double80 t(x); D80mulI32(    t, (int&)y); return t;}
inline Double80 operator/(const Double80 &x, const long     &y) { Double80 t(x); D80divI32(    t, (int&)y); return t;}
inline Double80 operator+(const Double80 &x, ULONG           y) { Double80 t(x); D80addUI32(   t, y); return t;}
inline Double80 operator-(const Double80 &x, ULONG           y) { Double80 t(x); D80subUI32(   t, y); return t;}
inline Double80 operator*(const Double80 &x, ULONG           y) { Double80 t(x); D80mulUI32(   t, y); return t;}
inline Double80 operator/(const Double80 &x, ULONG           y) { Double80 t(x); D80divUI32(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const INT64    &y) { Double80 t(x); D80addI64(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const INT64    &y) { Double80 t(x); D80subI64(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const INT64    &y) { Double80 t(x); D80mulI64(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const INT64    &y) { Double80 t(x); D80divI64(    t, y); return t;}
inline Double80 operator+(const Double80 &x, UINT64          y) { Double80 t(x); D80addUI64(   t, y); return t;}
inline Double80 operator-(const Double80 &x, UINT64          y) { Double80 t(x); D80subUI64(   t, y); return t;}
inline Double80 operator*(const Double80 &x, UINT64          y) { Double80 t(x); D80mulUI64(   t, y); return t;}
inline Double80 operator/(const Double80 &x, UINT64          y) { Double80 t(x); D80divUI64(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const float    &y) { Double80 t(x); D80addFlt(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const float    &y) { Double80 t(x); D80subFlt(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const float    &y) { Double80 t(x); D80mulFlt(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const float    &y) { Double80 t(x); D80divFlt(    t, y); return t;}
inline Double80 operator+(const Double80 &x, const double   &y) { Double80 t(x); D80addDbl(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const double   &y) { Double80 t(x); D80subDbl(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const double   &y) { Double80 t(x); D80mulDbl(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const double   &y) { Double80 t(x); D80divDbl(    t, y); return t;}
inline Double80 operator+(const Double80 &x, const Double80 &y) { Double80 t(x); D80addD80(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const Double80 &y) { Double80 t(x); D80subD80(    t, y); return t;}
inline Double80 operator-(const Double80 &x)                    { Double80 t(x); D80neg(       t   ); return t;}
inline Double80 operator*(const Double80 &x, const Double80 &y) { Double80 t(x); D80mulD80(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const Double80 &y) { Double80 t(x); D80divD80(    t, y); return t;}
inline Double80 operator+(const short    &x, const Double80 &y) { Double80 t(y); D80addI16(    t, x); return t;}
inline Double80 operator-(const short    &x, const Double80 &y) { Double80 t(y); D80subrI16(   t, x); return t;}
inline Double80 operator*(const short    &x, const Double80 &y) { Double80 t(y); D80mulI16(    t, x); return t;}
inline Double80 operator/(const short    &x, const Double80 &y) { Double80 t(y); D80divrI16(   t, x); return t;}
inline Double80 operator+(USHORT          x, const Double80 &y) { Double80 t(y); D80addUI16(   t, x); return t;}
inline Double80 operator-(USHORT          x, const Double80 &y) { Double80 t(y); D80subrUI16(  t, x); return t;}
inline Double80 operator*(USHORT          x, const Double80 &y) { Double80 t(y); D80mulUI16(   t, x); return t;}
inline Double80 operator/(USHORT          x, const Double80 &y) { Double80 t(y); D80divrUI16(  t, x); return t;}
inline Double80 operator+(const int      &x, const Double80 &y) { Double80 t(y); D80addI32(    t, x); return t;}
inline Double80 operator-(const int      &x, const Double80 &y) { Double80 t(y); D80subrI32(   t, x); return t;}
inline Double80 operator*(const int      &x, const Double80 &y) { Double80 t(y); D80mulI32(    t, x); return t;}
inline Double80 operator/(const int      &x, const Double80 &y) { Double80 t(y); D80divrI32(   t, x); return t;}
inline Double80 operator+(UINT            x, const Double80 &y) { Double80 t(y); D80addUI32(   t, x); return t;}
inline Double80 operator-(UINT            x, const Double80 &y) { Double80 t(y); D80subrUI32(  t, x); return t;}
inline Double80 operator*(UINT            x, const Double80 &y) { Double80 t(y); D80mulUI32(   t, x); return t;}
inline Double80 operator/(UINT            x, const Double80 &y) { Double80 t(y); D80divrUI32(  t, x); return t;}
inline Double80 operator+(const long     &x, const Double80 &y) { Double80 t(y); D80addI32(    t, (int&)x); return t;}
inline Double80 operator-(const long     &x, const Double80 &y) { Double80 t(y); D80subrI32(   t, (int&)x); return t;}
inline Double80 operator*(const long     &x, const Double80 &y) { Double80 t(y); D80mulI32(    t, (int&)x); return t;}
inline Double80 operator/(const long     &x, const Double80 &y) { Double80 t(y); D80divrI32(   t, (int&)x); return t;}
inline Double80 operator+(ULONG           x, const Double80 &y) { Double80 t(y); D80addUI32(   t, x); return t;}
inline Double80 operator-(ULONG           x, const Double80 &y) { Double80 t(y); D80subrUI32(  t, x); return t;}
inline Double80 operator*(ULONG           x, const Double80 &y) { Double80 t(y); D80mulUI32(   t, x); return t;}
inline Double80 operator/(ULONG           x, const Double80 &y) { Double80 t(y); D80divrUI32(  t, x); return t;}
inline Double80 operator+(const INT64    &x, const Double80 &y) { Double80 t(y); D80addI64(    t, x); return t;}
inline Double80 operator-(const INT64    &x, const Double80 &y) { Double80 t(y); D80subrI64(   t, x); return t;}
inline Double80 operator*(const INT64    &x, const Double80 &y) { Double80 t(y); D80mulI64(    t, x); return t;}
inline Double80 operator/(const INT64    &x, const Double80 &y) { Double80 t(y); D80divrI64(   t, x); return t;}
inline Double80 operator+(UINT64          x, const Double80 &y) { Double80 t(y); D80addUI64(   t, x); return t;}
inline Double80 operator-(UINT64          x, const Double80 &y) { Double80 t(y); D80subrUI64(  t, x); return t;}
inline Double80 operator*(UINT64          x, const Double80 &y) { Double80 t(y); D80mulUI64(   t, x); return t;}
inline Double80 operator/(UINT64          x, const Double80 &y) { Double80 t(y); D80divrUI64(  t, x); return t;}
inline Double80 operator+(const float    &x, const Double80 &y) { Double80 t(y); D80addFlt(    t, x); return t;}
inline Double80 operator-(const float    &x, const Double80 &y) { Double80 t(y); D80subrFlt(   t, x); return t;}
inline Double80 operator*(const float    &x, const Double80 &y) { Double80 t(y); D80mulFlt(    t, x); return t;}
inline Double80 operator/(const float    &x, const Double80 &y) { Double80 t(y); D80divrFlt(   t, x); return t;}
inline Double80 operator+(const double   &x, const Double80 &y) { Double80 t(y); D80addDbl(    t, x); return t;}
inline Double80 operator-(const double   &x, const Double80 &y) { Double80 t(y); D80subrDbl(   t, x); return t;}
inline Double80 operator*(const double   &x, const Double80 &y) { Double80 t(y); D80mulDbl(    t, x); return t;}
inline Double80 operator/(const double   &x, const Double80 &y) { Double80 t(y); D80divrDbl(   t, x); return t;}

inline Double80 &operator+=( Double80 &x, const short    &y) { D80addI16(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const short    &y) { D80subI16(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const short    &y) { D80mulI16(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const short    &y) { D80divI16(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, USHORT          y) { D80addUI16(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, USHORT          y) { D80subUI16(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, USHORT          y) { D80mulUI16(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, USHORT          y) { D80divUI16(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const int      &y) { D80addI32(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const int      &y) { D80subI32(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const int      &y) { D80mulI32(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const int      &y) { D80divI32(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, UINT            y) { D80addUI32(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, UINT            y) { D80subUI32(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, UINT            y) { D80mulUI32(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, UINT            y) { D80divUI32(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const long     &y) { D80addI32(   x, (int&)y); return x; }
inline Double80 &operator-=( Double80 &x, const long     &y) { D80subI32(   x, (int&)y); return x; }
inline Double80 &operator*=( Double80 &x, const long     &y) { D80mulI32(   x, (int&)y); return x; }
inline Double80 &operator/=( Double80 &x, const long     &y) { D80divI32(   x, (int&)y); return x; }
inline Double80 &operator+=( Double80 &x, ULONG           y) { D80addUI32(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, ULONG           y) { D80subUI32(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, ULONG           y) { D80mulUI32(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, ULONG           y) { D80divUI32(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const INT64    &y) { D80addI64(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const INT64    &y) { D80subI64(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const INT64    &y) { D80mulI64(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const INT64    &y) { D80divI64(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, UINT64          y) { D80addUI64(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, UINT64          y) { D80subUI64(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, UINT64          y) { D80mulUI64(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, UINT64          y) { D80divUI64(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const float    &y) { D80addFlt(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const float    &y) { D80subFlt(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const float    &y) { D80mulFlt(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const float    &y) { D80divFlt(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, const double   &y) { D80addDbl(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const double   &y) { D80subDbl(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const double   &y) { D80mulDbl(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const double   &y) { D80divDbl(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, const Double80 &y) { D80addD80(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const Double80 &y) { D80subD80(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const Double80 &y) { D80mulD80(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const Double80 &y) { D80divD80(   x, y); return x; }

inline bool operator==(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const short    &y) { return D80cmpI16(   x, y) <  0; }
inline bool operator> (const Double80 &x, const short    &y) { return D80cmpI16(   x, y) >  0; }
inline bool operator==(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) == 0; }
inline bool operator!=(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) != 0; }
inline bool operator<=(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) >= 0; }
inline bool operator< (const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) <  0; }
inline bool operator> (const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) >  0; }
inline bool operator==(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const int      &y) { return D80cmpI32(   x, y) <  0; }
inline bool operator> (const Double80 &x, const int      &y) { return D80cmpI32(   x, y) >  0; }
inline bool operator==(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) == 0; }
inline bool operator!=(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) != 0; }
inline bool operator<=(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) >= 0; }
inline bool operator< (const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) <  0; }
inline bool operator> (const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) >  0; }
inline bool operator==(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) == 0; }
inline bool operator!=(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) != 0; }
inline bool operator<=(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) <= 0; }
inline bool operator>=(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) >= 0; }
inline bool operator< (const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) <  0; }
inline bool operator> (const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) >  0; }
inline bool operator==(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) == 0; }
inline bool operator!=(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) != 0; }
inline bool operator<=(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) >= 0; }
inline bool operator< (const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) <  0; }
inline bool operator> (const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) >  0; }
inline bool operator==(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) <  0; }
inline bool operator> (const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) >  0; }
inline bool operator==(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) == 0; }
inline bool operator!=(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) != 0; }
inline bool operator<=(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) >= 0; }
inline bool operator< (const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) <  0; }
inline bool operator> (const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) >  0; }
inline bool operator==(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) <  0; }
inline bool operator> (const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) >  0; }
inline bool operator==(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) <  0; }
inline bool operator> (const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) >  0; }
inline bool operator==(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) <  0; }
inline bool operator> (const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) >  0; }

inline Double80 fabs(const Double80 &x) {
  Double80 tmp(x);
  D80fabs(tmp);
  return tmp;
}
inline Double80 fmod(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80rem(tmp, y);
  return tmp;
}
inline Double80 sqr(const Double80 &x) {
  Double80 tmp(x);
  D80sqr(tmp);
  return tmp;
}
inline Double80 sqrt(const Double80 &x) {
  Double80 tmp(x);
  D80sqrt(tmp);
  return tmp;
}
inline Double80 sin(const Double80 &x) {
  Double80 tmp(x);
  D80sin(tmp);
  return tmp;
}
inline Double80 cos(const Double80 &x) {
  Double80 tmp(x);
  D80cos(tmp);
  return tmp;
}
inline Double80 tan(const Double80 &x) {
  Double80 tmp(x);
  D80tan(tmp);
  return tmp;
}
inline Double80 atan(const Double80 &x) {
  Double80 tmp(x);
  D80atan(tmp);
  return tmp;
}
inline Double80 atan2(const Double80 &y, const Double80 &x) {
  Double80 tmp(y);
  D80atan2(tmp, x);
  return tmp;
}
// calculate both cos and sin. c:inout c, s:out
inline void sincos(Double80 &c, Double80 &s) {
  D80sincos(c, s);
}
inline Double80 exp(const Double80 &x) {
  Double80 tmp(x);
  D80exp(tmp);
  return tmp;
}
inline Double80 exp10(const Double80 &x) {
  Double80 tmp(x);
  D80exp10(tmp);
  return tmp;
}
inline Double80 exp2(const Double80 &x) {
  Double80 tmp(x);
  D80exp2(tmp);
  return tmp;
}
inline Double80 log(const Double80 &x) {
  Double80 tmp(x);
  D80log(tmp);
  return tmp;
}
inline Double80 log10(const Double80 &x) {
  Double80 tmp(x);
  D80log10(tmp);
  return tmp;
}
inline Double80 log2(const Double80 &x) {
  Double80 tmp(x);
  D80log2(tmp);
  return tmp;
}
inline Double80 pow(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80pow(tmp, y);
  return tmp;
}
inline Double80 floor(const Double80 &x) {
  Double80 tmp(x);
  D80floor(tmp);
  return tmp;
}
inline Double80 ceil(const Double80 &x) {
  Double80 tmp(x);
  D80ceil(tmp);
  return tmp;
}

#endif // IS64BIT

inline short  getShort(const Double80 &x) {
  return (short)getInt(x);
}
inline USHORT getUshort(const Double80 &x) {
#ifdef IS32BIT
  return (USHORT)getInt(x);
#else
  return (USHORT)getUint(x);
#endif
}

inline long getLong(const Double80 &x) {
  return getInt(x);
}

inline ULONG getUlong(const Double80 &x) {
  return getUint(x);
}

inline bool operator==(short   x, const Double80 &y) {  return y == x; }
inline bool operator==(USHORT  x, const Double80 &y) {  return y == x; }
inline bool operator==(int     x, const Double80 &y) {  return y == x; }
inline bool operator==(UINT    x, const Double80 &y) {  return y == x; }
inline bool operator==(long    x, const Double80 &y) {  return y == x; }
inline bool operator==(ULONG   x, const Double80 &y) {  return y == x; }
inline bool operator==(INT64   x, const Double80 &y) {  return y == x; }
inline bool operator==(UINT64  x, const Double80 &y) {  return y == x; }
inline bool operator==(float   x, const Double80 &y) {  return y == x; }
inline bool operator==(double  x, const Double80 &y) {  return y == x; }
inline bool operator!=(short   x, const Double80 &y) {  return y != x; }
inline bool operator!=(USHORT  x, const Double80 &y) {  return y != x; }
inline bool operator!=(int     x, const Double80 &y) {  return y != x; }
inline bool operator!=(UINT    x, const Double80 &y) {  return y != x; }
inline bool operator!=(long    x, const Double80 &y) {  return y != x; }
inline bool operator!=(ULONG   x, const Double80 &y) {  return y != x; }
inline bool operator!=(INT64   x, const Double80 &y) {  return y != x; }
inline bool operator!=(UINT64  x, const Double80 &y) {  return y != x; }
inline bool operator!=(float   x, const Double80 &y) {  return y != x; }
inline bool operator!=(double  x, const Double80 &y) {  return y != x; }
inline bool operator<=(short   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(USHORT  x, const Double80 &y) {  return y >= x; }
inline bool operator<=(int     x, const Double80 &y) {  return y >= x; }
inline bool operator<=(UINT    x, const Double80 &y) {  return y >= x; }
inline bool operator<=(long    x, const Double80 &y) {  return y >= x; }
inline bool operator<=(ULONG   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(INT64   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(UINT64  x, const Double80 &y) {  return y >= x; }
inline bool operator<=(float   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(double  x, const Double80 &y) {  return y >= x; }
inline bool operator>=(short   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(USHORT  x, const Double80 &y) {  return y <= x; }
inline bool operator>=(int     x, const Double80 &y) {  return y <= x; }
inline bool operator>=(UINT    x, const Double80 &y) {  return y <= x; }
inline bool operator>=(long    x, const Double80 &y) {  return y <= x; }
inline bool operator>=(ULONG   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(INT64   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(UINT64  x, const Double80 &y) {  return y <= x; }
inline bool operator>=(float   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(double  x, const Double80 &y) {  return y <= x; }
inline bool operator< (short   x, const Double80 &y) {  return y >  x; }
inline bool operator< (USHORT  x, const Double80 &y) {  return y >  x; }
inline bool operator< (int     x, const Double80 &y) {  return y >  x; }
inline bool operator< (UINT    x, const Double80 &y) {  return y >  x; }
inline bool operator< (long    x, const Double80 &y) {  return y >  x; }
inline bool operator< (ULONG   x, const Double80 &y) {  return y >  x; }
inline bool operator< (INT64   x, const Double80 &y) {  return y >  x; }
inline bool operator< (UINT64  x, const Double80 &y) {  return y >  x; }
inline bool operator< (float   x, const Double80 &y) {  return y >  x; }
inline bool operator< (double  x, const Double80 &y) {  return y >  x; }
inline bool operator> (short   x, const Double80 &y) {  return y <  x; }
inline bool operator> (USHORT  x, const Double80 &y) {  return y <  x; }
inline bool operator> (int     x, const Double80 &y) {  return y <  x; }
inline bool operator> (UINT    x, const Double80 &y) {  return y <  x; }
inline bool operator> (long    x, const Double80 &y) {  return y <  x; }
inline bool operator> (ULONG   x, const Double80 &y) {  return y <  x; }
inline bool operator> (INT64   x, const Double80 &y) {  return y <  x; }
inline bool operator> (UINT64  x, const Double80 &y) {  return y <  x; }
inline bool operator> (float   x, const Double80 &y) {  return y <  x; }
inline bool operator> (double  x, const Double80 &y) {  return y <  x; }

inline bool isDouble(const Double80 &x) {
  return x == getDouble(x);
}
inline bool isFloat(const Double80 &x) {
  return x == getFloat(x);
}
inline bool isInt64(const Double80 &x) {
  return x == getInt64(x);
}
inline bool isInt(const Double80 &x) {
  return x == getInt(x);
}

Double80 cot(  const Double80 &x);
Double80 asin( const Double80 &x);
Double80 acos( const Double80 &x);
Double80 acot( const Double80 &x);
Double80 root( const Double80 &x, const Double80 &y);
Double80 cosh( const Double80 &x);
Double80 sinh( const Double80 &x);
Double80 tanh( const Double80 &x);
Double80 acosh(const Double80 &x);
Double80 asinh(const Double80 &x);
Double80 atanh(const Double80 &x);
Double80 hypot(const Double80 &x, const Double80 &y);
Double80 fraction(   const Double80 &x);
int      sign(       const Double80 &x);
Double80 round(      const Double80 &x, int prec = 0);
Double80 Max(        const Double80 &x, const Double80 &y);
Double80 Min(        const Double80 &x, const Double80 &y);
Double80 minMax(     const Double80 &x, const Double80 &x1, const Double80 &x2);
bool     isNan(      const Double80 &x);
bool     isPInfinity(const Double80 &x);
bool     isNInfinity(const Double80 &x);
bool     isInfinity( const Double80 &x);

Double80 randDouble80(Random *rnd = NULL);
Double80 randDouble80(const Double80 &low, const Double80 &high, Random *rnd = NULL);

// dst must point to memory with at least 26 free char
char    *d80toa(char    *dst, const Double80 &x);
// dst must point to memory with at least 26 free wchar_t
wchar_t *d80tow(wchar_t *dst, const Double80 &x);

#ifdef _UNICODE
#define d80tot d80tow
#else
#define d80tot d80toa
#endif

String toString(      const Double80 &x, int precision=6, int width=0, int flags=0);

Double80 strtod80(const char    *s, char    **end);
Double80 wcstod80(const wchar_t *s, wchar_t **end);

#ifdef _UNICODE
#define _tcstod80 wcstod80
#else
#define _tcstod80 strtod80
#endif

istream &operator>>(istream &s,       Double80 &x);
ostream &operator<<(ostream &s, const Double80 &x);

std::wistream &operator>>(std::wistream &s,       Double80 &x);
std::wostream &operator<<(std::wostream &s, const Double80 &x);

StrStream &operator<<(StrStream &stream, const Double80 &x);
