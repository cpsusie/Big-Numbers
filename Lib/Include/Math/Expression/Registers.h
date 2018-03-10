#pragma once

typedef enum {
  REGTYPE_NONE
 ,REGTYPE_GP
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
} RegSize;

const TCHAR *getOpSizeName(RegSize regSize);

#ifdef IS32BIT
#define INDEX_REGSIZE          REGSIZE_DWORD
#define MAX_GPREGISTER_INDEX   7
#define MAX_XMMREGISTER_INDEX  7
typedef int                    MovMaxImmType;
#else
#define INDEX_REGSIZE         REGSIZE_QWORD
#define MAX_GPREGISTER_INDEX  15
#define MAX_XMMREGISTER_INDEX 15
typedef INT64                 MovMaxImmType;
#endif

class Register {
private:
  const BYTE m_index; // = [0..15]
#ifdef _DEBUG
protected:
  String m_name;
#define SETNAME() m_name = getName()
#else
#define SETNAME()
#endif // _DEBUG
  Register &operator=(const Register &); // not implemented, and not accessible.
                                          // All register are singletons
public:
  inline Register(BYTE index) : m_index(index) {
  }
  inline UINT getIndex() const {
    return m_index;
  }
  virtual RegType getType()  const = 0;
  virtual RegSize getSize()  const = 0;
  virtual String  getName()  const {
    return format(_T("Unknown register:(type,sz,index):(%d,%d,%u"), getType(),getSize(),getIndex());
  }
  inline bool operator==(const Register &r) const {
    return getType() == r.getType() && getSize() == r.getSize() && getIndex() == r.getIndex();
  }
  inline bool operator!=(const Register &r) const {
    return !(*this == r);
  }
};

class GPRegister : public Register {
private:
  const RegSize m_size; // = REGSIZE_BYTE, _WORD, _DWORD, _QWORD
public:
  inline GPRegister(RegSize size, BYTE index) : Register(index), m_size(size) {
    SETNAME();
  }
  RegType getType()  const {
    return REGTYPE_GP;
  }
  RegSize getSize()  const {
    return m_size;
  }
  String getName() const;
};

class IndexRegister : public GPRegister {
public:
  inline IndexRegister(BYTE index) : GPRegister(INDEX_REGSIZE, index) {
  }
};

class FPURegister : public Register {
public:
  inline FPURegister(BYTE index) : Register(index) {
    SETNAME();
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
    SETNAME();
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
    SETNAME();
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
extern const SegmentRegister ES,CS,SS,DS,FS,GS;
