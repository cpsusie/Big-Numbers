#include "pch.h"
#include <comdef.h>
#include <atlconv.h>
#include <limits.h>
#include <Math/Double80.h>
#include <math.h>
#include <float.h>

#pragma check_stack(off)
#pragma warning(disable : 4073)
#pragma init_seg(lib)

#define ASM_OPTIMIZED

//#define SMART_VERSION

DEFINECLASSNAME(FPU);

#ifdef IS32BIT

void FPU::init() { // static
  __asm {
    fninit
  }
}

unsigned short FPU::getStatusWord() { // static
  unsigned short sw;
  __asm {
    fstsw sw
  }
  return sw;
}

unsigned short FPU::getControlWord() { // static
  unsigned short cw;
  __asm {
    fnstcw cw 
  }
  return cw;
}

void FPU::setControlWord(unsigned short flags) { // static
  __asm {
    fldcw flags
  }
}

unsigned short FPU::getTagsWord() { // static
  unsigned short buffer[14];
  __asm {
    fstenv buffer
  }
  return buffer[4];
}

void FPU::clearExceptions() { // static
  __asm {
    fclex
  }
}

#endif

#define SETBIT(n,bit)   ((n) |  (1<<(bit)))
#define CLEARBIT(n,bit) ((n) & ~(1<<(bit)))

unsigned short FPU::setPrecisionMode(FPUPrecisionMode mode) { // static
  const unsigned short cw = getControlWord();
  switch(mode) {
  case FPU_LOW_PRECISION   : // set bit[8;9] of FPU control register to 0,0
    setControlWord((unsigned short)CLEARBIT(CLEARBIT(cw,8),9));
    break;
  case FPU_NORMAL_PRECISION: // set bit[8;9] of FPU control register to 0,1
    setControlWord((unsigned short)SETBIT(CLEARBIT(cw,8),9));
    break;
  case FPU_HIGH_PRECISION  : // set bit[8;9] of FPU control register to 1,1
    setControlWord((unsigned short)SETBIT(SETBIT(cw,8),9));
    break;
  default: throwInvalidArgumentException(__TFUNCTION__,_T("mode=%d"), mode);
  }
  return cw;
}

FPUPrecisionMode FPU::getPrecisionMode() { // static
  const unsigned short cw            = getControlWord();
  const int            precisionMode = (cw >> 8) & 3;
  switch(precisionMode) {
  case 0 : return FPU_LOW_PRECISION;
  case 2 : return FPU_NORMAL_PRECISION;
  case 3 : return FPU_HIGH_PRECISION;
  case 1 : 
  default: throwException(_T("%s:Invalid precisionMode. bit[8,9] = %x")
                         ,__TFUNCTION__,precisionMode); // Should not come here
           return FPU_HIGH_PRECISION;
  }
}

unsigned short FPU::setRoundMode(FPURoundMode mode) { // static
  const unsigned short cw = getControlWord();
  switch(mode) {
  case FPU_ROUNDCONTROL_ROUND     : // set bit[10;11] of FPU control register to 0,0
    setControlWord((unsigned short)CLEARBIT(CLEARBIT(cw,10),11));
    break;
  case FPU_ROUNDCONTROL_ROUNDDOWN : // set bit[10;11] of FPU control register to 1,0
    setControlWord((unsigned short)CLEARBIT(SETBIT(cw,10),11));
    break;
  case FPU_ROUNDCONTROL_ROUNDUP   : // set bit[10;11] of FPU control register to 0,1
    setControlWord((unsigned short)SETBIT(CLEARBIT(cw,10),11));
    break;
  case FPU_ROUNDCONTROL_TRUNCATE  : // set bit[10;11] of FPU control register to 1,1
    setControlWord((unsigned short)SETBIT(SETBIT(cw,10),11));
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("mode=%d"), mode);
    break;
  }
  return cw;
}

