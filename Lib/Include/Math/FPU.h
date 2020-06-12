#pragma once

#include <MyUtil.h>

typedef enum {
  FPU_LOW_PRECISION     = 0    // 32-bits floatingpoint
 ,FPU_NORMAL_PRECISION  = 2    // 64-bits floatingpoint
 ,FPU_HIGH_PRECISION    = 3    // 80-bits floatingpoint
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

#if defined(_DEBUG)
#pragma runtime_checks( "", off )
#endif _DEBUG

#pragma pack(push,1)
class FPUControlWord {
  friend class FPU;
private:
  union {
    WORD m_cw;
    struct {
      UINT m_im     : 1;   // Invalid operation Mask
      UINT m_dm     : 1;   // Denormalized operand Mask
      UINT m_zm     : 1;   // Zero divide Mask
      UINT m_om     : 1;   // Overflow Mask
      UINT m_um     : 1;   // Underflow Mask
      UINT m_pm     : 1;   // Precision Mark
      UINT m_unused : 1;
      UINT m_iem    : 1;   // Interrupt Enable Mask
      UINT m_pc     : 2;   // Precision Control
      UINT m_rc     : 2;   // Rounding Control
      UINT m_ic     : 1;   // Infinity Control
    };
  };
  inline FPUControlWord(USHORT cw) : m_cw(cw) {
  }
public:
  inline FPUControlWord() {
  }
  inline FPUControlWord &setPrecisionMode(FPUPrecisionMode p) {
    m_pc = (UINT)p;
    return *this;
  }
  inline FPUPrecisionMode getPrecisionMode() const {
    return (FPUPrecisionMode)m_pc;
  }
  inline FPUControlWord   &setRoundMode(FPURoundMode mode) {
    m_rc = mode;
    return *this;
  }
  inline FPURoundMode     getRoundMode() const {
    return (FPURoundMode)m_rc;
  }
  inline FPUControlWord   &adjustExceptionMask(USHORT enable, USHORT disable) {
    m_cw &= ~(enable & FPU_ALL_EXCEPTIONS); // 0-bit ENABLES the interrupt
    m_cw |= (disable & FPU_ALL_EXCEPTIONS); // 1-bit DISABLES it
    return *this;
  }
  inline USHORT            getExceptionMask() const {
    return m_cw & FPU_ALL_EXCEPTIONS;
  }
  inline operator USHORT() const {
    return m_cw;
  }
  String toString() const;
};

typedef struct {
  union {
    WORD m_data;
    struct {
      UINT m_i      : 1;   // Invalid Operation Exception
      UINT m_d      : 1;   // Denormalized Operand Exception
      UINT m_z      : 1;   // Zero Divide Exception
      UINT m_o      : 1;   // Overflow Exception
      UINT m_u      : 1;   // Underflow Exception
      UINT m_p      : 1;   // Precision Exception
      UINT m_sf     : 1;   // Stack Fault Exception
      UINT m_ir     : 1;   // Interrupt Request
      UINT m_c0     : 1;
      UINT m_c1     : 1;
      UINT m_c2     : 1;
      UINT m_top    : 3;   // Stack top
      UINT m_c3     : 1;
      UINT m_busy   : 1;   // Busy
    };
  };
  String toString() const;
} FPUStatusWord;

typedef enum {
  FPUREG_VALID_NZ
 ,FPUREG_ZERO
 ,FPUREG_NAN
 ,FPUREG_EMPTY
} FPURegisterContent;

typedef struct {
  union {
    WORD m_data;
    struct {
      UINT m_tag0  : 2;
      UINT m_tag1  : 2;
      UINT m_tag2  : 2;
      UINT m_tag3  : 2;
      UINT m_tag4  : 2;
      UINT m_tag5  : 2;
      UINT m_tag6  : 2;
      UINT m_tag7  : 2;
    };
  };
  inline FPURegisterContent getContent(UINT index) const {
    return (FPURegisterContent)((m_data>>index)&3);
  }
  String toString() const;
  static const TCHAR *getContentStr(FPURegisterContent content);
} FPUTagWord;

class FPUEnvironment {
public:
  FPUControlWord  m_ctrlWord;
  FPUStatusWord   m_statusWord;
  FPUTagWord      m_tagWord;
  DWORD           m_IP;
  WORD            m_codeSeg;
  WORD            m_filler4;
  DWORD           m_opAddr;
  WORD            m_dataSeg;
  WORD            m_filler5;
  String toString() const;
};

class Double80;

class FPUState : public FPUEnvironment {
public:
  BYTE  m_st[8][10];
  inline const Double80 &getReg(UINT index) const {
    return (Double80&)(m_st[index]);
  }
  String toString() const;
};

#pragma pack(pop)

#if defined(IS64BIT)
extern "C" {
void FPUinit();
WORD FPUgetStatusWord();
WORD FPUgetControlWord();
void FPUsetControlWord(const WORD *cw);
void FPUgetEnvironment(void *env);
void FPUgetState(      void *state);
void FPUclearExceptions();
void FPUclearExceptionsNoWait();
};
#endif

class FPU {
private:
  FPU() {} // Cannot be instatiated
#if defined(IS32BIT)
  static inline void      FPUinit() {
    __asm {
      finit
    }
  }
  static inline WORD      FPUgetStatusWord() {
    WORD sw;
    __asm {
      fstsw sw
    }
    return sw;
  }
  static inline WORD      FPUgetControlWord() {
    WORD cw;
    __asm {
      fstcw cw
    }
    return cw;
  }
  static inline void      FPUsetControlWord(const WORD *cw) {
    __asm {
      mov eax, DWORD PTR cw
      fldcw WORD PTR[eax]
    }
  }
  static inline void      FPUgetEnvironment(void *env) {
    __asm {
      mov eax, DWORD PTR env
      fstenv DWORD PTR[eax]
    }
  }
  static inline void      FPUgetState(void *state) {
    __asm {
      mov eax, DWORD PTR state
      fsave DWORD PTR[eax]
      frstor DWORD PTR[eax];
    }
  }
  static inline void      FPUclearExceptions() {
    __asm {
      fclex
    }
  }
  void static inline      FPUclearExceptionsNoWait() {
    __asm {
      fnclex
    }
  }
#endif // IS23BIT

public:

  static inline void      init() {
    FPUinit();
  }
  static inline FPUStatusWord    getStatusWord() {
    FPUStatusWord sw;
    sw.m_data = FPUgetStatusWord();
    return sw;
  }
  static inline FPUControlWord   getControlWord() {
    return FPUControlWord(FPUgetControlWord());
  }
  static inline void             setControlWord(const FPUControlWord &cw) {
    FPUsetControlWord(&cw.m_cw);
  }
  static inline FPUEnvironment   getEnvironment() {
    FPUEnvironment env;
    FPUgetEnvironment(&env);
    return env;
  }
  static inline FPUTagWord       getTagWord() {
    FPUEnvironment env;
    FPUgetEnvironment(&env);
    return env.m_tagWord;
  }
  static inline FPUState         getState() {
    FPUState state;
    FPUgetState(&state);
    return state;
  }
  static inline void             clearExceptions() {
    FPUclearExceptions();
  }
  static inline void             clearExceptionsNoWait() {
    FPUclearExceptionsNoWait();
  }
  static inline void             clearStatusWord() {
    const FPUControlWord cw(FPUgetControlWord());
    init();
    FPUsetControlWord(&cw.m_cw);
  }

  // returns current FPU controlword
  static inline FPUControlWord   setPrecisionMode(FPUPrecisionMode p) {
    FPUControlWord cw(FPUgetControlWord()), old = cw;
    FPUsetControlWord(&cw.setPrecisionMode(p).m_cw);
    return old;
  }
  static inline FPUPrecisionMode getPrecisionMode() {
    return getControlWord().getPrecisionMode();
  }
  // returns current FPU controlword
  static inline FPUControlWord   setRoundMode(FPURoundMode mode) {
    FPUControlWord cw(FPUgetControlWord()), old = cw;
    FPUsetControlWord(&cw.setRoundMode(mode).m_cw);
    return old;
  }
  static inline FPURoundMode     getRoundMode() {
    return getControlWord().getRoundMode();
  }
  // returns current FPU controlword
  static FPUControlWord          adjustExceptionMask(USHORT enable, USHORT disable) {
    FPUControlWord cw(getControlWord()), old = cw;
    FPUsetControlWord(&cw.adjustExceptionMask(enable, disable).m_cw);
    return old;
  }
  static inline void             restoreControlWord(const FPUControlWord &cw) {
    FPUsetControlWord(&cw.m_cw);
  }
  static inline int              getStackHeight() {
    const int TOP = getStatusWord().m_top;
    return (TOP != 0) ? (8 - TOP) : (getTagWord().m_data == 0xffff) ? 0 : 8;
  }
  static inline bool             stackOverflow() {
    return (getStatusWord().m_data & 0x240) == 0x240;
  }
  static inline bool             stackUnderflow() {
    return (getStatusWord().m_data & 0x240) == 0x040;
  }
  static inline bool             stackFault() {
    return getStatusWord().m_sf;
  }
  static _se_translator_function setExceptionTranslator(_se_translator_function f);
};

#if defined(_DEBUG)
#pragma runtime_checks( "", restore )
#endif _DEBUG

class FPUException {
public:
  int m_code;
  FPUException(int code) : m_code(code) {
  }
};
void FPUexceptionTranslator(UINT u, EXCEPTION_POINTERS *pExp);
