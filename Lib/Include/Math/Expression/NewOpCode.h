#pragma once

#include <MyUtil.h>
#include <MyAssert.h>
#include "Registers.h"

inline bool isByte( int    v) {
  return v == (char)v;
}
inline bool isWord( int    v) {
  return v == (short)v;
}
inline bool isDword(int    v) {
  return true;
}
inline bool isByte( UINT   v) {
  return v == (BYTE)v;
}
inline bool isWord( UINT   v) {
  return v == (USHORT)v;
}
inline bool isDword(UINT   v) {
  return true;
}
inline bool isByte( INT64  v) {
  return v == (char)v;
}
inline bool isWord( INT64  v) {
  return v == (short)v;
}
inline bool isDword(INT64  v) {
  return v == (int)v;
}
inline bool isByte( UINT64 v) {
  return v == (BYTE)v;
}
inline bool isWord( UINT64 v) {
  return v == (USHORT)v;
}
inline bool isDword(UINT64 v) {
  return v == (UINT)v;
}

typedef RegSize OperandSize;

class MemoryRef;

typedef enum {
  REGISTER
 ,MEMORYOPERAND
 ,IMMEDIATEVALUE
} OperandType;

String toString(OperandType type);
// Convert int32-value to disassembler format
String formatHexValue(int v, bool showSign);
// Convert int64-value to disassembler format
String formatHexValue(INT64 v, bool showSign);
String getImmSizeErrorString(const String &dst, INT64 immv);

class InstructionOperand {
private:
  const OperandType m_type;
  const OperandSize m_size;
  union {
    const Register *m_reg;
    BYTE            m_v8;
    WORD            m_v16;
    DWORD           m_v32;
    UINT64          m_v64;
  };

  static OperandSize findMinSize(int    v);
  static OperandSize findMinSize(UINT   v);
  static OperandSize findMinSize(INT64  v);
  static OperandSize findMinSize(UINT64 v);
  void setValue(int    v);
  void setValue(UINT   v);
  void setValue(INT64  v);
  void setValue(UINT64 v);
  void throwUnknownSize(const TCHAR *method) const;
  void validateSize(    const TCHAR *method, OperandSize expectedSize) const;
  void validateType(    const TCHAR *method, OperandType expectedType) const;

  DECLAREDEBUGSTR;

public:
  inline InstructionOperand(OperandType type, OperandSize size)
    : m_type(type)
    , m_size(size)
    , m_reg( NULL)
  {
  }
  inline InstructionOperand(const Register &reg)
    : m_type(REGISTER)
    , m_size(reg.getSize())
    , m_reg(&reg)
  {
    SETDEBUGSTR();
  }
  inline InstructionOperand(int    v)
    : m_type(IMMEDIATEVALUE)
    , m_size(findMinSize(v))
  {
    setValue(v);
    SETDEBUGSTR();
  }
  inline InstructionOperand(UINT   v)
    : m_type(IMMEDIATEVALUE)
    , m_size(findMinSize(v))
  {
    setValue(v);
    SETDEBUGSTR();
  }
  inline InstructionOperand(INT64  v)
    : m_type(IMMEDIATEVALUE)
    , m_size(findMinSize(v))
  {
    setValue(v);
    SETDEBUGSTR();
  }
  inline InstructionOperand(UINT64 v)
    : m_type(IMMEDIATEVALUE)
    , m_size(findMinSize(v))
  {
    setValue(v);
    SETDEBUGSTR();
  }
  inline OperandType getType() const {
    return m_type;
  }
  inline OperandSize getSize() const {
    return m_size;
  }
  inline const Register &getRegister() const {
    validateType(__TFUNCTION__,REGISTER);
    return *m_reg;
  }
  char   getImmInt8()   const;
  BYTE   getImmUint8()  const;
  short  getImmInt16()  const;
  USHORT getImmUint16() const;
  int    getImmInt32()  const;
  UINT   getImmUint32() const;
  INT64  getImmInt64()  const;
  UINT64 getImmUInt64() const;
  inline bool isShiftAmountOperand() const {
    return (getSize() == REGSIZE_BYTE)
        &&  ((getType()==IMMEDIATEVALUE)
          || ((getType()==REGISTER) && (getRegister() == CL)));
  }
  virtual const SegmentRegister &getSegmentRegister() const {
    throwUnsupportedOperationException(__TFUNCTION__);
    return ES;
  }
  virtual bool                   hasSegmentRegister() const {
    return false;
  }
  virtual const MemoryRef       *getMemoryReference() const {
    throwUnsupportedOperationException(__TFUNCTION__);
    return NULL;
  }
#ifdef IS64BIT
  virtual bool  needREXByte() const {
    return (getType() == REGISTER) ? m_reg->indexNeedREXByte() : false;
  }
#endif // IS64BIT
  virtual String toString() const;
};

class MemoryRef {
private:
  const IndexRegister *m_base,*m_inx;
  const BYTE           m_shift;
  const int            m_offset;
#ifdef IS64BIT
  const bool           m_needREXByte;
  static inline bool findRexByteNeeded(const IndexRegister *base, const IndexRegister *inx) {
    return (base && base->indexNeedREXByte()) || (inx && inx->indexNeedREXByte());
  }
#define SETNEEDREXBYTE(base,inx) ,m_needREXByte(findRexByteNeeded(base,inx))
#else
#define SETNEEDREXBYTE(base,inx)
#endif // IS64BIT

  DECLAREDEBUGSTR;

public:
  inline MemoryRef(const IndexRegister *base, const IndexRegister *inx, BYTE shift=0, int offset=0)
    : m_base(  base  )
    , m_inx(   inx   )
    , m_shift( shift )
    , m_offset(offset)
    SETNEEDREXBYTE(base,inx)
  {
    SETDEBUGSTR();
  }
  inline MemoryRef(const IndexRegister &base)
    : m_base( &base  )
    , m_inx(   NULL  )
    , m_shift( 0     )
    , m_offset(0     )
    SETNEEDREXBYTE(&base,NULL)
  {
    SETDEBUGSTR();
  }
  inline MemoryRef(DWORD addr)
    : m_base(  NULL  )
    , m_inx(   NULL  )
    , m_shift( 0     )
    , m_offset(addr  )
    SETNEEDREXBYTE(NULL,NULL)
  {
    SETDEBUGSTR();
  }
  inline bool isDisplaceOnly() const {
    return (m_base == NULL) && (m_inx == NULL);
  }
  // set m_needREXByte in x64
  void sortBaseInx();
  inline const IndexRegister *getBase()   const { return m_base;         }
  inline const IndexRegister *getInx()    const { return m_inx;          }
  inline BYTE                 getShift()  const { return m_shift;        }
  inline int                  getOffset() const { return m_offset;       }
  inline bool                 hasBase()   const { return m_base != NULL; }
  inline bool                 hasInx()    const { return m_inx  != NULL; }
  inline bool                 hasShift()  const { return m_shift >= 1;   }
  inline bool                 hasOffset() const { return m_offset != 0;  }

#ifdef IS64BIT
  inline bool                 needREXByte() const {
    return m_needREXByte;
  }
#endif // IS64BIT
  String                      toString()  const;
};