FPURoundMode FPU::getRoundMode() { // static
  const unsigned short cw           = getControlWord();
  const int            roundingMode = (cw >> 10) & 3;
  switch(roundingMode) {
  case 0 : return FPU_ROUNDCONTROL_ROUND;
  case 1 : return FPU_ROUNDCONTROL_ROUNDDOWN;
  case 2 : return FPU_ROUNDCONTROL_ROUNDUP;
  case 3 : return FPU_ROUNDCONTROL_TRUNCATE;
  default: throwException(_T("%s::%s:Invalid roundMode. bit[10,11] = %x")
                         ,s_className, __TFUNCTION__, roundingMode); // Should not come here
           return FPU_ROUNDCONTROL_ROUND;
  }
}

int FPU::getStackHeight() { // static
  int TOP = ((getStatusWord() >> 11) & 7);
  return (TOP != 0) ? (8 - TOP) : (getTagsWord() == 0xffff) ? 0 : 8;
}

bool FPU::stackFault() { // static
  return (getStatusWord() & 0x40) == 0x40;
}

bool FPU::stackOverflow() { // static
  return (getStatusWord() & 0x240) == 0x240;
}

bool FPU::stackUnderflow() { // static
  return (getStatusWord() & 0x240) == 0x040;
}


void FPU::enableExceptions(bool enable, unsigned short flags) { // static
  flags &= 0x3f;  // We are only interested in the first 6 bits
  if(enable) {
    setControlWord(getControlWord() & ~flags); // 0-bit ENABLES the interrupt, a 1-bit DISABLES it
  } else {
    setControlWord(getControlWord() | flags);
  }
}

void FPU::clearStatusWord() { // static
  unsigned short cw = getControlWord();
  init();
  setControlWord(cw);
}

class InitFPU {
public:
  InitFPU();
};

InitFPU::InitFPU() {
  FPU::setPrecisionMode(FPU_HIGH_PRECISION);
}

static InitFPU initFPU;

class InitDouble80 {
public:
  inline InitDouble80() {
    Double80::initClass();
  }
};

DEFINECLASSNAME(Double80);

