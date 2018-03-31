#pragma once

#include <TinyBitSet.h>

typedef enum {
  REGTYPE_NONE
 ,REGTYPE_GPR
 ,REGTYPE_SEG
 ,REGTYPE_FPU
 ,REGTYPE_XMM
} RegType;

typedef enum {
  REGSIZE_BYTE     /* 8-bit   */
 ,REGSIZE_WORD     /* 16-bit  */
 ,REGSIZE_DWORD    /* 32-bit  */
 ,REGSIZE_QWORD    /* 64-bit  */
 ,REGSIZE_TBYTE    /* 80-bit  */
 ,REGSIZE_OWORD    /* 128-bit */
 ,REGSIZE_VOID     /* for LEA */
 ,REGSIZE_END   = -1
} RegSize;

#ifdef _DEBUG
#define SETDEBUGSTR() m_debugStr = toString()
#define DECLAREDEBUGSTR  protected: String m_debugStr
#else
#define SETDEBUGSTR()
#define DECLAREDEBUGSTR
#endif // _DEBUG

class RegSizeSet : public BitSet8 {
  DECLAREDEBUGSTR;
public:
  // Terminate with REGSIZE_END
  RegSizeSet(RegSize s1,...);
  String toString() const;
};

String toString(RegType regType);
String toString(RegSize regSize);
BYTE regSizeToByteCount(RegSize regSize);

#ifdef IS32BIT
#define INDEX_REGSIZE          REGSIZE_DWORD
#define MAX_GPREGISTER_INDEX   7
#define MAX_XMMREGISTER_INDEX  7
typedef int                    MovMaxImmType;
#else // IS64BIT
#define INDEX_REGSIZE         REGSIZE_QWORD
#define MAX_GPREGISTER_INDEX  15
#define MAX_XMMREGISTER_INDEX 15
typedef INT64                 MovMaxImmType;

typedef enum {
  REX_DONTCARE
 ,REX_REQUIRED
 ,REX_NOTALLOWED
} RexByteUsage;
#endif // IS64BIT

class Register {
private:
  const BYTE m_index; // = [0..15]
  Register &operator=(const Register &); // not implemented, and not accessible.
                                         // All register are singletons
  DECLAREDEBUGSTR;
public:

  static const RegSizeSet s_wordRegCapacity, s_dwordRegCapacity, s_qwordRegCapacity;

  inline Register(BYTE index) : m_index(index) {
  }
  inline UINT getIndex() const {
    return m_index;
  }
  virtual RegType getType()  const = 0;
  virtual RegSize getSize()  const = 0;
  // is this AL,AX,EAX or RAX
  inline bool isGPR0() const {
    return (getType() == REGTYPE_GPR) && (getIndex() == 0);
  }
#ifdef IS32BIT

#define IS_REXCOMPATIBLE(reg,rexBytePresent) true
#define REXBYTEUSAGECMP(reg1,reg2) 0
#else  // IS64BIT
  virtual bool    isREXCompatible(bool rexBytePresent) const {
    return true;
  }

#define IS_REXCOMPATIBLE(reg,rexBytePresent) ((reg).isREXCompatible(rexBytePresent))
#define REXBYTEUSAGECMP(reg1,reg2)           ((int)(reg1).getRexByteUsage() - (int)(reg2).getRexByteUsage())

  virtual RexByteUsage getRexByteUsage() const {
    return REX_DONTCARE;
  }
  inline bool     indexNeedREXByte() const {
    return getIndex() >= 8;
  }
  // for error messages
  static const TCHAR *getREXCompatibleRegisterNames(bool rexBytePresent);
#endif // IS64BIT

  virtual String  getName()  const {
    return format(_T("Unknown register:(type,sz,index):(%d,%d,%u"), getType(),getSize(),getIndex());
  }
  inline bool operator==(const Register &r) const {
    return (getType()                == r.getType() )
        && (getSize()                == r.getSize() )
        && (getIndex()               == r.getIndex())
        && (REXBYTEUSAGECMP(*this,r) == 0           );
  }
  inline bool operator!=(const Register &r) const {
    return !(*this == r);
  }

  static bool sizeContainsSrcSize(RegSize dstSize, RegSize immSize);

  inline bool containsSize(RegSize immSize) const {
    return sizeContainsSrcSize(getSize(), immSize);
  }
  String toString() const {
    return getName();
  }
};