MemoryRef operator+(const IndexRegister &base, int offset);
MemoryRef operator-(const IndexRegister &base, int offset);
MemoryRef operator+(const MemoryRef     &mr  , int offset);
MemoryRef operator-(const MemoryRef     &mr  , int offset);
MemoryRef operator+(const IndexRegister &base, const MemoryRef     &mr );
MemoryRef operator+(const IndexRegister &base, const IndexRegister &inx);
MemoryRef operator*(BYTE a, const IndexRegister &inx);
MemoryRef operator*(const IndexRegister &inx, BYTE a);

class MemoryOperand : public InstructionOperand {
private:
  const SegmentRegister *m_segReg;
  MemoryRef              m_mr;
public:
  inline MemoryOperand(OperandSize size, const MemoryRef &mr, const SegmentRegister *segReg=NULL)
    : InstructionOperand(MEMORYOPERAND, size)
    , m_segReg(segReg)
    , m_mr(mr)
  {
    m_mr.sortBaseInx();
    SETDEBUGSTR();
  }
  inline MemoryOperand(OperandSize size, DWORD addr, const SegmentRegister *segReg=NULL)
    : InstructionOperand(MEMORYOPERAND, size)
    , m_segReg(segReg)
    , m_mr(addr)
  {
    SETDEBUGSTR();
  }
  const SegmentRegister &getSegmentRegister() const {
    return *m_segReg;
  }
  bool                   hasSegmentRegister() const {
    return m_segReg != NULL;
  }
  const MemoryRef       *getMemoryReference() const {
    return &m_mr;
  }
#ifdef IS64BIT
  bool needREXByte() const {
    return m_mr.needREXByte();
  }
#endif // IS64BIT
  String toString() const;
};

template<OperandSize size> class MemoryPtr : public MemoryOperand {
public:
  MemoryPtr(const MemoryRef &mr) : MemoryOperand(size, mr) {
  }
#ifdef IS32BIT
  MemoryPtr(const SegmentRegister &segReg, const MemoryRef &mr) : MemoryOperand(size, mr, &segReg) {
  }
  MemoryPtr(const SegmentRegister &segReg, DWORD addr) : MemoryOperand(size, addr, &segReg) {
  }
#else // IS64BIT
  MemoryPtr(DWORD addr) : MemoryOperand(size, addr) {
  }
#endif // IS64BIT
};

typedef MemoryPtr<REGSIZE_BYTE > BYTEPtr;
typedef MemoryPtr<REGSIZE_WORD > WORDPtr;
typedef MemoryPtr<REGSIZE_DWORD> DWORDPtr;
typedef MemoryPtr<REGSIZE_QWORD> QWORDPtr;
typedef MemoryPtr<REGSIZE_OWORD> XMMWORDPtr;
typedef MemoryPtr<REGSIZE_TBYTE> TBYTEPtr;

#define MAX_INSTRUCTIONSIZE   15

class OpcodeBase;
class Opcode0Arg;

class InstructionBase {
protected:
  BYTE   m_bytes[15];
  UINT   m_size            : 4; // = [0..15]
  InstructionBase(const OpcodeBase &opcode);
public:
  InstructionBase(const Opcode0Arg &opcode);
  // In bytes
  inline UINT size() const {
    return m_size;
  }
  inline const BYTE *getBytes() const {
    return m_bytes;
  }
  String toString() const;
};

// ----------------------------------------------------------------------
#define REGTYPE_NONE_ALLOWED   0x00000001
#define REGTYPE_GPR_ALLOWED    0x00000002
#define REGTYPE_SEG_ALLOWED    0x00000004
#define REGTYPE_FPU_ALLOWED    0x00000008
#define REGTYPE_XMM_ALLOWED    0x00000010
#define REGSIZE_BYTE_ALLOWED   0x00000020
#define REGSIZE_WORD_ALLOWED   0x00000040
#define REGSIZE_DWORD_ALLOWED  0x00000080
#define REGSIZE_QWORD_ALLOWED  0x00000100
#define REGSIZE_TBYTE_ALLOWED  0x00000200
#define REGSIZE_OWORD_ALLOWED  0x00000400
#define BYTEPTR_ALLOWED        0x00000800
#define WORDPTR_ALLOWED        0x00001000
#define DWORDPTR_ALLOWED       0x00002000
#define QWORDPTR_ALLOWED       0x00004000
#define TBYTEPTR_ALLOWED       0x00008000
#define OWORDPTR_ALLOWED       0x00010000
#define IMMEDIATEVALUE_ALLOWED 0x00020000
#define HAS_SIZEBIT            0x00040000
#define HAS_DIRECTIONBIT       0x00080000

#ifdef IS32BIT
#define ALL_GPRSIZE_ALLOWED     (REGSIZE_BYTE_ALLOWED | REGSIZE_WORD_ALLOWED | REGSIZE_DWORD_ALLOWED)
#define ALL_GPRSIZEPTR_ALLOWED  (BYTEPTR_ALLOWED      | WORDPTR_ALLOWED      | DWORDPTR_ALLOWED     )
#else  // IS64BIT
#define ALL_GPRSIZE_ALLOWED     (REGSIZE_BYTE_ALLOWED | REGSIZE_WORD_ALLOWED | REGSIZE_DWORD_ALLOWED | REGSIZE_QWORD_ALLOWED)
#define ALL_GPRSIZEPTR_ALLOWED  (BYTEPTR_ALLOWED      | WORDPTR_ALLOWED      | DWORDPTR_ALLOWED      | QWORDPTR_ALLOWED     )
#endif // IS64BIT

#define ALL_REGISTER_TYPES     (REGTYPE_GPR_ALLOWED  | REGTYPE_SEG_ALLOWED  | REGTYPE_FPU_ALLOWED | REGTYPE_XMM_ALLOWED)
#define ALL_GPR_ALLOWED        (REGTYPE_GPR_ALLOWED  | ALL_GPRSIZE_ALLOWED)
#define ALL_GPRPTR_ALLOWED     (ALL_GPRSIZEPTR_ALLOWED)

