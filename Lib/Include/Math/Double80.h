#pragma once

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
  DECLARECLASSNAME;
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
void   D80consI32(        Double80 &s  , const int      &x  );
void   D80consUI32(       Double80 &s  , UINT            x  );
void   D80consI64(        Double80 &s  , const INT64    &x  );
void   D80consUI64(       Double80 &s  , UINT64          x  );
void   D80consFloat(      Double80 &s  , const float    &x  );
void   D80consDouble(     Double80 &s  , const double   &x  );
int    D80ToI32(          const Double80 &x);
UINT   D80ToUI32(         const Double80 &x);
INT64  D80ToI64(          const Double80 &x);
UINT64 D80ToUI64(         const Double80 &x);
float  D80ToFloat(        const Double80 &x);
double D80ToDouble(       const Double80 &x);
int    D80cmp(            const Double80 &x, const Double80 &y);
int    D80cmpI32(         const Double80 &x, const int      &y);
int    D80cmpUI32(        const Double80 &x, UINT            y);
int    D80cmpI64(         const Double80 &x, const INT64    &y);
int    D80cmpUI64(        const Double80 &x, UINT64          y);
int    D80cmpFloat(       const Double80 &x, const float    &y);
int    D80cmpDouble(      const Double80 &x, const double   &y);
int    D80isZero(         const Double80 &x);
void   D80add(            Double80 &dst, const Double80 &x);
void   D80sub(            Double80 &dst, const Double80 &x);
void   D80mul(            Double80 &dst, const Double80 &x);
void   D80div(            Double80 &dst, const Double80 &x);
void   D80rem(            Double80 &dst, const Double80 &x);
void   D80neg(            Double80 &x);
void   D80addI32(         Double80 &dst, const int      &x);
void   D80subI32(         Double80 &dst, const int      &x);
void   D80mulI32(         Double80 &dst, const int      &x);
void   D80divI32(         Double80 &dst, const int      &x);
void   D80addUI32(        Double80 &dst, UINT            x);
void   D80subUI32(        Double80 &dst, UINT            x);
void   D80mulUI32(        Double80 &dst, UINT            x);
void   D80divUI32(        Double80 &dst, UINT            x);
void   D80addI64(         Double80 &dst, const INT64    &x);
void   D80subI64(         Double80 &dst, const INT64    &x);
void   D80mulI64(         Double80 &dst, const INT64    &x);
void   D80divI64(         Double80 &dst, const INT64    &x);
void   D80addUI64(        Double80 &dst, UINT64          x);
void   D80subUI64(        Double80 &dst, UINT64          x);
void   D80mulUI64(        Double80 &dst, UINT64          x);
void   D80divUI64(        Double80 &dst, UINT64          x);
void   D80addFloat(       Double80 &dst, const float    &x);
void   D80subFloat(       Double80 &dst, const float    &x);
void   D80mulFloat(       Double80 &dst, const float    &x);
void   D80divFloat(       Double80 &dst, const float    &x);
void   D80addDouble(      Double80 &dst, const double   &x);
void   D80subDouble(      Double80 &dst, const double   &x);
void   D80mulDouble(      Double80 &dst, const double   &x);
void   D80divDouble(      Double80 &dst, const double   &x);
void   D80inc(            Double80 &x);
void   D80dec(            Double80 &x);
void   D80getPi(          Double80 &dst);
void   D80getEps(         Double80 &dst);
void   D80getMin(         Double80 &dst);
void   D80getMax(         Double80 &dst);
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
void   D80sincos(         Double80 &c, Double80       &s); // inout is c, out s
void   D80exp(            Double80 &x);
void   D80log(            Double80 &x);
void   D80log10(          Double80 &x);
void   D80log2(           Double80 &x);
void   D80pow(            Double80 &x, const Double80 &y); // x = pow(x,y)
void   D80pow10(          Double80 &x);
void   D80pow2(           Double80 &x);
void   D80floor(          Double80 &x);
void   D80ceil(           Double80 &x);
void   D80ToBCD(BYTE bcd[10], const Double80 &src);
void   D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);
}
#endif // IS64BIT