class GPRegister : public Register {
private:
  const RegSize      m_size; // = REGSIZE_BYTE, _WORD, _DWORD, _QWORD
#ifdef IS64BIT
  const RexByteUsage m_rexByteUsage;
#endif // IS64BIT
public:
  inline GPRegister(RegSize size
                   ,BYTE index
#ifdef IS64BIT
                   ,RexByteUsage rexByteUsage=REX_DONTCARE
#endif // IS64BIT
                   )
    : Register(index)
    , m_size(size)
#ifdef IS64BIT
    , m_rexByteUsage(rexByteUsage)
#endif // IS64BIT
  {
    SETDEBUGSTR();
  }
  RegType getType()  const {
    return REGTYPE_GPR;
  }
  RegSize getSize()  const {
    return m_size;
  }
#ifdef IS64BIT
  bool    isREXCompatible(bool rexBytePresent) const;
  RexByteUsage getRexByteUsage() const {
    return m_rexByteUsage;
  }
#endif // IS64BIT
  String getName() const;
};

class IndexRegister : public GPRegister {
public:
  inline IndexRegister(BYTE index) : GPRegister(INDEX_REGSIZE, index) {
  }
  inline bool isValidIndexRegister() const {
    return (getIndex()&7)!=4;
  }
};

class FPURegister : public Register {
public:
  inline FPURegister(BYTE index) : Register(index) {
    SETDEBUGSTR();
  }
  RegType getType()  const {
    return REGTYPE_FPU;
  }
  RegSize getSize()  const {
    return REGSIZE_TBYTE;
  }
  String getName() const;
};

class XMMRegister : public Register {
public:
  inline XMMRegister(BYTE index) : Register(index) {
    SETDEBUGSTR();
  }
  RegType getType()  const {
    return REGTYPE_XMM;
  }
  RegSize getSize()  const {
    return REGSIZE_OWORD;
  }
  String getName() const;
};

class SegmentRegister : public Register {
public:
  inline SegmentRegister(BYTE index) : Register(index) {
    SETDEBUGSTR();
  }
  RegType getType()  const {
    return REGTYPE_SEG;
  }
  RegSize getSize()  const {
    return REGSIZE_WORD;
  }
  String getName() const;
};

// 8 bit registers
extern const GPRegister    AL,CL,DL,BL,AH,CH,DH,BH;
#ifdef IS64BIT
extern const GPRegister    SPL,BPL,SIL,DIL;
#endif

// 16 bit registers
extern const GPRegister    AX,CX,DX,BX,SP,BP,SI,DI;

#ifdef IS32BIT
// 32 bit registers
extern const IndexRegister EAX ,ECX ,EDX ,EBX ,ESP ,EBP ,ESI ,EDI;
#else // IS64BIT
// 8 bit registers  (only x64)
extern const GPRegister    R8B ,R9B ,R10B,R11B,R12B,R13B,R14B,R15B;

// 16 bit registers (only x64)
extern const GPRegister    R8W ,R9W ,R10W,R11W,R12W,R13W,R14W,R15W;

// 32 bit registers (EAX-EDI are not IndexRegisters, only GPRegisters
extern const GPRegister    EAX ,ECX ,EDX ,EBX ,ESP ,EBP ,ESI ,EDI;
// 32 bit registers (only x64)
extern const GPRegister    R8D ,R9D ,R10D,R11D,R12D,R13D,R14D,R15D;

// 64 bit registers (only x64)
extern const IndexRegister RAX ,RCX ,RDX ,RBX ,RSP ,RBP ,RSI ,RDI;
extern const IndexRegister R8  ,R9  ,R10 ,R11 ,R12 ,R13 ,R14 ,R15;

#endif // IS64BIT

extern const FPURegister ST0,ST1,ST2,ST3,ST4,ST5,ST6,ST7;

extern const XMMRegister XMM0,XMM1,XMM2,XMM3,XMM4,XMM5,XMM6,XMM7;

#ifdef IS64BIT
extern const XMMRegister XMM8,XMM9,XMM10,XMM11,XMM12,XMM13,XMM14,XMM15;
#endif // IS64BIT

// Segment registers (16-bit)
#ifdef IS32BIT
extern const SegmentRegister ES,CS,SS,DS,FS,GS;
#else // IS64BIT
extern const SegmentRegister FS,GS;
#endif