#define ALL_MEMOPSIZES         (BYTEPTR_ALLOWED | WORDPTR_ALLOWED | DWORDPTR_ALLOWED | QWORDPTR_ALLOWED | TBYTEPTR_ALLOWED | OWORDPTR_ALLOWED )

class OpcodeBase {
private:
  static const RegSizeSet s_wordRegCapacity  , s_dwordRegCapacity, s_qwordRegCapacity;
  static const RegSizeSet s_validImmSizeToMem, s_validImmSizeToReg;

  UINT         m_bytes;
  const UINT   m_flags;
  const String m_mnemonic;
  // Opcode extension 0..7
  const UINT   m_extension : 3;
  // In bytes 0..3
  const UINT   m_size      : 2;
  // Number of operands
  const UINT   m_opCount   : 3;

protected:
  OpcodeBase(const String &mnemonic, const InstructionBase &src, BYTE extension=0, UINT flags=0);
  void throwInvalidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2) const;
  void throwInvalidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
  void throwInvalidOperandType(       const InstructionOperand &op, BYTE index) const;
  static void throwUnknownOperandType(const TCHAR *method, OperandType type);
  bool isRegisterTypeAllowed(     RegType     type) const;
  bool isRegisterSizeAllowed(     RegSize     size) const;
  bool isMemoryOperandSizeAllowed(OperandSize size) const;

  inline bool isRegisterAllowed() const {
    return (getFlags() & ALL_REGISTER_TYPES) != 0;
  }
  inline bool isMemoryOperandAllowed() const {
    return (getFlags() & ALL_MEMOPSIZES) != 0;
  }
  inline bool isImmediateValueAllowed() const {
    return (getFlags() & IMMEDIATEVALUE_ALLOWED) != 0;
  }
  inline bool isRegisterAllowed(const Register &reg) const {
    return isRegisterTypeAllowed(reg.getType()) && isRegisterSizeAllowed(reg.getSize());
  }
  inline bool isMemoryOperandAllowed(const MemoryOperand &mem) const {
    return isMemoryOperandSizeAllowed(mem.getSize());
  }
  static bool sizeContainsSrcSize(OperandSize dstSize, OperandSize srcSize);

  bool validateOpCount(                  int                       count                               , bool throwOnError) const;
  bool validateRegisterAllowed(          const Register           &reg                                 , bool throwOnError) const;
  bool validateMemoryOperandAllowed(     const MemoryOperand      &mem                                 , bool throwOnError) const;
  bool validateImmediateValueAllowed(                                                                    bool throwOnError) const;
  bool validateImmediateValue(           OperandSize dstSize           , const InstructionOperand &imm , const RegSizeSet *regSizeSet, bool throwOnError) const;
  bool validateSameSize(                 const Register           &reg1, const Register           &reg2, bool throwOnError) const;
  bool validateSameSize(                 const Register           &reg , const InstructionOperand &op  , bool throwOnError) const;
  bool validateSameSize(                 const InstructionOperand &op1 , const InstructionOperand &op2 , bool throwOnError) const;
  bool validateRegisterOperand(          const InstructionOperand &op  , int   index                   , bool throwOnError) const;
  bool validateRegisterOrMemoryOperand(  const InstructionOperand &op  , int   index                   , bool throwOnError) const;
  bool validateShiftAmountOperand(       const InstructionOperand &op  , int   index                   , bool throwOnError) const;

#ifdef IS32BIT
  inline bool validateIsRexCompatible(   const Register           &reg , const InstructionOperand &op  , bool throwOnError) const {
    return true;
  }
  inline bool validateIsRexCompatible(   const Register           &reg1, const Register           &reg2, bool throwOnError) const {
    return true;
  }
#else // IS64BIT
          bool validateIsRexCompatible(  const Register           &reg , const InstructionOperand &op  , bool throwOnError) const;
          bool validateIsRexCompatible(  const Register           &reg1, const Register           &reg2, bool throwOnError) const;
#endif // IS64BIT
  virtual bool isValidOperandCombination(const Register           &reg , const InstructionOperand &op  , bool throwOnError) const;
public:
  OpcodeBase(const String &mnemonic, UINT op, BYTE extension, BYTE opCount, UINT flags);

  const String &getMnemonic() const {
    return m_mnemonic;
  }
  // Size of Opcode in bytes
  inline BYTE size() const {
    return m_size;
  }
  // Opcode extension. value [0..7]
  inline BYTE getExtension() const {
    return m_extension;
  }
  // Number of operands
  inline BYTE getOpCount() const {
    return m_opCount;
  }
  // Raw opcode bytes
  inline const BYTE *getBytes() const {
    return (BYTE*)&m_bytes;
  }
  // Various attributes
  inline UINT getFlags() const {
    return m_flags;
  }
  virtual bool isValidOperand(           const InstructionOperand &op                                , bool throwOnError=false) const;
  virtual bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  virtual bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError=false) const;
  virtual InstructionBase operator()(    const InstructionOperand &op) const;
  virtual InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2) const;
  virtual InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
};

class Opcode0Arg : public OpcodeBase {
public:
  inline Opcode0Arg(const String &mnemonic, UINT op, UINT flags=0)
    : OpcodeBase(mnemonic, op, 0, 0, flags)
  {
  }
  Opcode0Arg(const String &mnemonic, const Opcode0Arg &op, OperandSize size);
};

class Opcode1Arg : public OpcodeBase {
public:
  inline Opcode1Arg(const String &mnemonic, UINT op, BYTE extension, UINT flags=ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | HAS_SIZEBIT)
    : OpcodeBase(mnemonic, op, extension, 1, flags)
  {
  }
  InstructionBase operator()(const InstructionOperand &op) const;
};