#ifdef IS32BIT
extern const double   _Dmaxi32P1;
extern const Double80 _D80maxi64P1;
#endif // IS32BIT

class Double80 {
private:
  DECLARECLASSNAME;
  BYTE m_value[10]; // Must be the first field in the class
  void init(const _TUCHAR *s);

#ifdef IS32BIT
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
#endif // IS32BIT

public:
  inline Double80() {
  }

  inline Double80(long x) {
    *this = (Double80)(int)(x);
  }

  inline Double80(ULONG x) {
    *this = (Double80)(UINT)(x);
  }

#ifdef IS32BIT
  inline Double80(int x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  Double80(UINT x);
  inline Double80(INT64 x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  Double80(UINT64 x);
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
#else
  inline Double80(int x) {
    D80consI32(*this, x);
  }
  inline Double80(UINT x) {
    D80consUI32(*this, x);
  }
  inline Double80(INT64 x) {
    D80consI64(*this, x);
  }
  inline Double80(UINT64 x) {
    D80consUI64(*this, x);
  }
  inline Double80(float x) {
    D80consFloat(*this, x);
  }
  inline Double80(double x) {
    D80consDouble(*this, x);
  }
#endif // IS32BIT

  explicit inline Double80(const String &s) {
    init(s.cstr());
  }
  explicit Double80(const wchar_t *s);
  explicit Double80(const char    *s);
  explicit inline Double80(const BYTE *bytes) {
    memcpy(&m_value,bytes,sizeof(m_value));
  }

  inline friend long getLong(const Double80 &x) {
    return getInt(x);
  }

  inline friend ULONG getUlong(const Double80 &x) {
    return getUint(x);
  }

#ifdef IS32BIT
  friend int    getInt(   const Double80 &x);
  friend UINT   getUint(  const Double80 &x);
  friend INT64  getInt64( const Double80 &x);
  friend UINT64 getUint64(const Double80 &x);
  friend inline float getFloat(const Double80 &x) {
    float result;
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fstp result
    }
    return result;
  }
  friend inline double getDouble(const Double80 &x) {
    double result;
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fstp result
    }
    return result;
  }
  static int    getExpo2( const Double80 &x);
  static int    getExpo10(const Double80 &x); // x == 0 ? 0 : floor(log10(|x|))