const        Double80     Double80::zero;
const        Double80     Double80::one;
const        Double80     Double80::M_PI;
const        Double80     Double80::DBL80_EPSILON((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\xc0\x3f"); // 1.08420217248550443e-019;
const        Double80     Double80::DBL80_MIN(    (BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\x01\x00"); // 3.36210314311209209e-4932;
const        Double80     Double80::DBL80_MAX(    (BYTE*)"\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x7f"); // 1.18973149535723227e+4932
const        int          Double80::DBL80_DIG = 19;

static const Double80     M_PI_2;
static const Double80     M_PI_05;
static const Double80     M_PI_2_60;
static const Double80     half;
static const Double80     oneTenth;
static const Double80     ten;
static const Double80     minusOne;
static const Double80     two;
static const Double80     tenE18;
static const Double80     tenE18M1;
static const Double80     maxi32;
static const Double80     log2_5;
static const Double80     digitLookupTable[10];

static const Double80 maxi32P1 = Double80((UINT)_I32_MAX + 1);
static const Double80 maxi64   = Double80(_I64_MAX);
static const Double80 maxi64P1 = Double80((unsigned __int64)_I64_MAX + 1);


void Double80::initClass() {
  if(sizeof(Double80) != 10) {
    throwException(_T("%s::Size of Double80 must be 10. Size=%d."), s_className, sizeof(Double80));
  }

  Double80 *p = (Double80*)(&M_PI);
  FPU::init();

#ifdef IS32BIT
  __asm {
    fldpi
    mov eax, p
    fstp TBYTE PTR [eax]
  }
#else
  D80getPi(*p);
#endif // IS32BIT

  (Double80&)zero            = 0;
  (Double80&)one             = 1;
  (Double80&)two             = 2;
  (Double80&)ten             = 10;
  (Double80&)minusOne        = -1;

  (Double80&)DBL80_EPSILON   = Double80((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\xc0\x3f"); // 1.08420217248550443e-019;
  (Double80&)DBL80_MIN       = Double80((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\x01\x00"); // 3.36210314311209209e-4932;
  (Double80&)DBL80_MAX       = Double80((BYTE*)"\xff\xff\xff\xff\xff\xff\xff\xff\xfe\x7f"); // 1.18973149535723227e+4932

  (Double80&)M_PI_2          = M_PI * 2;
  (Double80&)M_PI_05         = M_PI / 2;
  (Double80&)M_PI_2_60       = pow2(60) * M_PI_2;

  (Double80&)half            = 0.5;
  (Double80&)oneTenth        = 0.1;
  (Double80&)log2_5          = log2(5);
  (Double80&)tenE18          = 1e18;
  (Double80&)tenE18M1        = tenE18 - Double80::one;
  (Double80&)maxi32          = _I32_MAX;

  for(int i = 0; i < 10; i++) {
    (Double80&)digitLookupTable[i] = i;
  }

  FPU::clearExceptions();
}

static InitDouble80 initDouble80;

Double80::Double80(const BYTE *bytes) {
  memcpy(&m_value,bytes,sizeof(m_value));
}

Double80::Double80(const String &s) {
  init((const _TUCHAR*)(s.cstr()));
}

Double80::Double80(const TCHAR *s) {
  init((const _TUCHAR*)s);
}

#ifdef UNICODE
Double80::Double80(const char *s) {
  USES_CONVERSION;
  init(A2W(s));
}
#endif // UNICODE

void Double80::init(const _TUCHAR *s) {
  bool isNegative = false;
  Double80 result = zero;
  while(_istspace(*s)) {
    s++;
  }
  if(*s == _T('-')) {
    isNegative = true;
    s++;
  } else if(*s == _T('+')) {
    s++;
  }
  while(_istdigit(*s)) {
    result = result * ten + digitLookupTable[*(s++) - _T('0')];
  }
  if(*s == _T('.') && _istdigit(s[1])) {
    s++;
    Double80 decimals = zero;
    for(Double80 p = oneTenth; isdigit(*s); p /= ten) {
      decimals += p * digitLookupTable[*(s++) - _T('0')];
    }
    result += decimals;
  }
  if(*s == _T('e') || *s == _T('E')) {
    s++;
    int expoSign = 1;
    if(*s == _T('-')) {
      expoSign = -1;
      s++;
    } else if(*s == _T('+')) {
      s++;
    }
    int exponent = 0;
    while(_istdigit(*s)) {
      exponent = exponent * 10 + (*(s++) - _T('0'));
    }
    exponent *= expoSign;
    if(exponent < -4900) {
      result *= pow10(-4900);
      exponent += 4900;
      result *= pow10(exponent);
    } else {
      result *= pow10(exponent);    
    }
  }
  *this = isNegative ? -result : result;
}

#ifdef IS32BIT

Double80::Double80(long x) {
  __asm {
    fild x
    mov eax, this
    fstp TBYTE PTR [eax]
  }
}

Double80::Double80(unsigned long x) {
  if(x > _I32_MAX) {
    static const long i32max = _I32_MAX;
    const long tmp = x & _I32_MAX;
    __asm {
      fild tmp
      fild i32max
      fadd
      fld1
      fadd
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  } else {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
}

Double80::Double80(__int64 x) {
  __asm {
    fild x
    mov eax, this
    fstp TBYTE PTR [eax]
  }
}

Double80::Double80(unsigned __int64 x) {
  if(x > _I64_MAX) {
    static const __int64 i64max = _I64_MAX;
    const __int64 tmp = x & _I64_MAX;
    __asm {
      fild tmp
      fild i64max
      fadd
      fld1
      fadd
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  } else {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
}

Double80::Double80(float x) {
  __asm {
    fld x
    mov eax, this
    fstp TBYTE PTR [eax]
  }
}

Double80::Double80(double x) {
  __asm {
    fld x
    mov eax, this
    fstp TBYTE PTR [eax]
  }
}

int Double80::getExpo2(const Double80 &x) { // static
  int result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fxtract
    fstp st(0)
    fistp result
  }
  return result;
}

int Double80::getExpo10(const Double80 &x) { // static
  unsigned short cwSave,ctrlFlags;
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
    fld1
    fxch st(1)
    fabs
    fyl2x
    fldlg2
    fmul
    fnstcw cwSave
    mov ax, cwSave
    or  ax, 0x400              // set bit 10
    and ax, 0xf7ff             // clear bit 11
    mov ctrlFlags, ax
    fldcw ctrlFlags
    frndint
    fldcw cwSave
    fistp result
Exit:
  }
  return result;
}

long getLong(const Double80 &x) {
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_TRUNCATE);
  long result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fistp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

unsigned long getUlong(const Double80 &x) {
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_TRUNCATE);
  unsigned long result;
  if(x > maxi32) {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fld maxi32P1
      fsub
      fistp result;
    }
    result += (unsigned long)_I32_MAX + 1;
  } else {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fistp result;
    }
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

__int64 getInt64(const Double80 &x) {
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_TRUNCATE);
  __int64 result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fistp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

unsigned __int64 getUint64(const Double80 &x) {
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_TRUNCATE);
  unsigned __int64 result;
  if(x > maxi64) {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fld maxi64P1
      fsub
      fistp result;
    }
    result += (unsigned __int64)_I64_MAX + 1;
  } else {
    __asm {
      mov eax, x
      fld TBYTE PTR [eax]
      fistp result;
    }
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

float getFloat(const Double80 &x) {
  float result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fstp result
  }
  return result;
}

double getDouble(const Double80 &x) {
  double result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fstp result
  }
  return result;
}

Double80 operator+(const Double80 &x, const Double80 &y) {
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

Double80 operator-(const Double80 &x, const Double80 &y) {
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

Double80 operator-(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fchs
    fstp result
  }
  return result;
}

Double80 operator*(const Double80 &x, const Double80 &y) {
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

Double80 operator/(const Double80 &x, const Double80 &y) {
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

Double80 &Double80::operator+=(const Double80 &x) {
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

Double80 &Double80::operator-=(const Double80 &x) {
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

Double80 &Double80::operator*=(const Double80 &x) {
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

Double80 &Double80::operator/=(const Double80 &x) {
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

Double80 &Double80::operator++() { // prefix-form
  __asm {
    mov eax, this
    fld TBYTE PTR [eax]
    fld1
    fadd
    fstp TBYTE PTR [eax]
  }
  return *this;
}

Double80 &Double80::operator--() { // prefix-form
  __asm {
    mov eax, this
    fld TBYTE PTR [eax]
    fld1
    fsub
    fstp TBYTE PTR [eax]
  }
  return *this;
}

Double80 Double80::operator++(int dummy) { // postfix-form
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

Double80 Double80::operator--(int dummy) { // postfix-form
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

bool operator!=(const Double80 &x, const Double80 &y) {
  bool result = false;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop y 
    je Exit                     // if st(0) == st(1) (x < y) goto end
    mov result, 1
Exit:
    fstp st(0)                  // pop x
  }
  return result;
}

bool operator==(const Double80 &x, const Double80 &y) {
  bool result = false;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop y
    jne Exit                    // if st(0) != st(1) (x != y) goto end
    mov result, 1
Exit:
    fstp st(0)                  // pop x
  }
  return result;
}

bool operator>=(const Double80 &x, const Double80 &y) {
  bool result = false;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    jb Exit                     // if st(0) < st(1) (x < y) goto end
    mov result, 1
Exit:
    fstp st(0)                  // pop y
  }
  return result;
}

bool operator<=(const Double80 &x, const Double80 &y) {
  bool result = false;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop y
    jb Exit                     // if st(0) < st(1) (y < x) goto end
    mov result, 1
Exit:
    fstp st(0)                  // pop x
  }
  return result;
}

bool operator>(const Double80 &x, const Double80 &y) {
  bool result = true;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    ja Exit                     // if st(0) > st(1) (x > y) goto end
    mov result, 0
Exit:
    fstp st(0)                  // pop y
  }
  return result;
}

bool operator<(const Double80 &x,const Double80 &y) {
  bool result = true;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop y
    ja Exit                     // if st(0) > st(1) (y > x) goto end
    mov result, 0
Exit:
    fstp st(0)                  // pop x
  }
  return result;
}

bool Double80::isZero() const {
  bool result = false;
  __asm {
    mov eax, this
    fld TBYTE PTR [eax]
    ftst
    fstsw ax
    sahf
    fstp    st(0)
    jne Exit                    // if st(0) != st(1) (this != zero) goto end
    mov result, 1
Exit:
  }
  return result;
}

Double80 fabs(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fabs
    fstp result
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

Double80 sqr(const Double80 &x) {
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

Double80 sqrt(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsqrt
    fstp result
  }
  return result;
}

Double80 sin(const Double80 &x) {
  Double80 result = fmod(x,M_PI_2_60);
  __asm {
    fld result
    fsin
    fstp result
  }
  return result;
}

Double80 cos(const Double80 &x) {
  Double80 result = fmod(x,M_PI_2_60);
  __asm {
    fld result
    fcos
    fstp result
  }
  return result;
}

Double80 tan(const Double80 &x) {
  Double80 result = fmod(x,M_PI_2_60);
  __asm {
    fld result
    fptan
    fstp result
    fstp result
  }
  return result;
}

Double80 atan(const Double80 &x) {
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

Double80 atan2(const Double80 &y ,const Double80 &x) {
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

void sincos(Double80 &c, Double80 &s) { // calculate both cos and sin. c:inout c, s:out
  Double80 r = fmod(c,M_PI_2_60);
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
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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

  FPU::restoreControlWord(cwSave);
  return result;
}

Double80 log(const Double80 &x) {
  Double80 result;
  __asm {
    fld1
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fldln2
    fmul
    fstp result
  }
  return result;
}

Double80 log10(const Double80 &x) {
  Double80 result;
  __asm {
    fld1
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fldlg2
    fmul
    fstp result
  }
  return result;
}

Double80 log2(const Double80 &x) {
  Double80 result;
  __asm {
    fld1
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fstp result
  }
  return result;
}

Double80 pow(const Double80 &x, const Double80 &y) {
  if(y.isZero()) {
    return Double80::one;
  }
  if(x.isZero()) {
    return y.isNegative() ? (Double80::one / Double80::zero) : Double80::zero;
  }

  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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
  FPU::restoreControlWord(cwSave);
  return result;
}

Double80 pow10(const Double80 &x) {
  if(x.isZero()) {
    return Double80::one;
  }

  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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
  FPU::restoreControlWord(cwSave);
  return result;
}

Double80 pow2(const Double80 &x) {
  if(x.isZero()) {
    return Double80::one;
  }

  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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

Double80 floor(const Double80 &x) {
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDUP);
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

#endif // IS32BIT

Double80 cot(const Double80 &x) {
  return Double80::one/tan(x);
}

Double80 asin(const Double80 &x) {
  if(x == Double80::one) {
    return M_PI_05;
  } else if(x == minusOne) {
    return -M_PI_05;
  } else {
    return atan2(x,sqrt(Double80::one-x*x));
  }
}

Double80 acos(const Double80 &x) {
  return M_PI_05 - asin(x);
}

Double80 acot(const Double80 &x) {
  return M_PI_05 - atan(x);
}

Double80 cosh(const Double80 &x) {
  return (exp(x) + exp(-x))/two;
}

Double80 sinh(const Double80 &x) {
  return (exp(x) - exp(-x))/two;
}

Double80 tanh(const Double80 &x) {
  const Double80 e1 = exp(x);
  const Double80 e2 = Double80::one/e1;
  return (e1 - e2)/(e1+e2);
}

Double80 acosh(const Double80 &x) {
  return log(x + sqrt(x*x-Double80::one));
}

Double80 asinh(const Double80 &x) {
  return log(x + sqrt(x*x+Double80::one));
}

Double80 atanh(const Double80 &x) {
  return log(sqrt((Double80::one+x)/(Double80::one-x)));
}

Double80 root(const Double80 &x, const Double80 &y) {
  return pow(x,Double80::one/y);
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

int sign(const Double80 &x) {
  if(x.isZero()) {
    return 0;
  } else if(x.isNegative()) {
    return -1;
  } else {
    return 1;
  }
}

Double80 round(const Double80 &x, int dec) { // 5-rounding
  int sx = sign(x);
  switch(sx) {
  case 0:
    return Double80::zero;
  case  1:
  case -1:
    switch(sign(dec)) {
    case 0:
      return sx == 1 ? floor(half+x) : -floor(half-x);
    case 1 :
      { Double80 p = pow10(dec);
        const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
        Double80 result = (sx == 1) ? floor(half+x*p) : -floor(half-x*p);
        result /= p;
        FPU::restoreControlWord(cwSave);
        return result;
      }
    case -1:
      { Double80 p = pow10(-dec);
        const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
        Double80 result = (sx == 1) ? floor(half+x/p) : -floor(half-x/p);
        result *= p;
        FPU::restoreControlWord(cwSave);
        return result;
      } 
    }
  }
  throwException(_T("round dropped to the end. x=%s. dec=%d"),x.toString().cstr(),dec);
  return x; // Should never come here
}

Double80 Max(const Double80 &x, const Double80 &y) {
  return (x >= y) ? x : y;
}

Double80 Min(const Double80 &x, const Double80 &y) {
  return (x <= y) ? x : y;
}

#define SIGNIFICAND(d) ((*((unsigned __int64*)(&(d)))) & 0xffffffffffffffffui64)
#define EXPONENT(d)    ((*(unsigned short*)(((char*)&(d)) + 8)) & 0x7fff)

bool isPInfinity(const Double80 &x) {
  return isInfinity(x) && x.isPositive();
}

bool isNInfinity(const Double80 &x) {
  return isInfinity(x) && x.isNegative();
}

bool isInfinity(const Double80 &x) {
  return isNan(x) && (SIGNIFICAND(x) == 0x8000000000000000ui64);
}

bool isNan(const Double80 &x) {
  return EXPONENT(x) == 0x7fff;
}

unsigned long Double80::hashCode() const {
  return *(unsigned long*)m_value 
       ^ *(unsigned long*)(m_value+4)
       ^ *(unsigned short*)(m_value+8);
}

String Double80::toString() const {
  TCHAR tmp[30];
  return d80tot(tmp, *this);
}

char *Double80::d80toa(char *dst, const Double80 &x) {
#ifndef UNICODE
  return d80tot(dst, x);
#else
  USES_CONVERSION;
  TCHAR tmp[30];
  return strcpy(dst, W2A(d80tot(tmp, x)));
#endif
}

#define declareBuffer(b,size)          TCHAR b[size];  int b##_length = 0
#define declareAssignedBuffer(b,dst)   TCHAR *b = dst; int b##_length = 0
#define addChar(b,ch)                  b[b##_length++] = ch
#define addDigit(b,d)                  addChar(b,(d)+'0')
#define getLength(b)                   (b##_length)
#define removeLast(b)                  b[--b##_length]

TCHAR *Double80::d80tot(TCHAR *dst, const Double80 &x) {
  if(isNan(x)) {
    if(!isInfinity(x)) {
      return _tcscpy(dst, _T("Nan"));
    } else if(isPInfinity(x)) {
      return _tcscpy(dst, _T("+Infinity"));
    } else if(isNInfinity(x)) {
      return _tcscpy(dst, _T("-Infinity"));
    }
  } else if(x.isZero()) {
    return _tcscpy(dst, _T("0.00000000000000000e+000"));
  }

  int expo10 = getExpo10(x);
  BYTE bcd[10];

#ifndef ASM_OPTIMIZED

  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
  Double80 m = (expo10 == 0) ? x : (x / pow10(expo10));
  m = m * tenE18;
  while(fabs(m) >= tenE18M1) {
    m = m / ten;
    expo10++;
  }

  // Assertion: 1 <= |m| < 1e18-1 and x = m * 10^(expo10-18)

#ifdef IS32BIT
  __asm {
    fld m
    fbstp TBYTE PTR bcd
  }
#else
  D80ToBCD(bcd, m);
#endif // IS32BIT

  FPU::restoreControlWord(cwSave);

#else // ASM_OPTIMIZED

#ifdef IS32BIT
  const unsigned short cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUND);
  static const Double80 E18    = tenE18;
  static const Double80 E18M1  = tenE18M1;
  static const Double80 TEN    = ten;
  static const Double80 LOG2_5 = log2_5;

  unsigned short cwSave1,ctrlFlags;

  #ifndef SMART_VERSION
    _asm {
      mov eax, expo10
      cmp eax, 0
      jne ScaleX

      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      jmp Rescale

  ScaleX:                         // Find m = x / 10^abs(expo10)
      fild expo10                 //                                       st0=expo10
      fldl2t                      //                                       st0=log2(10)         , st1=expo10
      fmul
      fld st(0)
      fnstcw cwSave1              // Save control word
      mov ax, cwSave1
      or  ax, 0x400               // Set   bit 10
      and ax, 0xf7ff              // Clear bit 11
      mov ctrlFlags, ax
      fldcw ctrlFlags
      frndint                     // Round down
      fldcw cwSave1               // Restore control word
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
      fld E18                     //                                       st0=1e18             , st1=m
      fmul                        // m *= 1e18                             st0=m
      mov eax, expo10             //                                       eax=expo10
      fld E18M1                   //                                       st0=1e18-1           , st1=m
  WhileLoop:                      // while(|m| >= 1e18-1) {                st0=1e18-1           , st1=m
      fld st(1)                   //                                       st0=m                , st1=1e18-1          , st2=m
      fabs                        //                                       st0=|m|              , st1=1e18-1          , st2=m
      fcomip st, st(1)            //   compare |m| and 1e18-1 and pop |m|  st0=1e18-1           , st1=m
      jb Exit                     //   if(|m| < 1e18-1) goto Exit          st0=1e18-1           , st1=m
      fld TEN                     //                                       st0=10               , st1=1e18-1          , st2=m
      fdivp st(2), st(0)          //   m /= 10 and pop st0                 st0=1e18-1           , st1=m
      inc eax                     //   expo10++
      jmp WhileLoop               // }
  Exit:
      fstp st(0)                  // Pop st(0)                             st0=m
      fbstp TBYTE PTR bcd         // Pop m into bcd                        Assertion: 1 <= |st0| < 1e18-1 and x = st0 * 10^(eax-18)
      mov expo10, eax             // Restore expo10
    }

  #else // SMART_VERSION

    const int p18 = expo10 - 18;

    _asm {
      mov eax, p18
      cmp eax, 0
      jne ScaleX

      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]
      jmp Rescale

  ScaleX:                         // Find m = x/10^p18. First find 5^p18
      fild p18                    //                                       st0=p18
      fld  LOG2_5                 //                                       st0=log2(5)          , st1=p18
      fmul
      fld st(0)
      fnstcw cwSave1              // Save control word
      mov ax, cwSave1
      or  ax, 0x400               // Set   bit 10
      and ax, 0xf7ff              // Clear bit 11
      mov ctrlFlags, ax
      fldcw ctrlFlags
      frndint                     // Round down
      fldcw cwSave1               // Restore control word
      fsub st(1),st(0)
      fxch st(1)
      f2xm1
      fld1
      fadd
      fscale
      fstp st(1)                  //                                       st0=5^p18
                                  // Calculate x/2^p18
      mov eax, DWORD PTR x
      fld TBYTE PTR [eax]         //                                       st0=x                , st1=5^p18
      fxtract                     //                                       st0=significand(x)   , st1=expo2(x)        , st2=5^p18
      fild p18                    //                                       st0=p18              , st1=significand(x)  , st2=expo2(x), st3=5^p18
      fsubp st(2),st              //                                       st0=significand(x)   , st1=expo2(x)-p18    , st2=5^p18
      fscale                      //                                       st0=x/2^p18          , st1=expo2(x)-p18    , st2=5^p18
      fstp st(1)                  // pop st1                               st0=x/2^p18          , st1=5^p18
      fdivr                       // st0 /= st1                            st0=m = x/2^p18/5^p18 = x/10^p18

  Rescale:                        //                                       st0=m
      mov eax, expo10             //                                       eax=expo10
      fld E18M1                   //                                       st0=1e18-1           , st1=m
  WhileLoop:                      // while(|m| >= 1e18-1) {                st0=1e18-1           , st1=m
      fld st(1)                   //                                       st0=m                , st1=1e18-1          , st2=m
      fabs                        //                                       st0=|m|              , st1=1e18-1          , st2=m
      fcomip st, st(1)            //   compare |m| and 1e18-1 and pop |m|  st0=1e18-1           , st1=m
      jb Exit                     //   if(|m| < 1e18-1) goto end           st0=1e18-1           , st1=m
      fld TEN                     //                                       st0=10               , st1=1e18-1          , st2=m
      fdivp st(2), st(0)          //   m /= 10 and pop st0                 st0=1e18-1           , st1=m
      inc eax                     //   expo10++
      jmp WhileLoop               // }
    Exit:
      fstp st(0)                  // Pop st(0)                             st0=m
      fbstp TBYTE PTR bcd         // Pop m into bcd                        Assertion: 1 <= |st0| < 1e18-1 and x = st0 * 10^(eax-18)
      mov expo10, eax             // Restore expo10
    }
  #endif // SMART_VERSION

  FPU::restoreControlWord(cwSave);
#else // ! IS32BIT (ie IS64BIT)

  D80ToBCDAutoScale(bcd, x, expo10);

#endif // IS32BIT

#endif // ASM_OPTIMIZED

  declareAssignedBuffer(result, dst);

  if(bcd[9] & 0x80) {
    addChar(result,_T('-'));
  }
  bool gotDigit = false;
  int decimals;
  for(int i = 8; i >= 0; i--) {
    if(gotDigit) {
      addDigit(result,bcd[i]>>4);
      addDigit(result,bcd[i]&0xf);
    } else if(bcd[i] == 0) {
      continue;
    } else {
      gotDigit = true;
      if(bcd[i] & 0xf0) {
        addDigit(result,bcd[i]>>4);
        addChar(result,_T('.'));
        addDigit(result,bcd[i]&0x0f);
        decimals = i * 2 - 1;
      } else {
        addDigit(result,bcd[i]&0xf);
        addChar(result,_T('.'));
        decimals = i * 2 - 2;
      }
    }
  }

  addChar(result,_T('e'));
  expo10 += decimals - 16;
  if(expo10 < 0) {
    addChar(result,_T('-'));
    expo10 = -expo10;
  } else {
    addChar(result,_T('+'));
    if(expo10 == 0) {
      addChar(result,_T('0'));
      addChar(result,_T('0'));
      addChar(result,_T('0'));
      addChar(result,0);
      return dst;
    }
  }

  declareBuffer(exponentBuffer,10);

  do {
    const int ed = expo10 % 10;
    expo10 /= 10;
    addDigit(exponentBuffer,ed);
  } while(expo10 != 0);
  for(int i = getLength(exponentBuffer); i <= 2; i++) {
    addChar(result,_T('0'));
  }
  while(getLength(exponentBuffer) > 0) {
    addChar(result,removeLast(exponentBuffer));
  }
  addChar(result,0);

  return dst;
}
