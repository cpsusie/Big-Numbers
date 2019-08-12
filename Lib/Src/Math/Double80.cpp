#include "pch.h"
#include <Math/Double80.h>
#include <Math/FPU.h>

#pragma check_stack(off)
#pragma warning(disable : 4073)
#pragma init_seg(lib)

// define this, to get a dump of all possible values returned by Double80::pow10
// will be disabled when compile for Win32, release, as it is used in NumberAddin, where we dont want this to be done!!
#define _DUMPPOW10

#ifdef IS32BIT
#ifndef _DEBUG
#ifdef _DUMPPOW10
#undef _DUMPPOW10
#endif
#endif
#endif

class InitDouble80 {
public:
  inline InitDouble80() {
    Double80::initClass();
  }
};

void Double80::initClass() {
  if(sizeof(Double80) != 10) {
    throwException(_T("%s:Size of Double80 must be 10. Size=%zu."), __TFUNCTION__, sizeof(Double80));
  }
  FPU::init();
  FPU::clearExceptions();
  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
}

static InitDouble80 initDouble80;

const Double80 Double80::_0( 0  );
const Double80 Double80::_05(0.5);
const Double80 Double80::_1(1);
const Double80 Double80::_DBL80_EPSILON ((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\xc0\x3f"); // 1.08420217248550443e-019;
const Double80 Double80::_DBL80_MIN     ((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\x01\x00"); // 3.36210314311209209e-4932;
const Double80 Double80::_DBL80_MAX     ((BYTE*)"\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x7f"); // 1.18973149535723227e+4932
const Double80 Double80::_DBL80_QNAN    ((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\xc0\xff\xff"); // quiet NaN
const Double80 Double80::_DBL80_SNAN    ((BYTE*)"\x00\x08\x00\x00\x00\x00\x00\x80\xff\xff"); // signaling NaN
const Double80 Double80::_DBL80_PINF    ((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\xff\x7f"); // +infinity;
const Double80 Double80::_DBL80_TRUE_MIN((BYTE*)"\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00"); // denormalized minimal positive value
const Double80            DBL80_PI(      (BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\x00\x40"); // pi
const Double80            DBL80_PI_05(   (BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\xff\x3f"); // pi/2

int _fpclass(const Double80 &x) {
  switch(getExponent(x)) {
  case 0     :
    { const UINT64 s = getSignificand(x);
      if(s == 0) {
        return x.isPositive() ? _FPCLASS_PZ : _FPCLASS_NZ;
      } else {
        return x.isPositive() ? _FPCLASS_PD : _FPCLASS_ND;
      }
    }
  case 0x7fff: // NaN -> INFINITY and INDEFINITE
    { const UINT64 s = getSignificand(x);
      if(s == (1ui64 << 63)) { // +/-INFINITY
        return x.isPositive() ? _FPCLASS_PINF: _FPCLASS_NINF;
      } // either snan or qnan. snan if signbit=1 and f0=1, f1=0 and at least one of {f2..f63} = 1
      return (((s>>62) == 2) && (s & 0x3fffffffffffffffui64) && getSign(x)) ? _FPCLASS_SNAN : _FPCLASS_QNAN;
    }
  }
  return x.isPositive() ? _FPCLASS_PN : _FPCLASS_NN;
}

int fpclassify(const Double80 &v) {
  switch(getExponent(v)) {
  case 0x7fff:
    return (getSignificand(v) == (1ui64<<63)) ? FP_INFINITE : FP_NAN;
  case 0:
    return (getSignificand(v) == 0) ? FP_ZERO : FP_SUBNORMAL;
  default:
    return FP_NORMAL;
  }
}

#define MINPOW10 -4932
#define MAXPOW10 4932
template<int lowSize> class RawPow10Cache {
private:
  Double80 m_pow10L[lowSize], m_pow10H[MAXPOW10 / lowSize + 1];
public:
  RawPow10Cache() {
    Double80 p = 1;
    const FPUControlWord oldcw = FPU::getControlWord();
    FPUControlWord       newcw = oldcw;
    newcw.adjustExceptionMask(0, -1)
         .setRoundMode(FPU_ROUNDCONTROL_ROUND)
         .setPrecisionMode(FPU_HIGH_PRECISION);
    FPU::setControlWord(newcw);

    for(int i = 0; i < ARRAYSIZE(m_pow10L); i++, p *= 10) {
      m_pow10L[i] = p;
    }
    Double80 p1 = 1;
    for(int i = 0; i < ARRAYSIZE(m_pow10H); i++, p1 *= p) {
      m_pow10H[i] = p1;
    }
    FPU::clearExceptionsNoWait();
    FPU::restoreControlWord(oldcw);
  }

  Double80 pow10(int p) const {
    if(p < 0) {
      return Double80::_1 / pow10(-p);
    } else {
      const int l = p % ARRAYSIZE(m_pow10L), h = p / ARRAYSIZE(m_pow10L);
      if(h) {
        if(h >= ARRAYSIZE(m_pow10H)) {
          return DBL80_MAX;
        }
        return l ? (m_pow10H[h] * m_pow10L[l]) : m_pow10H[h];
      } else { // h == 0
        return m_pow10L[l];
      }
    }
    throwInvalidArgumentException(__TFUNCTION__, _T("p=%d"), p);
    return 0;
  }
};

class Pow10Cache {
private:
  RawPow10Cache<16> m_c1;
  RawPow10Cache<26> m_c2;

public:
  Double80 pow10(int p) const;

#ifdef _DUMPPOW10
  void dumpAll() const;
#endif _DUMPPOW10
};

static const Double80     tenE18(  1e18    );
static const Double80     tenE18M1(tenE18-1);

Double80 Double80::pow10(int p) {
  static const Pow10Cache p10Cache;
#ifdef _DUMPPOW10
  static bool s_isDumping = false;
  if(!s_isDumping) {
    s_isDumping = true; // to prevent infinite recursion
    p10Cache.dumpAll();
  }
#endif _DUMPPOW10
  return p10Cache.pow10(p);
}

#ifdef IS32BIT
static const float ten(10.0f);
const double   _Dmaxi16P1   = ((UINT)_I16_MAX + 1);
const double   _Dmaxi32P1   = ((UINT)_I32_MAX + 1);
const Double80 _D80maxi64(  (BYTE*)"\xfe\xff\xff\xff\xff\xff\xff\xff\x3d\x40");            // _I64_MAX;
const Double80 _D80maxi64P1((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\x3e\x40");            // (UINT64)_I64_MAX + 1
static const Double80     M_2PiExp260 ((BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\x3d\x40"); // 2pi*exp2(60) (=7.244019458077122e+018)
#endif  // IS32BIT

#ifdef IS32BIT

int Double80::getExpo10(const Double80 &x) { // static
  USHORT cwSave,ctrlFlags;
  int result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fldz
    fcomip st, st(1)            // compare x and pop 0
    jne x_not_zero              // if(x != 0) goto x_not_zero
    fstp st(0)                  // pop x
    mov result, 0               // x == 0 => result = 0
    jmp Exit
x_not_zero:
    fabs
    fldlg2
    fxch st(1)
    fyl2x
    fnstcw cwSave
    mov ax, cwSave
    or  ax, 0x400               // set bit 10
    and ax, 0xf7ff              // clear bit 11
    mov ctrlFlags, ax           // FPU.ctrlWorld.bit[10;11] = 1,0 = ROUND DOWN
    fldcw ctrlFlags
    fistp result
    fldcw cwSave
Exit:
  }
  return result;
}

UINT getUint(const Double80 &x) {
  UINT result;
  if(x > _I32_MAX) {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fsub _Dmaxi32P1
      fisttp result
    }
    result += (UINT)_I32_MAX + 1;
  } else if(x < -_I32_MAX) {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fchs
      fsub _Dmaxi32P1
      fisttp result
    }
    result = -(int)result;
    result -= (UINT)_I32_MAX + 1;
  } else {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fisttp result
    }
  }
  return result;
}

UINT64 getUint64(const Double80 &x) {
  UINT64 result;
  if(x > _D80maxi64) {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fld _D80maxi64P1
      fsub
      fisttp result;
    }
    if(result <= (UINT64)_I64_MAX) {
      result += (UINT64)_I64_MAX + 1;
    }
  } else {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fisttp result;
    }
  }
  return result;
}

Double80 fmod(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]         //                                                    st0=y
    fabs                        // y = abs(y)                                         st0=|y|
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]         //                                                    st0=x,st1=|y|
    fldz                        //                                                    st0=0,st1=x,st2=|y|
    fcomip st, st(1)            // compare and pop zero                               st0=x,st1=|y|
    ja RepeatNegativeX          // if st(0) > st(1) (0 > x) goto repeat_negative_x
RepeatPositiveX:                // do {                                               st0=x,st1=|y|, x > 0
    fprem                       //   st0 %= y
    fstsw ax
    sahf
    jpe RepeatPositiveX         // } while(statusword.c2 != 0);
    fldz                        //                                                    st0=0,st1=x,st2=|y|
    fcomip st, st(1)            // compare and pop zero
    jbe pop2                    // if(st(0) <= st(1) (0 <= remainder) goto pop2
    fadd                        // remainder += y
    fstp result                 // pop result
    jmp Exit                    // goto end
RepeatNegativeX:                // do {                                               st0=x,st=|y|, x < 0
    fprem                       //   st0 %= y
    fstsw ax
    sahf
    jpe RepeatNegativeX         // } while(statusword.c2 != 0)
    fldz
    fcomip st, st(1)            // compare and pop zero
    jae pop2                    // if(st(0) >= st(1) (0 >= remainder) goto pop2
    fsubr                       // remainder -= y
    fstp result                 // pop result
    jmp Exit                    // goto end

pop2:                           //                                                    st0=x%y,st1=y
    fstp result                 // pop result
    fstp st(0)                  // pop y
Exit:
  }
  return result;
}

Double80 sin(const Double80 &x) {
  Double80 result = fmod(x,M_2PiExp260);
  __asm {
    fld result
    fsin
    fstp result
  }
  return result;
}

Double80 cos(const Double80 &x) {
  Double80 result = fmod(x,M_2PiExp260);
  __asm {
    fld result
    fcos
    fstp result
  }
  return result;
}

Double80 tan(const Double80 &x) {
  Double80 result = fmod(x,M_2PiExp260);
  __asm {
    fld result
    fptan
    fstp result
    fstp result
  }
  return result;
}

void sincos(Double80 &c, Double80 &s) { // calculate both cos and sin. c:inout c, s:out
  Double80 r = fmod(c,M_2PiExp260);
  __asm {
    fld r
    fsincos
    mov eax, DWORD PTR c
    fstp TBYTE PTR [eax]
    mov eax, DWORD PTR s
    fstp TBYTE PTR [eax]
  }
}

Double80 exp(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fldl2e
    fmul
    fld st(0)
    frndint
    fsub st(1),st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  return result;
}

Double80 exp10(const Double80 &x) {
  if(x.isZero()) {
    return Double80::_1;
  }

  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fldl2t
    fmul
    fld st(0)
    frndint
    fsub st(1),st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  return result;
}

Double80 exp2(const Double80 &x) {
  if(x.isZero()) {
    return Double80::_1;
  }
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fld st(0)
    frndint
    fsub st(1),st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

Double80 pow(const Double80 &x, const Double80 &y) {
  if(y.isZero()) {
    return Double80::_1;
  }
  if(x.isZero()) {
    return y.isNegative() ? (Double80::_1 / Double80::_0) : Double80::_0;
  }

  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fld st(0)
    frndint
    fsub st(1),st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  return result;
}

Double80 floor(const Double80 &x) {
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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

Double80 ceil(const Double80 &x) {
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDUP);
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

void D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10) {
  const FPUControlWord  cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
  __asm {
      mov edx, DWORD PTR expo10   // edx = &expo10
      mov eax, DWORD PTR[edx]
      cmp eax, 0
      jne ScaleX

      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      jmp Rescale

  ScaleX:                         // Find m = x / 10^abs(expo10)
      fild DWORD PTR[edx]         //                                       st0=expo10
      fldl2t                      //                                       st0=log2(10)         , st1=expo10
      fmul
      fld st(0)
      frndint
      fsub st(1),st(0)
      fxch st(1)
      f2xm1
      fld1
      fadd
      fscale
      fstp st(1)                  //                                       st0=10^expo10

      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]         //                                       st0=x                , st1=10^expo10
      fdivr                       //                                       st0=x/10^expo10

  Rescale:                        //                                       st0=m
      fld tenE18                  //                                       st0=1e18             , st1=m
      fmul                        // m *= 1e18                             st0=m
      mov eax, DWORD PTR[edx]     //                                       eax=expo10
      fld tenE18M1                //                                       st0=1e18-1           , st1=m
  WhileLoop:                      // while(|m| >= 1e18-1) {                st0=1e18-1           , st1=m
      fld st(1)                   //                                       st0=m                , st1=1e18-1          , st2=m
      fabs                        //                                       st0=|m|              , st1=1e18-1          , st2=m
      fcomip st, st(1)            //   compare |m| and 1e18-1 and pop |m|  st0=1e18-1           , st1=m
      jb Exit                     //   if(|m| < 1e18-1) goto Exit          st0=1e18-1           , st1=m
      fld ten                     //                                       st0=10               , st1=1e18-1          , st2=m
      fdivp st(2), st(0)          //   m /= 10 and pop st0                 st0=1e18-1           , st1=m
      inc eax                     //   expo10++
      jmp WhileLoop               // }
  Exit:                           // eax = new expo10
      mov DWORD PTR[edx], eax     // Restore expo10
      fstp st(0)                  // Pop st(0)                             st0=m
      mov eax, DWORD PTR bcd
      fbstp TBYTE PTR[eax]        // Pop m into bcd                        Assertion: 1 <= |st0| < 1e18-1 and x = st0 * 10^(eax-18)
  }
  FPU::restoreControlWord(cwSave);
}

#endif // IS32BIT

Double80::Double80(float x) {
  switch (_fpclass(x)) {
  case _FPCLASS_QNAN:
    *this = std::numeric_limits<Double80>::quiet_NaN();
    break;
  case _FPCLASS_SNAN:
    *this = std::numeric_limits<Double80>::signaling_NaN();
    break;
  default:
    D80FromFlt(*this, x);
  }
}

Double80::Double80(double x) {
  switch (_fpclass(x)) {
  case _FPCLASS_QNAN:
    *this = std::numeric_limits<Double80>::quiet_NaN();
    break;
  case _FPCLASS_SNAN:
    *this = std::numeric_limits<Double80>::signaling_NaN();
    break;
  default:
    D80FromDbl(*this, x);
  }
}

float  getFloat(const Double80 &x) {
  switch (_fpclass(x)) {
  case _FPCLASS_QNAN:
    return std::numeric_limits<float>::quiet_NaN();
  case _FPCLASS_SNAN:
    return std::numeric_limits<float>::signaling_NaN();
  default:
    return D80ToFlt(x);
  }
}

double getDouble(const Double80 &x) {
  switch (_fpclass(x)) {
  case _FPCLASS_QNAN:
    return std::numeric_limits<double>::quiet_NaN();
  case _FPCLASS_SNAN:
    return std::numeric_limits<double>::signaling_NaN();
  default:
    return D80ToDbl(x);
  }
}

Double80 asin(const Double80 &x) {
  if(x == 1) {
    return DBL80_PI_05;
  } else if(x == -1) {
    return -DBL80_PI_05;
  } else {
    return atan2(x,sqrt(1.0-x*x));
  }
}

Double80 mypow(const Double80 &x, const Double80 &y) {
  if(x < 0) {
    if(y == floor(y)) {
      const int d = getInt(y);
      return (d & 1) ? -pow(-x,y) : pow(-x,y);
    }
  }
  return pow(x, y);
}

Double80 root(const Double80 &x, const Double80 &y) {
  if(x.isNegative()) {
    if(y == floor(y)) {
      const int d = getInt(y);
      if((d & 1)) {
        return -pow(-x, 1.0/y);
      }
    }
  }
  return pow(x, 1.0/y);
}

Double80 fraction(const Double80 &x) {
  if(x.isZero()) {
    return x;
  } else if(x.isNegative()) {
    return -fraction(-x);
  } else {
    return x - floor(x);
  }
}

Double80 round(const Double80 &x, int dec) { // 5-rounding
  const int sx = sign(x);
  switch(sx) {
  case 0:
    return Double80::_0;
  case  1:
  case -1:
    switch(sign(dec)) {
    case 0:
      return (sx == 1) ? floor(0.5+x) : -floor(0.5-x);
    case 1 :
      { Double80 p = Double80::pow10(dec);
        const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
        Double80 result = (sx == 1) ? floor(0.5+x*p) : -floor(0.5-x*p);
        result /= p;
        FPU::restoreControlWord(cwSave);
        return result;
      }
    case -1:
      { Double80 p = Double80::pow10(-dec);
        const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
        Double80 result = (sx == 1) ? floor(0.5+x/p) : -floor(0.5-x/p);
        result *= p;
        FPU::restoreControlWord(cwSave);
        return result;
      }
    }
  }
  throwException(_T("round dropped to the end. x=%s. dec=%d"),toString(x).cstr(),dec);
  return x; // Should never come here
}

ULONG Double80::hashCode() const {
  return *(ULONG*)m_value
       ^ *(ULONG*)(m_value+4)
       ^ *(USHORT*)(m_value+8);
}

Double80 Pow10Cache::pow10(int p) const {
  Double80 d1 = m_c1.pow10(p);
  const Double80 d2 = m_c2.pow10(p);
  const UINT64 sig1 = *((UINT64*)&d1);
  const UINT64 sig2 = *((UINT64*)&d2);
  if(sig1 == sig2) {
    return d1;
  }
  (*(UINT64*)&d1) = (sig1 & 0xffffffff00000000) + (((sig1 & 0xffffffff) + (sig2 & 0xffffffff)) >> 1);
  return d1;
}

#ifdef _DUMPPOW10
#include <DebugLog.h>

static String d80ToHexDumpFormat(const Double80 &v) {
  const UINT64 sig   = getSignificand(v);
  const UINT   expo  = getExponent(   v);
  const UINT   expo2 = getExpo2(      v);
  TCHAR tmpStr[100];
  return format(_T("%-29s[%#18I64x %#6x] expo2:%+6d"), d80tot(tmpStr, v), sig, expo, expo2);
}

void Pow10Cache::dumpAll() const {
  redirectDebugLog();
  debugLog(_T("Double80::pow10-values\n"));
  for(int i = MINPOW10; i <= MAXPOW10; i++) {
    debugLog(_T("pow10(%+5d):%s\n"), i, d80ToHexDumpFormat(pow10(i)).cstr());
  }
  debugLog(_T("End Double80::pow10-values\n"));
}
#endif // _DUMPPOW10