  inline Double80 operator+(const Double80 &x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      fadd
      fstp result
    }
    return result;
  }
  inline Double80 operator-(const Double80 &x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      fsub
      fstp result
    }
    return result;
  }
  inline Double80 operator-() const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fchs
      fstp result
    }
    return result;
  }
  inline Double80 operator*(const Double80 &x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      fmul
      fstp result
    }
    return result;
  }
  inline Double80 operator/(const Double80 &x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      fdiv
      fstp result
    }
    return result;
  }

  inline Double80 operator+(int x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fiadd x
      fstp result
    }
    return result;
  }
  inline Double80 operator-(int x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fisub x
      fstp result
    }
    return result;
  }
  inline Double80 operator*(int x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fimul x
      fstp result
    }
    return result;
  }
  inline Double80 operator/(int x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fidiv x
      fstp result
    }
    return result;
  }
  Double80 operator+(UINT x) const;
  Double80 operator-(UINT x) const;
  Double80 operator*(UINT x) const;
  Double80 operator/(UINT x) const;
  inline Double80 operator+(long x) const {
    return *this + (int)x;
  }
  inline Double80 operator-(long x) const {
    return *this - (int)x;
  }
  inline Double80 operator*(long x) const {
    return *this * (int)x;
  }
  inline Double80 operator/(long x) const {
    return *this / (int)x;
  }
  inline Double80 operator+(ULONG x) const {
    return *this + (UINT)x;
  }
  inline Double80 operator-(ULONG x) const {
    return *this - (UINT)x;
  }
  inline Double80 operator*(ULONG x) const {
    return *this * (UINT)x;
  }
  inline Double80 operator/(ULONG x) const {
    return *this / (UINT)x;
  }

  inline Double80 operator+(INT64 x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fadd
      fstp result
    }
    return result;
  }
  inline Double80 operator-(INT64 x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fsub
      fstp result
    }
    return result;
  }
  inline Double80 operator*(INT64 x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fmul
      fstp result
    }
    return result;
  }
  inline Double80 operator/(INT64 x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fdiv
      fstp result
    }
    return result;
  }

  Double80 operator+(const UINT64 &) const;
  Double80 operator-(const UINT64 &) const;
  Double80 operator*(const UINT64 &) const;
  Double80 operator/(const UINT64 &) const;

  inline Double80 operator+(float x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fadd x
      fstp result
    }
    return result;
  }
  inline Double80 operator-(float x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fsub x
      fstp result
    }
    return result;
  }
  inline Double80 operator*(float x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fmul x
      fstp result
    }
    return result;
  }
  inline Double80 operator/(float x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fdiv x
      fstp result
    }
    return result;
  }

  inline Double80 operator+(double x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fadd x
      fstp result
    }
    return result;
  }
  inline Double80 operator-(double x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fsub x
      fstp result
    }
    return result;
  }
  inline Double80 operator*(double x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fmul x
      fstp result
    }
    return result;
  }
  inline Double80 operator/(double x) const {
    Double80 result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fdiv x
      fstp result
    }
    return result;
  }

  inline Double80 &operator+=(const Double80 &x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov ebx, DWORD PTR x
      fld TBYTE PTR [ebx]
      fadd
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator-=(const Double80 &x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov ebx, DWORD PTR x
      fld TBYTE PTR [ebx]
      fsub
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator*=(const Double80 &x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov ebx, DWORD PTR x
      fld TBYTE PTR [ebx]
      fmul
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator/=(const Double80 &x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      mov ebx, DWORD PTR x
      fld TBYTE PTR [ebx]
      fdiv
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator+=(int x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fiadd x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator-=(int x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fisub x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator*=(int x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fimul x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator/=(int x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fidiv x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  Double80 &operator+=(UINT x);
  Double80 &operator-=(UINT x);
  Double80 &operator*=(UINT x);
  Double80 &operator/=(UINT x);
  inline Double80 &operator+=(long x) {
    return operator+=((int)x);
  }
  inline Double80 &operator-=(long x) {
    return operator-=((int)x);
  }
  inline Double80 &operator*=(long x) {
    return operator*=((int)x);
  }
  inline Double80 &operator/=(long x) {
    return operator/=((int)x);
  }
  Double80 &operator+=(ULONG x) {
    return operator+=((UINT)x);
  }
  Double80 &operator-=(ULONG x) {
    return operator-=((UINT)x);
  }
  Double80 &operator*=(ULONG x) {
    return operator*=((UINT)x);
  }
  Double80 &operator/=(ULONG x) {
    return operator/=((UINT)x);
  }
  inline Double80 &operator+=(INT64 x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fadd
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator-=(INT64 x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fsub
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator*=(INT64 x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fmul
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator/=(INT64 x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fild x
      fdiv
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  Double80 &operator+=(const UINT64 &x);
  Double80 &operator-=(const UINT64 &x);
  Double80 &operator*=(const UINT64 &x);
  Double80 &operator/=(const UINT64 &x);
  inline Double80 &operator+=(float x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fadd x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator-=(float x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fsub x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator*=(float x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fmul x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator/=(float x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fdiv x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator+=(double x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fadd x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator-=(double x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fsub x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator*=(double x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fmul x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator/=(double x) {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fdiv x
      fstp TBYTE PTR [eax]
    }
    return *this;
  }

  inline Double80 &operator++() {   // prefix-form
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fadd
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 &operator--() {   // prefix-form
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fsub
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  inline Double80 operator++(int) { // postfix-form
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
  inline Double80 operator--(int) { // postfix-form
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

  bool isZero()     const;

  bool operator==(const Double80 &x) const;
  bool operator!=(const Double80 &x) const;
  bool operator<=(const Double80 &x) const;
  bool operator>=(const Double80 &x) const;
  bool operator< (const Double80 &x) const;
  bool operator> (const Double80 &x) const;

  bool operator==(int             x) const;
  bool operator!=(int             x) const;
  bool operator<=(int             x) const;
  bool operator>=(int             x) const;
  bool operator< (int             x) const;
  bool operator> (int             x) const;

  bool operator==(UINT            x) const;
  bool operator!=(UINT            x) const;
  bool operator<=(UINT            x) const;
  bool operator>=(UINT            x) const;
  bool operator< (UINT            x) const;
  bool operator> (UINT            x) const;

  inline bool operator==(long     x) const {
    return operator==((int)x);
  }
  inline bool operator!=(long     x) const {
    return operator!=((int)x);
  }
  inline bool operator<=(long     x) const {
    return operator<=((int)x);
  }
  inline bool operator>=(long     x) const {
    return operator>=((int)x);
  }
  inline bool operator< (long     x) const {
    return operator<((int)x);
  }
  inline bool operator> (long     x) const {
    return operator>((int)x);
  }
  inline bool operator==(ULONG    x) const {
    return operator==((UINT)x);
  }
  inline bool operator!=(ULONG    x) const {
    return operator!=((UINT)x);
  }
  inline bool operator<=(ULONG    x) const {
    return operator<=((UINT)x);
  }
  inline bool operator>=(ULONG    x) const {
    return operator>=((UINT)x);
  }
  inline bool operator< (ULONG    x) const {
    return operator<((UINT)x);
  }
  inline bool operator> (ULONG    x) const {
    return operator>((UINT)x);
  }
  bool operator==(const INT64    &x) const;
  bool operator!=(const INT64    &x) const;
  bool operator<=(const INT64    &x) const;
  bool operator>=(const INT64    &x) const;
  bool operator< (const INT64    &x) const;
  bool operator> (const INT64    &x) const;

  bool operator==(const UINT64   &x) const;
  bool operator!=(const UINT64   &x) const;
  bool operator<=(const UINT64   &x) const;
  bool operator>=(const UINT64   &x) const;
  bool operator< (const UINT64   &x) const;
  bool operator> (const UINT64   &x) const;

  bool operator==(float           x) const;
  bool operator!=(float           x) const;
  bool operator<=(float           x) const;
  bool operator>=(float           x) const;
  bool operator< (float           x) const;
  bool operator> (float           x) const;

  bool operator==(const double   &x) const;
  bool operator!=(const double   &x) const;
  bool operator<=(const double   &x) const;
  bool operator>=(const double   &x) const;
  bool operator< (const double   &x) const;
  bool operator> (const double   &x) const;

#else // !IS32BIT (ie IS64BIT)

  friend inline int getInt(const Double80 &x) {
    return D80ToI32(x);
  }

  friend inline UINT getUint(const Double80 &x) {
    return D80ToUI32(x);
  }

  friend inline INT64 getInt64(const Double80 &x) {
    return D80ToI64(x);
  }

  friend inline UINT64 getUint64(const Double80 &x) {
    return D80ToUI64(x);
  }

  friend inline float getFloat(const Double80 &x) {
    return D80ToFloat(x);
  }

  friend inline double getDouble(const Double80 &x) {
    return D80ToDouble(x);
  }

  static inline int getExpo2(const Double80 &x) {
    return D80getExpo2(x);
  }

  static inline int getExpo10(const Double80 &x) { // x == 0 ? 0 : floor(log10(|x|))
    return D80getExpo10(x);
  }

  inline Double80 operator+(const Double80 &x) const {
    Double80 tmp(*this);
    D80add(tmp, x);
    return tmp;
  }
  inline Double80 operator-(const Double80 &x) const {
    Double80 tmp(*this);
    D80sub(tmp, x);
    return tmp;
  }
  inline Double80 operator-() const {
    Double80 tmp(*this);
    D80neg(tmp);
    return tmp;
  }
  inline Double80 operator*(const Double80 &x) const {
    Double80 tmp(*this);
    D80mul(tmp, x);
    return tmp;
  }
  inline Double80 operator/(const Double80 &x) const {
    Double80 tmp(*this);
    D80div(tmp, x);
    return tmp;
  }

  inline Double80 operator+(int x) const {
    Double80 tmp(*this);
    D80addI32(tmp, x);
    return tmp;
  }
  inline Double80 operator-(int x) const {
    Double80 tmp(*this);
    D80subI32(tmp, x);
    return tmp;
  }
  inline Double80 operator*(int x) const {
    Double80 tmp(*this);
    D80mulI32(tmp, x);
    return tmp;
  }
  inline Double80 operator/(int x) const {
    Double80 tmp(*this);
    D80divI32(tmp, x);
    return tmp;
  }

  inline Double80 operator+(UINT x) const {
    Double80 tmp(*this);
    D80addUI32(tmp, x);
    return tmp;
  }
  inline Double80 operator-(UINT x) const {
    Double80 tmp(*this);
    D80subUI32(tmp, x);
    return tmp;
  }
  inline Double80 operator*(UINT x) const {
    Double80 tmp(*this);
    D80mulUI32(tmp, x);
    return tmp;
  }
  inline Double80 operator/(UINT x) const {
    Double80 tmp(*this);
    D80divUI32(tmp, x);
    return tmp;
  }

  inline Double80 operator+(long x) const {
    Double80 tmp(*this);
    D80addI32(tmp, (int&)x);
    return tmp;
  }
  inline Double80 operator-(long x) const {
    Double80 tmp(*this);
    D80subI32(tmp, (int&)x);
    return tmp;
  }
  inline Double80 operator*(long x) const {
    Double80 tmp(*this);
    D80mulI32(tmp, (int&)x);
    return tmp;
  }
  inline Double80 operator/(long x) const {
    Double80 tmp(*this);
    D80divI32(tmp, (int&)x);
    return tmp;
  }

  inline Double80 operator+(ULONG x) const {
    Double80 tmp(*this);
    D80addUI32(tmp, x);
    return tmp;
  }
  inline Double80 operator-(ULONG x) const {
    Double80 tmp(*this);
    D80subUI32(tmp, x);
    return tmp;
  }
  inline Double80 operator*(ULONG x) const {
    Double80 tmp(*this);
    D80mulUI32(tmp, x);
    return tmp;
  }
  inline Double80 operator/(ULONG x) const {
    Double80 tmp(*this);
    D80divUI32(tmp, x);
    return tmp;
  }

  inline Double80 operator+(INT64 x) const {
    Double80 tmp(*this);
    D80addI64(tmp, x);
    return tmp;
  }
  inline Double80 operator-(INT64 x) const {
    Double80 tmp(*this);
    D80subI64(tmp, x);
    return tmp;
  }
  inline Double80 operator*(INT64 x) const {
    Double80 tmp(*this);
    D80mulI64(tmp, x);
    return tmp;
  }
  inline Double80 operator/(INT64 x) const {
    Double80 tmp(*this);
    D80divI64(tmp, x);
    return tmp;
  }

  inline Double80 operator+(UINT64 x) const {
    Double80 tmp(*this);
    D80addUI64(tmp, x);
    return tmp;
  }
  inline Double80 operator-(UINT64 x) const {
    Double80 tmp(*this);
    D80subUI64(tmp, x);
    return tmp;
  }
  inline Double80 operator*(UINT64 x) const {
    Double80 tmp(*this);
    D80mulUI64(tmp, x);
    return tmp;
  }
  inline Double80 operator/(UINT64 x) const {
    Double80 tmp(*this);
    D80divUI64(tmp, x);
    return tmp;
  }

  inline Double80 operator+(float x) const {
    Double80 tmp(*this);
    D80addFloat(tmp, x);
    return tmp;
  }
  inline Double80 operator-(float x) const {
    Double80 tmp(*this);
    D80subFloat(tmp, x);
    return tmp;
  }
  inline Double80 operator*(float x) const {
    Double80 tmp(*this);
    D80mulFloat(tmp, x);
    return tmp;
  }
  inline Double80 operator/(float x) const {
    Double80 tmp(*this);
    D80divFloat(tmp, x);
    return tmp;
  }

  inline Double80 operator+(double x) const {
    Double80 tmp(*this);
    D80addDouble(tmp, x);
    return tmp;
  }
  inline Double80 operator-(double x) const {
    Double80 tmp(*this);
    D80subDouble(tmp, x);
    return tmp;
  }
  inline Double80 operator*(double x) const {
    Double80 tmp(*this);
    D80mulDouble(tmp, x);
    return tmp;
  }
  inline Double80 operator/(double x) const {
    Double80 tmp(*this);
    D80divDouble(tmp, x);
    return tmp;
  }

  inline Double80 &operator+=(const Double80 &x) {
    D80add(*this, x);
    return *this;
  }
  inline Double80 &operator-=(const Double80 &x) {
    D80sub(*this, x);
    return *this;
  }
  inline Double80 &operator*=(const Double80 &x) {
    D80mul(*this, x);
    return *this;
  }
  inline Double80 &operator/=(const Double80 &x) {
    D80div(*this, x);
    return *this;
  }

  inline Double80 &operator+=(int x) {
    D80addI32(*this, x);
    return *this;
  }
  inline Double80 &operator-=(int x) {
    D80subI32(*this, x);
    return *this;
  }
  inline Double80 &operator*=(int x) {
    D80mulI32(*this, x);
    return *this;
  }
  inline Double80 &operator/=(int x) {
    D80divI32(*this, x);
    return *this;
  }

  inline Double80 &operator+=(UINT x) {
    D80addUI32(*this, x);
    return *this;
  }
  inline Double80 &operator-=(UINT x) {
    D80subUI32(*this, x);
    return *this;
  }
  inline Double80 &operator*=(UINT x) {
    D80mulUI32(*this, x);
    return *this;
  }
  inline Double80 &operator/=(UINT x) {
    D80divUI32(*this, x);
    return *this;
  }

  inline Double80 &operator+=(long x) {
    D80addI32(*this, (int&)x);
    return *this;
  }
  inline Double80 &operator-=(long x) {
    D80subI32(*this, (int&)x);
    return *this;
  }
  inline Double80 &operator*=(long x) {
    D80mulI32(*this, (int&)x);
    return *this;
  }
  inline Double80 &operator/=(long x) {
    D80divI32(*this, (int&)x);
    return *this;
  }

  inline Double80 &operator+=(ULONG x) {
    D80addUI32(*this, x);
    return *this;
  }
  inline Double80 &operator-=(ULONG x) {
    D80subUI32(*this, x);
    return *this;
  }
  inline Double80 &operator*=(ULONG x) {
    D80mulUI32(*this, x);
    return *this;
  }
  inline Double80 &operator/=(ULONG x) {
    D80divUI32(*this, x);
    return *this;
  }

  inline Double80 &operator+=(INT64 x) {
    D80addI64(*this, x);
    return *this;
  }
  inline Double80 &operator-=(INT64 x) {
    D80subI64(*this, x);
    return *this;
  }
  inline Double80 &operator*=(INT64 x) {
    D80mulI64(*this, x);
    return *this;
  }
  inline Double80 &operator/=(INT64 x) {
    D80divI64(*this, x);
    return *this;
  }

  inline Double80 &operator+=(UINT64 x) {
    D80addUI64(*this, x);
    return *this;
  }
  inline Double80 &operator-=(UINT64 x) {
    D80subUI64(*this, x);
    return *this;
  }
  inline Double80 &operator*=(UINT64 x) {
    D80mulUI64(*this, x);
    return *this;
  }
  inline Double80 &operator/=(UINT64 x) {
    D80divUI64(*this, x);
    return *this;
  }

  inline Double80 &operator+=(float x) {
    D80addFloat(*this, x);
    return *this;
  }
  inline Double80 &operator-=(float x) {
    D80subFloat(*this, x);
    return *this;
  }
  inline Double80 &operator*=(float x) {
    D80mulFloat(*this, x);
    return *this;
  }
  inline Double80 &operator/=(float x) {
    D80divFloat(*this, x);
    return *this;
  }

  inline Double80 &operator+=(double x) {
    D80addDouble(*this, x);
    return *this;
  }
  inline Double80 &operator-=(double x) {
    D80subDouble(*this, x);
    return *this;
  }
  inline Double80 &operator*=(double x) {
    D80mulDouble(*this, x);
    return *this;
  }
  inline Double80 &operator/=(double x) {
    D80divDouble(*this, x);
    return *this;
  }

  inline Double80 &operator++() {   // prefix-form
    D80inc(*this);
    return *this;
  }
  inline Double80 &operator--() {   // prefix-form
    D80dec(*this);
    return *this;
  }
  inline Double80 operator++(int) { // postfix-form
    Double80 result(*this);
    D80inc(*this);
    return result;
  }
  inline Double80 operator--(int) { // postfix-form
    Double80 result(*this);
    D80dec(*this);
    return result;
  }

  inline bool isZero() const {
    return D80isZero(*this) ? true : false;
  }

  inline bool operator==(const Double80 &x) const {
    return D80cmp(*this, x) == 0;
  }
  inline bool operator!=(const Double80 &x) const {
    return D80cmp(*this, x) != 0;
  }
  inline bool operator<=(const Double80 &x) const {
    return D80cmp(*this, x) <= 0;
  }
  inline bool operator>=(const Double80 &x) const {
    return D80cmp(*this, x) >= 0;
  }
  inline bool operator< (const Double80 &x) const {
    return D80cmp(*this, x) < 0;
  }
  inline bool operator> (const Double80 &x) const {
    return D80cmp(*this, x) > 0;
  }

  inline bool operator==(const int &x) const {
    return D80cmpI32(*this, x) == 0;
  }
  inline bool operator!=(const int &x) const {
    return D80cmpI32(*this, x) != 0;
  }
  inline bool operator<=(const int &x) const {
    return D80cmpI32(*this, x) <= 0;
  }
  inline bool operator>=(const int &x) const {
    return D80cmpI32(*this, x) >= 0;
  }
  inline bool operator< (const int &x) const {
    return D80cmpI32(*this, x) < 0;
  }
  inline bool operator> (const int &x) const {
    return D80cmpI32(*this, x) > 0;
  }

  inline bool operator==(const UINT x) const {
    return D80cmpUI32(*this, x) == 0;
  }
  inline bool operator!=(const UINT x) const {
    return D80cmpUI32(*this, x) != 0;
  }
  inline bool operator<=(const UINT x) const {
    return D80cmpUI32(*this, x) <= 0;
  }
  inline bool operator>=(const UINT x) const {
    return D80cmpUI32(*this, x) >= 0;
  }
  inline bool operator< (const UINT x) const {
    return D80cmpUI32(*this, x) < 0;
  }
  inline bool operator> (const UINT x) const {
    return D80cmpUI32(*this, x) > 0;
  }

  inline bool operator==(const long &x) const {
    return D80cmpI32(*this, (const int&)x) == 0;
  }
  inline bool operator!=(const long &x) const {
    return D80cmpI32(*this, (const int&)x) != 0;
  }
  inline bool operator<=(const long &x) const {
    return D80cmpI32(*this, (const int&)x) <= 0;
  }
  inline bool operator>=(const long &x) const {
    return D80cmpI32(*this, (const int&)x) >= 0;
  }
  inline bool operator< (const long &x) const {
    return D80cmpI32(*this, (const int&)x) < 0;
  }
  inline bool operator> (const long &x) const {
    return D80cmpI32(*this, (const int&)x) > 0;
  }

  inline bool operator==(const ULONG x) const {
    return D80cmpUI32(*this, x) == 0;
  }
  inline bool operator!=(const ULONG x) const {
    return D80cmpUI32(*this, x) != 0;
  }
  inline bool operator<=(const ULONG x) const {
    return D80cmpUI32(*this, x) <= 0;
  }
  inline bool operator>=(const ULONG x) const {
    return D80cmpUI32(*this, x) >= 0;
  }
  inline bool operator< (const ULONG x) const {
    return D80cmpUI32(*this, x) < 0;
  }
  inline bool operator> (const ULONG x) const {
    return D80cmpUI32(*this, x) > 0;
  }

  inline bool operator==(const INT64 &x) const {
    return D80cmpI64(*this, x) == 0;
  }
  inline bool operator!=(const INT64 &x) const {
    return D80cmpI64(*this, x) != 0;
  }
  inline bool operator<=(const INT64 &x) const {
    return D80cmpI64(*this, x) <= 0;
  }
  inline bool operator>=(const INT64 &x) const {
    return D80cmpI64(*this, x) >= 0;
  }
  inline bool operator< (const INT64 &x) const {
    return D80cmpI64(*this, x) < 0;
  }
  inline bool operator> (const INT64 &x) const {
    return D80cmpI64(*this, x) > 0;
  }

  inline bool operator==(const UINT64 x) const {
    return D80cmpUI64(*this, x) == 0;
  }
  inline bool operator!=(const UINT64 x) const {
    return D80cmpUI64(*this, x) != 0;
  }
  inline bool operator<=(const UINT64 x) const {
    return D80cmpUI64(*this, x) <= 0;
  }
  inline bool operator>=(const UINT64 x) const {
    return D80cmpUI64(*this, x) >= 0;
  }
  inline bool operator< (const UINT64 x) const {
    return D80cmpUI64(*this, x) < 0;
  }
  inline bool operator> (const UINT64 x) const {
    return D80cmpUI64(*this, x) > 0;
  }

  inline bool operator==(const float &x) const {
    return D80cmpFloat(*this, x) == 0;
  }
  inline bool operator!=(const float &x) const {
    return D80cmpFloat(*this, x) != 0;
  }
  inline bool operator<=(const float &x) const {
    return D80cmpFloat(*this, x) <= 0;
  }
  inline bool operator>=(const float &x) const {
    return D80cmpFloat(*this, x) >= 0;
  }
  inline bool operator< (const float &x) const {
    return D80cmpFloat(*this, x) < 0;
  }
  inline bool operator> (const float &x) const {
    return D80cmpFloat(*this, x) > 0;
  }

  inline bool operator==(const double &x) const {
    return D80cmpDouble(*this, x) == 0;
  }
  inline bool operator!=(const double &x) const {
    return D80cmpDouble(*this, x) != 0;
  }
  inline bool operator<=(const double &x) const {
    return D80cmpDouble(*this, x) <= 0;
  }
  inline bool operator>=(const double &x) const {
    return D80cmpDouble(*this, x) >= 0;
  }
  inline bool operator< (const double &x) const {
    return D80cmpDouble(*this, x) < 0;
  }
  inline bool operator> (const double &x) const {
    return D80cmpDouble(*this, x) > 0;
  }

#endif // IS32BIT

  static TCHAR   *d80tot(TCHAR   *dst, const Double80 &x); // dst must point to memory with at least 26 free TCHAR
  static char    *d80toa(char    *dst, const Double80 &x); // dst must point to memory with at least 26 free char
  static wchar_t *d80tow(wchar_t *dst, const Double80 &x); // dst must point to memory with at least 26 free wchar_t

  inline bool isPositive() const { return (m_value[9] & 0x80) == 0; }
  inline bool isNegative() const { return (m_value[9] & 0x80) != 0; }

  ULONG hashCode() const;

  inline String toString() const {
    TCHAR tmp[30];
    return d80tot(tmp, *this);
  }

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
  static const int      DBL80_DIG;     // # of decimal digits of precision

  static void initClass();
};

#ifdef IS32BIT

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
void     sincos(Double80 &c, Double80 &s); // calculate both cos and sin. c:inout c, s:out
Double80 exp(  const Double80 &x);
Double80 log(  const Double80 &x);
Double80 log10(const Double80 &x);
Double80 log2( const Double80 &x);
Double80 pow(  const Double80 &x, const Double80 &y);
Double80 pow10(const Double80 &x);
Double80 pow2( const Double80 &x);
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

#else // !IS32BIT (ie IS64BIT)

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
inline void sincos(Double80 &c, Double80 &s) { // calculate both cos and sin. c:inout c, s:out
  D80sincos(c, s);
}
inline Double80 exp(const Double80 &x) {
  Double80 tmp(x);
  D80exp(tmp);
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
inline Double80 pow10(const Double80 &x) {
  Double80 tmp(x);
  D80pow10(tmp);
  return tmp;
}
inline Double80 pow2(const Double80 &x) {
  Double80 tmp(x);
  D80pow2(tmp);
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

#endif // IS32BIT

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

String toString(      const Double80 &x, int precision=6, int width=0, int flags=0);

tistream &operator>>(tistream &s,       Double80 &x);
tostream &operator<<(tostream &s, const Double80 &x);

StrStream &operator<<(StrStream &stream, const Double80 &x);