class Opcode2Arg : public OpcodeBase {
public :
  Opcode2Arg(const String &mnemonic, UINT op, UINT flags=ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | IMMEDIATEVALUE_ALLOWED | HAS_SIZEBIT | HAS_DIRECTIONBIT)
    : OpcodeBase(mnemonic, op, 0, 2, flags) {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeMovRegImm : public Opcode2Arg {
public:
  OpcodeMovRegImm(const String &mnemonic, BYTE op) : Opcode2Arg(mnemonic, op) {
  }
  InstructionBase operator()(const Register &reg, const InstructionOperand &imm) const;
  bool isValidOperandCombination(const Register &reg, const InstructionOperand &imm, bool throwOnError) const;
};

class OpcodeMovMemImm : public Opcode2Arg {
public:
  OpcodeMovMemImm(const String &mnemonic, BYTE op) : Opcode2Arg(mnemonic, op) {
  }
  InstructionBase operator()(const InstructionOperand &mem, const InstructionOperand &imm) const;
};

class OpcodeMov : public Opcode2Arg {
private:
  const OpcodeMovRegImm m_regImmCode;
  const OpcodeMovMemImm m_memImmCode;
protected:
  bool isValidOperandCombination(const Register &reg, const InstructionOperand &op2, bool throwOnError) const;
public :
  OpcodeMov(const String &mnemonic, BYTE op, BYTE regImmOp, BYTE memImmOp)
    : Opcode2Arg(  mnemonic, op)
    , m_regImmCode(mnemonic, regImmOp)
    , m_memImmCode(mnemonic, memImmOp)
  {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeSetxx : public Opcode1Arg {
public:
  OpcodeSetxx(const String &mnemonic, UINT op) : Opcode1Arg(mnemonic, op, 0, REGTYPE_GPR_ALLOWED | REGSIZE_BYTE_ALLOWED | BYTEPTR_ALLOWED) {
  }
};

class OpcodeShiftRot : public OpcodeBase {
private:
  OpcodeBase m_immCode;
public:
  OpcodeShiftRot(const String &mnemonic, BYTE extension);
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
};

class OpcodeDoubleShift : public OpcodeBase {
private:
  OpcodeBase m_immCode;
public:
  OpcodeDoubleShift(const String &mnemonic, UINT opCL, UINT opImm);
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError=false) const;
};

class OpcodeBitScan : public Opcode2Arg {
public:
  OpcodeBitScan(const String &mnemonic, UINT op);
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
};

class StringInstruction : public Opcode0Arg {
public:
  StringInstruction(const String &mnemonic, UINT op) : Opcode0Arg(mnemonic, op, HAS_SIZEBIT) {
  }
  StringInstruction(const String &mnemonic, const StringInstruction &ins, OperandSize size) : Opcode0Arg(mnemonic, ins, size) {
  }
};

class StringPrefix : public OpcodeBase {
public:
  StringPrefix(const String &mnemonic, BYTE op) : OpcodeBase(mnemonic,op,0,0,0) {
  }
  InstructionBase operator()(const StringInstruction &ins) const;
};

// Set Byte on Condition
extern OpcodeSetxx       SETO;                             // Set byte   if overflow
extern OpcodeSetxx       SETNO;                            // Set byte   if not overflow
extern OpcodeSetxx       SETB;                             // Set byte   if below                 (unsigned)
extern OpcodeSetxx       SETAE;                            // Set byte   if above or equal        (unsigned)
extern OpcodeSetxx       SETE;                             // Set byte   if equal                 (signed/unsigned)
extern OpcodeSetxx       SETNE;                            // Set byte   if not equal             (signed/unsigned)
extern OpcodeSetxx       SETBE;                            // Set byte   if below or equal        (unsigned)
extern OpcodeSetxx       SETA;                             // Set byte   if above                 (unsigned)
extern OpcodeSetxx       SETS;                             // Set byte   if sign
extern OpcodeSetxx       SETNS;                            // Set byte   if not sign
extern OpcodeSetxx       SETPE;                            // Set byte   if parity even
extern OpcodeSetxx       SETPO;                            // Set byte   if parity odd
extern OpcodeSetxx       SETL;                             // Set byte   if less                  (signed  )
extern OpcodeSetxx       SETGE;                            // Set byte   if greater or equal      (signed  )
extern OpcodeSetxx       SETLE;                            // Set byte   if less or equal         (signed  )
extern OpcodeSetxx       SETG;                             // Set byte   if greater               (signed  )

#define                  SETNAE         SETB               // Set byte   if not above or equal    (unsigned)
#define                  SETC           SETB               // Set byte   if carry                 (unsigned)
#define                  SETNC          SETAE              // Set byte   if not carry             (unsigned)
#define                  SETNB          SETAE              // Set byte   if not below             (unsigned)
#define                  SETZ           SETE               // Set byte   if 0                     (signed/unsigned)
#define                  SETNZ          SETNE              // Set byte   if not zero              (signed/unsigned)
#define                  SETNA          SETBE              // Set byte   if not above             (unsigned)
#define                  SETNBE         SETA               // Set byte   if not below or equal    (unsigned)
#define                  SETNGE         SETL               // Set byte   if not greater or equal  (signed  )
#define                  SETNL          SETGE              // Set byte   if not less              (signed  )
#define                  SETNG          SETLE              // Set byte   if not greater           (signed  )
#define                  SETNLE         SETG               // Set byte   if not less or equal     (signed  )

extern Opcode0Arg        RET;                              // Near return to calling procedure

extern Opcode0Arg        CMC;                              // Complement carry flag
extern Opcode0Arg        CLC;                              // Clear carry flag     CF = 0
extern Opcode0Arg        STC;                              // Set   carry flag     CF = 1
extern Opcode0Arg        CLI;                              // Clear interrupt flag IF = 0
extern Opcode0Arg        STI;                              // Set   interrupt flag IF = 1
extern Opcode0Arg        CLD;                              // Clear direction flag DF = 0
extern Opcode0Arg        STD;                              // Set   direction flag DF = 1

#ifdef IS64BIT
extern Opcode0Arg        CLGI;                             // Clear Global Interrupt Flag
extern Opcode0Arg        STGI;                             // Set Global Interrupt Flag
#endif // IS64BIT

extern Opcode0Arg        PUSHF;                            // Push FLAGS  onto stack         { sp-=2, *sp = FLAGS; }
extern Opcode0Arg        POPF;                             // Pop  FLAGS register from stack { FLAGS = *SP; sp+=2; }
extern Opcode0Arg        SAHF;                             // Store AH into FLAGS
extern Opcode0Arg        LAHF;                             // Load FLAGS into AH register

#ifdef IS32BIT
extern Opcode0Arg        PUSHAD;                           // Push all double-word (32-bit) registers onto stack
extern Opcode0Arg        POPAD;                            // Pop  all double-word (32-bit) registers from stack
extern Opcode0Arg        PUSHFD;                           // Push EFLAGS register onto stack { sp-=4, *sp = EFLAGS; }
extern Opcode0Arg        POPFD;                            // Pop data into EFLAGS register   { EFLAGS = *SP; sp+=4; }
#else // IS64BIT
extern Opcode0Arg        PUSHFQ;                           // Push RFLAGS register onto stack
extern Opcode0Arg        POPFQ;                            // Pop data into RFLAGS register
#endif // IS64BIT

extern Opcode0Arg        NOOP;                             // No operation
extern Opcode2Arg        ADD;                              // Integer Addition
extern Opcode2Arg        ADC;                              // Integer Addition with Carry
extern Opcode2Arg        OR;                               // Logical Inclusive OR
extern Opcode2Arg        AND;                              // Logical AND
extern Opcode2Arg        SUB;                              // Integer Subtraction
extern Opcode2Arg        SBB;                              // Integer Subtraction with Borrow
extern Opcode2Arg        XOR;                              // Logical Exclusive OR
extern Opcode2Arg        CMP;                              // Compare Two Operands
extern OpcodeMov         MOV;                              // Move data (copying)

extern Opcode1Arg        NOT;                              // Negate the operand, logical NOT
extern Opcode1Arg        NEG;                              // Two's complement negation
extern Opcode1Arg        MUL;                              // Unsigned multiply ah:al=al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src
extern Opcode1Arg        IMUL;                             // Signed multiply   ah:ax=al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src

extern Opcode1Arg        DIV;                              // Unsigned divide   ah:al   /= src, al  = quot, ah  = rem
                                                           //                   dx:ax   /= src, ax  = quot, dx  = rem  
                                                           //                   edx:eax /= src, eax = quot, edx = rem  
                                                           //                   rdx:rax /= src, rax = quot, rdx = rem
extern Opcode1Arg        IDIV;                             // Signed divide     ah:al   /= src, al  = quot, ah  = rem. ah  must contain sign extension of al.
                                                           //                   dx:ax   /= src. ax  = quot, dx  = rem. dx  must contain sign extension of ax.
                                                           //                   edx:eax /= src. eax = quot, edx = rem. edx must contain sign extension of eax.
                                                           //                   rdx:rax /= src. rax = quot, rdx = rem. rdx must contain sign extension of rax.

extern OpcodeShiftRot    ROL;                              // Rotate left  by cl/imm
extern OpcodeShiftRot    ROR;                              // Rotate right by cl/imm
extern OpcodeShiftRot    RCL;                              // Rotate left  by cl/imm (with carry)
extern OpcodeShiftRot    RCR;                              // Rotate right by cl/imm (with carry)
extern OpcodeShiftRot    SHL;                              // Shift  left  by cl/imm                 (unsigned shift left )
extern OpcodeShiftRot    SHR;                              // Shift  right by cl/imm                 (unsigned shift right)
#define                  SAL SHL                           // Shift  Arithmetically left  by cl/imm  (signed shift   left - same as shl)
extern OpcodeShiftRot    SAR;                              // Shift  Arithmetically right by cl/imm  (signed shift   right)

extern OpcodeDoubleShift SHLD;                             // Shift left  by cl/imm, filling opened bitpositions, by most significant bits of reg
extern OpcodeDoubleShift SHRD;                             // Shift right by cl/imm, filling opened bitpositions, by least significant bits of reg

extern OpcodeBitScan     BSF;                              // Bitscan forward
extern OpcodeBitScan     BSR;                              // Bitscan reversed

extern Opcode0Arg        CBW;                              // Convert byte  to word.  Sign extend AL  into AX.      Copy sign (bit  7) of AL  into higher  8 bits of AX
extern Opcode0Arg        CWDE;                             // Convert word  to dword. Sign extend AX  into EAX.     Copy sign (bit 15) of AX  into higher 16 bits of EAX
extern Opcode0Arg        CWD;                              // Convert word  to dword. Sign extend AX  into DX:AX.   Copy sign (bit 15) of AX  into every     bit  of DX
extern Opcode0Arg        CDQ;                              // Convert dword to qword. Sign extend EAX into EDX:EAX. Copy sign (bit 31) of EAX into every     bit  of EDX

#ifdef IS64BIT
extern Opcode0Arg        CDQE;                             // Convert dword to qword. Sign extend EAX into RAX.     Copy sign (bit 31) of EAX into higher 32 bits of RAX
extern Opcode0Arg        CQO;                              // Convert qword to oword. Sign extend RAX into RDX:RAX. Copy sign (bit 63) of RAX into every     bit  of RDX
#endif // IS64BIT

//#define INS_BYTE                               B1INS(0x6C)
//#define OUTS_BYTE                              B1INS(0x6E)

extern StringInstruction MOVSB;                            // Move byte from string to string; if(DF==0) *(byte*)DI++ = *(byte*)SI++;      else *(byte*)DI-- = *(byte*)SI--;
extern StringInstruction CMPSB;                            // Compare bytes in memory        ; if(DF==0) Compares ES:[DI++] with DS:[SI++] else Compares ES:[DI--] with DS:[SI--] 
extern StringInstruction STOSB;                            // Store byte in string           ; if(DF==0) *ES:DI++     = AL;                else *ES:DI--     = AL;
extern StringInstruction LODSB;                            // Load string byte               ; if(DF==0)     AL       = *SI++;             else     AL       = *SI--;
extern StringInstruction SCASB;                            // Compare byte string            ; if(DF==0) Compares ES:[DI++] with AL        else Compares ES:[DI--] with AL

// Same as MOVSB...,but with WORD, and AL -> AX
extern StringInstruction MOVSW;
extern StringInstruction CMPSW;
extern StringInstruction STOSW;
extern StringInstruction LODSW;
extern StringInstruction SCASW;

// Same as MOVSB...,but with DWORD, and AL->EAX, SI->ESI,DI->EDI
extern StringInstruction MOVSD;
extern StringInstruction CMPSD;
extern StringInstruction STOSD;
extern StringInstruction LODSD;
extern StringInstruction SCASD;

#ifdef IS64BIT
// Same as MOVSB...,but with QWORD, and AL -> RAX, SI->RSI,DI->RDI
extern StringInstruction MOVSQ;
extern StringInstruction CMPSQ;
extern StringInstruction STOSQ;
extern StringInstruction LODSQ;
extern StringInstruction SCASQ;
#endif // IS64BIT

extern StringPrefix      REP;                              // Apply to INS, OUTS, MOVS, LODS, and STOS
extern StringPrefix      REPE;                             // Apply to CMPS and SCAS instructions
extern StringPrefix      REPNE;                            // Apply to CMPS and SCAS instructions

#ifdef __NEVER__

#define XCHG_EAX_R32(        r32)              B1INS(0x90     |  (r32))                   // r32=eax-edi
#define XCHG_AX_R16(         r16)              WORDOP(XCHG_EAX_R32(r16))                  // r16=ax-di
#define XCHG_REG_MEM(        reg)              B1OPREG(0x86,reg)                          // Build op2 with MEM_ADDR-*,REGREG-macroes


#define MOV_FROM_SEGREG_WORD(seg)              B2OP(0x8C00    | ((seg)<<3))               // Build dst with MEM_ADDR-*,REGREG-macroes
#define LEA_R32_DWORD(       dst)              B2OPNOREGREG(0x8D00 | ((dst)<<3))          // Build src with MEM_ADDR-macroes
#define MOV_TO_SEGREG_WORD(  seg)              B2OP(0x8E00    | ((seg)<<3))               // Build src with MEM_ADDR-*,REGREG-macroes
#define POP_DWORD                              B2OP(0x8F00)                               // Build dst with MEM_ADDR-*,REGREG-macroes


#define MOV_TO_AL_IMM_ADDR_BYTE                B1INS(  0xA0  )                            // 4/8 byte address. move byte  pointed to by 2. operand to AL
#define MOV_TO_EAX_IMM_ADDR_DWORD              B1INS(  0xA1  )                            // 4/8 byte address. move dword pointed to by 2. operand to EAX
#define MOV_TO_AX_IMM_ADDR_WORD                WORDOP( MOV_TO_EAX_IMM_ADDR_DWORD)         // 4/8 byte address. move word  pointed to by 2. operand to AX
#define MOV_TO_RAX_IMM_ADDR_QWORD              REX3(   MOV_TO_EAX_IMM_ADDR_DWORD)         // 8 byte address

#define MOV_FROM_AL_IMM_ADDR_BYTE              B1INS(  0xA2  )                            // 4/8 byte address. move AL  to byte  pointed to by 2. operand
#define MOV_FROM_EAX_IMM_ADDR_DWORD            B1INS(  0xA3  )                            // 4/8 byte address. move EAX to dword pointed to by 2. operand
#define MOV_FROM_AX_IMM_ADDR_WORD              WORDOP( MOV_FROM_EAX_IMM_ADDR_DWORD)       // 4/8 byte address. move AX  to word  pointed to by 2. operand
#define MOV_FROM_RAX_IMM_ADDR_QWORD            REX3(   MOV_FROM_EAX_IMM_ADDR_DWORD)       // 8 byte address

// Additional forms of IMUL
#define IMUL2_R32_DWORD(      r32)             B3OP(0x0FAF00    | ((r32)<<3))             // 2 arguments       (r32 *= src           )
#define IMUL2_R16_WORD(       r16)             WORDOP(IMUL2_R32_DWORD(r16  ))             //                   (r16 *= src           )

#define IMUL3_DWORD_IMM_DWORD(r32)             B2OP(0x6900      | ((r32)<<3))             // 3 args, r32,src,4 byte operand (r32 = src * imm.dword)
#define IMUL3_DWORD_IMM_BYTE( r32)             B2OP(0x6B00      | ((r32)<<3))             // 3 args. r32.src.1 byte operand (r32 = src * imm.byte )

#define IMUL3_WORD_IMM_WORD(  r16)             WORDOP(IMUL3_DWORD_IMM_DWORD(r16))         // 2 byte operand    (r16 = src * imm word )
#define IMUL3_WORD_IMM_BYTE(  r16)             WORDOP(IMUL3_DWORD_IMM_BYTE( r16))         // 1 byte operand    (r16 = src * imm byte )

#define INC_BYTE                               B2OP(0xFE00          )
#define INC_DWORD                              B2OP(0xFF00          )
#define INC_R8( r8 )                           REGREG(INC_BYTE, r8  )

#define DEC_BYTE                               B2OP(0xFE08          )
#define DEC_DWORD                              B2OP(0xFF08          )
#define DEC_R8( r8 )                           REGREG(DEC_BYTE, r8  )

#ifdef IS32BIT

#define INC_R32(r32)                           B1INS(0x40    | r32.getIndex())
#define DEC_R32(r32)                           B1INS(0x48    | r32.getIndex())

#else // IS64BIT

#define INC_R32(r32)                           REGREG( INC_DWORD, r32)
#define INC_QWORD                              REX3(INC_DWORD         )
#define INC_R64(r64)                           REX1(    INC_R32  , r64)

#define DEC_R32(r32)                           REGREG( DEC_DWORD, r32)
#define DEC_QWORD                              REX3(DEC_DWORD         )
#define DEC_R64(r64)                           REX1(    DEC_R32  , r64)

#endif // IS64BIT

#define INC_WORD                               WORDOP(INC_DWORD)
#define INC_R16(r16)                           WORDOP(INC_R32(  r16))

#define DEC_WORD                               WORDOP(DEC_DWORD)
#define DEC_R16(r16)                           WORDOP(DEC_R32(  r16))


#ifdef IS64BIT

#define IMUL2_R64_DWORD(      r64)             REX2(IMUL2_R32_DWORD      ,r64)            // 2 arguments       (r64 *= src           )

#define IMUL3_QWORD_IMM_DWORD(r64)             REX1(IMUL3_DWORD_IMM_DWORD,r64)            // 3 args, r64,src,4 byte operand (r64 = src * imm.dword)
#define IMUL3_QWORD_IMM_BYTE( r64)             REX1(IMUL3_DWORD_IMM_BYTE ,r64)            // 3 args. r64.src.1 byte operand (r64 = src * imm.byte )

#endif // IS64BIT

#define MOVSD_XMM_MMWORD(xmm)                  B3OP1XMM(0xF20F10,xmm)       // Build src with MEM_ADDR-*
#define MOVSD_MMWORD_XMM(xmm)                  B3OP1XMM(0xF20F11,xmm)       // Build dst with MEM_ADDR-*

#define MOVAPS_REG_MEM(   xmm)                 B2OPXMM( 0x0F28,xmm)         // Build op2 with MEM_ADDR-*,REGREG-macroes
#define MOVAPS_MEM_REG(   xmm)                 B2OPXMM( 0x0F29,xmm)         // Build op2 with MEM_ADDR-*,REGREG-macroes

#define ADDSD(xmm)                             B4OP(0xF20F5800 | ((xmm) << 3))               // Build src with MEM_ADDR-*,REGREG-macroes
#define MULSD(xmm)                             B4OP(0xF20F5900 | ((xmm) << 3))
#define SUBSD(xmm)                             B4OP(0xF20F5C00 | ((xmm) << 3))
#define DIVSD(xmm)                             B4OP(0xF20F5E00 | ((xmm) << 3))

// PUSH/POP _R8 not available

#ifdef IS32BIT

#define PUSH_R32(r32)                          B1INS(0x50       | r32.getIndex())         // No operand
#define POP_R32( r32)                          B1INS(0x58       | r32.getIndex())         // No operand

#define PUSH_R16(r16)                          WORDOP(PUSH_R32(r16))                      // No operand
#define POP_R16( r16)                          WORDOP(POP_R32( r16))                      // No operand


#define PUSH_IMM_BYTE                          B1INS(0x6A    )                            // 1 byte value
#define PUSH_IMM_DWORD                         B1INS(0x68    )                            // 4 byte value
#define PUSH_IMM_WORD                          WORDOP(PUSH_IMM_DWORD)                     // 2 byte value

#else // IS64BIT

#define _PUSH_R32(r32)                         B1INS(0x50       | r32.getIndex())         // No operand
#define _POP_R32( r32)                         B1INS(0x58       | r32.getIndex())         // No operand

#define PUSH_R64(r64)                          REX0(_PUSH_R32, r64)                       // No operand
#define POP_R64( r64)                          REX0(_POP_R32 , r64)                       // No operand

#endif // IS64BIT

// FPU instructions

#define FWAIT                                  B1INS(0x9B)                                // Wait while FPU is busy

#define FADD_0i(  i)                           FPUINS(0xD8C0     | (i))                   // st(0) += st(i)
#define FADD_i0(  i)                           FPUINS(0xDCC0     | (i))                   // st(i) += st(0)
#define FADDP_i0( i)                           FPUINS(0xDEC0     | (i))                   // st(i) += st(0); pop st(0)
#define FADD                                   FADDP_i0(1)                                // st(1) += st(0); pop st(0)

#define FMUL_0i(  i)                           FPUINS(0xD8C8     | (i))                   // st(0) *= st(i)
#define FMUL_i0(  i)                           FPUINS(0xDCC8     | (i))                   // st(i) *= st(0)
#define FMULP_i0( i)                           FPUINS(0xDEC8     | (i))                   // st(i) *= st(0); pop st(0)
#define FMUL                                   FMULP_i0(1)                                // st(1) *= st(0); pop st(0)

#define FSUB_0i(  i)                           FPUINS(0xD8E0     | (i))                   // st(0) -= st(i)
#define FSUBR_0i( i)                           FPUINS(0xD8E8     | (i))                   // st(0) =  st(i) - st(0)
#define FSUBR_i0( i)                           FPUINS(0xDCE0     | (i))                   // st(i) =  st(0) - st(i)
#define FSUB_i0(  i)                           FPUINS(0xDCE8     | (i))                   // st(i) -= st(0)
#define FSUBRP_i0(i)                           FPUINS(0xDEE0     | (i))                   // st(i) =  st(0) - st(i); pop st(0)
#define FSUBP_i0( i)                           FPUINS(0xDEE8     | (i))                   // st(i) -= st(0); pop st(0)
#define FSUB                                   FSUBP_i0(1)                                // st(1) -= st(0); pop st(0)

#define FDIV_0i(  i)                           FPUINS(0xD8F0     | (i))                   // st(0) /= st(i)
#define FDIVR_0i( i)                           FPUINS(0xD8F8     | (i))                   // st(0) =  st(i) / st(0)
#define FDIVR_i0( i)                           FPUINS(0xDCF0     | (i))                   // st(i) =  st(0) / st(i)
#define FDIV_i0(  i)                           FPUINS(0xDCF8     | (i))                   // st(i) /= st(0)
#define FDIVRP_i0(i)                           FPUINS(0xDEF0     | (i))                   // st(i) =  st(0) / st(i); pop st(0)
#define FDIVP_i0( i)                           FPUINS(0xDEF8     | (i))                   // st(i) /= st(0); pop st(0)
#define FDIV                                   FDIVP_i0(1)                                // st(1) /= st(0); pop st(0)

#define FCOM(     i)                           FPUINS(0xD8D0     | (i))                   // Compare st(0) to st(i)
#define FCOMP(    i)                           FPUINS(0xD8D8     | (i))                   // Compare st(0) to st(i), pop st(0)
#define FCOMI(    i)                           FPUINS(0xDBF0     | (i))                   // Compare st(0) to st(i) and set CPU-flags
#define FUCOM(    i)                           FPUINS(0xDDE0     | (i))                   // Unordered compare st(0) to st(i)
#define FUCOMI(   i)                           FPUINS(0xDBE8     | (i))                   // Unordered compare st(0) to st(i) and set CPU-flags
#define FUCOMP(   i)                           FPUINS(0xDDE8     | (i))                   // Unordered compare st(0) to st(i); pop st(0)
#define FCOMIP(   i)                           FPUINS(0xDFF0     | (i))                   // Compare st(0) to st(i) and set CPU-flags; pop st(0)
#define FUCOMIP(  i)                           FPUINS(0xDFE8     | (i))                   // Unordered compare st(0) to st(i) and set CPU-flags; pop st(0)
#define FCOMPP                                 FPUINS(0xDED9)                             // Compare st(0) to st(1); pop both
#define FUCOMPP                                FPUINS(0xDAE9)                             // Unordered compare st(0) to st(1); pop both
#define FFREE(    i)                           FPUINS(0xDDC0     | (i))                   // Free a data register
#define FST(      i)                           FPUINS(0xDDD0     | (i))                   // Store st(0) into st(i)
#define FSTP(     i)                           FPUINS(0xDDD8     | (i))                   // Store st(0) into st(i); pop st(0)

#define FLD(      i)                           FPUINS(0xD9C0     | (i))                   // Push st(i) into st(0)
#define FXCH(     i)                           FPUINS(0xD9C8     | (i))                   // Swap st(0) and st(i)
#define FNOP                                   FPUINS(0xD9D0)                             // No operation
#define FCHS                                   FPUINS(0xD9E0)                             // st(0) = -st(0)
#define FABS                                   FPUINS(0xD9E1)                             // st(0) = abs(st(0))
#define FTST                                   FPUINS(0xD9E4)                             // Compare st(0) to 0.0
#define FXAM                                   FPUINS(0xD9E5)                             // Examine the content of st(0)
#define FLD1                                   FPUINS(0xD9E8)                             // push 1.0
#define FLDL2T                                 FPUINS(0xD9E9)                             // push log2(10)
#define FLDL2E                                 FPUINS(0xD9EA)                             // push log2(e)
#define FLDPI                                  FPUINS(0xD9EB)                             // push pi
#define FLDLG2                                 FPUINS(0xD9EC)                             // push log10(2)
#define FLDLN2                                 FPUINS(0xD9ED)                             // push ln(2)
#define FLDZ                                   FPUINS(0xD9EE)                             // push 0.0
#define F2XM1                                  FPUINS(0xD9F0)                             // st(0) = 2^st(0)-1, assume -1 <= st(0) <= 1
#define FYL2X                                  FPUINS(0xD9F1)                             // st(1) = log2(st(0))*st(1); pop st(0)
#define FPTAN                                  FPUINS(0xD9F2)                             // st(0) = tan(st(0)); push 1.0
#define FPATAN                                 FPUINS(0xD9F3)                             // st(1) = atan(st(1)/st(0)); pop st(0)
#define FXTRACT                                FPUINS(0xD9F4)                             // st(0) = unbiased exponent in floating point format of st(0). then push signinificant wiht exponent 0
#define FPREM1                                 FPUINS(0xD9F5)                             // As FPREM. Magnitude of the remainder <= ST(1) / 2
#define FDECSTP                                FPUINS(0xD9F6)                             // Decrement stack pointer. st0->st1, st7->st0, ..., st1->st2
#define FINCSTP                                FPUINS(0xD9F7)                             // Increment stack pointer. st0->st7, st1->st0, ..., st7->st6
#define FPREM                                  FPUINS(0xD9F8)                             // Partial remainder. st(0) %= st(1). Exponent of st(0) reduced with at most 63
#define FYL2XP1                                FPUINS(0xD9F9)                             // st(1) = log2(st(0)+1)*st(1); pop st(0)
#define FSQRT                                  FPUINS(0xD9FA)                             // st(0) = sqrt(st(0))
#define FSINCOS                                FPUINS(0xD9FB)                             // Sine and cosine of the angle value in ST(0), st(0)=sin; push(cos)
#define FRNDINT                                FPUINS(0xD9FC)                             // st(0) = nearest integral value according to the rounding mode
#define FSCALE                                 FPUINS(0xD9FD)                             // st(0) *= 2^int(st(1))
#define FSIN                                   FPUINS(0xD9FE)                             // st(0) = sin(ST(0))
#define FCOS                                   FPUINS(0xD9FF)                             // st(0) = cos(ST(0))

// Move st(i) to st(0) if specified CPU condition is true
#define FCMOVB( i)                             FPUINS(0xDAC0 | (i))                       // Move if below (CF=1)
#define FCMOVEQ(i)                             FPUINS(0xDAC8 | (i))                       // Move if equal (ZF=1)
#define FCMOVBE(i)                             FPUINS(0xDAD0 | (i))                       // Move if below or equal (CF=1 or ZF=1)
#define FCMOVU( i)                             FPUINS(0xDAD8 | (i))                       // Move if unordered (PF=1)
#define FCMOVAE(i)                             FPUINS(0xDBC0 | (i))                       // Move if above or equal (CF=0)
#define FCMOVNE(i)                             FPUINS(0xDBC8 | (i))                       // Move if not equal (ZF=0)
#define FCMOVA( i)                             FPUINS(0xDBD0 | (i))                       // Move if above (CF=0 and ZF=0)
#define FCMOVNU(i)                             FPUINS(0xDBD8 | (i))                       // Move if not unordered (PF=0)

#define FNSTSW_AX                              FPUINS(0xDFE0)                             // Store status word into CPU register AX
// These opcodes should all be used with MEM_ADDR_* to get the various addressing-modes

#define FLDCW_WORD                             FPUINSA(0xD928)                            // load control word
#define FNSTCW_WORD                            FPUINSA(0xD938)                            // store control word
#define FNSTSW_WORD                            FPUINSA(0xDD38)                            // store status word

// Real4 (float)
#define FLD_DWORD                              FPUINSA(0xD900)
#define FST_DWORD                              FPUINSA(0xD910)
#define FSTP_DWORD                             FPUINSA(0xD918)

// Real8 (double)
#define FLD_QWORD                              FPUINSA(0xDD00)
#define FST_QWORD                              FPUINSA(0xDD10)
#define FSTP_QWORD                             FPUINSA(0xDD18)

// Real10 (Double80)
#define FLD_TBYTE                              FPUINSA(0xDB28)
#define FSTP_TBYTE                             FPUINSA(0xDB38)

// 16-bit integer (signed short)
#define FILD_WORD                              FPUINSA(0xDF00)
#define FIST_WORD                              FPUINSA(0xDF10)
#define FISTP_WORD                             FPUINSA(0xDF18)
#define FISTTP_WORD                            FPUINSA(0xDF40)

// 32-bit integer (signed int)
#define FILD_DWORD                             FPUINSA(0xDB00)
#define FIST_DWORD                             FPUINSA(0xDB10)
#define FISTP_DWORD                            FPUINSA(0xDB18)
#define FISTTP_DWORD                           FPUINSA(0xDB40)

// 64-bit integer (signed __int64)
#define FILD_QWORD                             FPUINSA(0xDF28)
#define FISTP_QWORD                            FPUINSA(0xDF38)
#define FISTTP_QWORD                           FPUINSA(0xDD40)

// Real4 (float)
#define FADD_DWORD                             FPUINSA(0xD800)
#define FMUL_DWORD                             FPUINSA(0xD808)
#define FCOM_DWORD                             FPUINSA(0xD810)
#define FCOMP_DWORD                            FPUINSA(0xD818)
#define FSUB_DWORD                             FPUINSA(0xD820)
#define FSUBR_DWORD                            FPUINSA(0xD828)
#define FDIV_DWORD                             FPUINSA(0xD830)
#define FDIVR_DWORD                            FPUINSA(0xD838)

// Real8 (double)
#define FADD_QWORD                             FPUINSA(0xDC00)
#define FMUL_QWORD                             FPUINSA(0xDC08)
#define FCOM_QWORD                             FPUINSA(0xDC10)
#define FCOMP_QWORD                            FPUINSA(0xDC18)
#define FSUB_QWORD                             FPUINSA(0xDC20)
#define FSUBR_QWORD                            FPUINSA(0xDC28)
#define FDIV_QWORD                             FPUINSA(0xDC30)
#define FDIVR_QWORD                            FPUINSA(0xDC38)

// 16-bit integer (short)
#define FIADD_WORD                             FPUINSA(0xDE00)
#define FIMUL_WORD                             FPUINSA(0xDE08)
#define FICOM_WORD                             FPUINSA(0xDE10)
#define FICOMP_WORD                            FPUINSA(0xDE18)
#define FISUB_WORD                             FPUINSA(0xDE20)
#define FISUBR_WORD                            FPUINSA(0xDE28)
#define FIDIV_WORD                             FPUINSA(0xDE30)
#define FIDIVR_WORD                            FPUINSA(0xDE38)

// 32-bit integer (int)
#define FIADD_DWORD                            FPUINSA(0xDA00)
#define FIMUL_DWORD                            FPUINSA(0xDA08)
#define FICOM_DWORD                            FPUINSA(0xDA10)
#define FICOMP_DWORD                           FPUINSA(0xDA18)
#define FISUB_DWORD                            FPUINSA(0xDA20)
#define FISUBR_DWORD                           FPUINSA(0xDA28)
#define FIDIV_DWORD                            FPUINSA(0xDA30)
#define FIDIVR_DWORD                           FPUINSA(0xDA38)

#define FBLD                                   FPUINSA(0xDF20)                            // LoaD BCD data from memory
#define FBSTP                                  FPUINSA(0xDF30)                            // STore BCD data to memory

#endif