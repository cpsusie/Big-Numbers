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
inline bool isInt(size_t v) {
  return (intptr_t)v == (int)v;
}

typedef RegSize OperandSize;

typedef enum {
  REGISTER
 ,MEMORYOPERAND
 ,IMMEDIATEVALUE
} OperandType;

String toString(OperandType type);
// Convert int32-value to disassembler format
String formatHexValue(int    v, bool showSign);
// Convert int64-value to disassembler format
String formatHexValue(INT64  v, bool showSign);
String formatHexValue(UINT   v);
String formatHexValue(UINT64 v);
String getImmSizeErrorString(const String &dst, INT64 immv);

typedef vBitSet8<OperandType> OperandTypeSet;

class MemoryRef {
private:
  static const char s_shift[9];
  const IndexRegister *m_base,*m_inx;
  const BYTE           m_shift;
  const size_t         m_offset;
#ifdef IS32BIT
#define SETNEEDREXBYTE(base,inx)
#else // IS64BIT

  const bool m_needREXByte;
  static inline bool findRexByteNeeded(const IndexRegister *base, const IndexRegister *inx) {
    return (base && base->indexNeedREXByte()) || (inx && inx->indexNeedREXByte());
  }
#define SETNEEDREXBYTE(base,inx) ,m_needREXByte(findRexByteNeeded(base,inx))
#endif // IS64BIT

  static void throwInvalidIndexScale(const TCHAR *method, BYTE a);

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
  inline MemoryRef(size_t addr)
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
  static inline char findShift(BYTE a) {
    if((a >= ARRAYSIZE(s_shift)) || (s_shift[a] < 0)) throwInvalidIndexScale(__TFUNCTION__,a);
    return s_shift[a];
  }
  static void throwInvalidIndexRegister(const TCHAR *method, const Register &reg);
  void throwInvalidIndex(const TCHAR *method, char op, const String &str) const;

  // set m_needREXByte in x64
  void sortBaseInx();
  inline const IndexRegister *getBase()   const { return m_base;         }
  inline const IndexRegister *getInx()    const { return m_inx;          }
  inline BYTE                 getShift()  const { return m_shift;        }
  inline int                  getOffset() const { assert(isInt(m_offset)); return (int)m_offset;  }
  inline size_t               getAddr()   const { return m_offset;       }
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
  static int offsetCmp(const MemoryRef &mr1, const MemoryRef &mr2);
  static int addrCmp(  const MemoryRef &mr1, const MemoryRef &mr2);
  static int memRefCmp(const MemoryRef &mr1, const MemoryRef &mr2);
};

inline MemoryRef operator+(const IndexRegister &base, int offset) {
  return MemoryRef(&base,NULL,0,offset);
}

inline MemoryRef operator-(const IndexRegister &base, int offset) {
  return MemoryRef(&base,NULL,0,-offset);
}

inline MemoryRef operator+(const MemoryRef &mr, int offset) {
  if(mr.hasOffset()) mr.throwInvalidIndex(__TFUNCTION__,'+',formatHexValue(offset,false));
  return offset ? MemoryRef(mr.getBase(),mr.getInx(),mr.getShift(),offset) : mr;
}

inline MemoryRef operator-(const MemoryRef &mr, int offset) {
  if(mr.hasOffset()) mr.throwInvalidIndex(__TFUNCTION__,'-',formatHexValue(offset,false));
  return offset ? MemoryRef(mr.getBase(),mr.getInx(),mr.getShift(),-offset) : mr;
}

inline MemoryRef operator+(const IndexRegister &base, const MemoryRef &mr) {
  if(mr.hasBase() || (!mr.hasInx() || mr.hasOffset())) {
    mr.throwInvalidIndex(__TFUNCTION__,'+',base.getName());
  }
  return MemoryRef(&base,mr.getInx(),mr.getShift());
}

inline MemoryRef operator+(const IndexRegister &base, const IndexRegister &inx) {
  return MemoryRef(&base,&inx,0);
}

inline MemoryRef operator*(BYTE a, const IndexRegister &inx) {
  if(!inx.isValidIndexRegister()) MemoryRef::throwInvalidIndexRegister(__TFUNCTION__,inx);
  return MemoryRef(NULL,&inx,MemoryRef::findShift(a));
}

inline MemoryRef operator*(const IndexRegister &inx, BYTE a) {
  if(!inx.isValidIndexRegister()) MemoryRef::throwInvalidIndexRegister(__TFUNCTION__,inx);
  return MemoryRef(NULL,&inx,MemoryRef::findShift(a));
}

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
  void throwSizeError(  const TCHAR *method, OperandSize expectedSize) const;
  void throwTypeError(  const TCHAR *method, OperandType expectedType) const;

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
  inline OperandSize getLimitedSize(OperandSize limit) const {
    return Register::getLimitedSize(getSize(),limit);
  }
  inline const Register &getRegister() const {
//    assert(isRegister());
    return *m_reg;
  }
  inline bool isRegister() const {
    return getType() == REGISTER;
  }
  inline bool isMemoryRef() const {
    return getType() == MEMORYOPERAND;
  }
  inline bool isImmediateValue() const {
    return getType() == IMMEDIATEVALUE;
  }
  inline bool isRegister(RegType type) const {
    return isRegister() && (getRegister().getType() == type);
  }
  inline bool isGPR0() const {
    return isRegister() && getRegister().isGPR0();
  }
  inline bool isST0() const {
    return isRegister() && getRegister().isST0();
  }
  inline bool isDisplaceOnly() const {
    return isMemoryRef() && getMemoryReference().isDisplaceOnly();
  }
  inline bool isImmByte() const {
    return isImmediateValue() && (getSize() == REGSIZE_BYTE);
  }
  char   getImmInt8()   const;
  BYTE   getImmUint8()  const;
  short  getImmInt16()  const;
  USHORT getImmUint16() const;
  int    getImmInt32()  const;
  UINT   getImmUint32() const;
  INT64  getImmInt64()  const;
  UINT64 getImmUint64() const;

  inline bool isShiftAmountOperand() const {
    return isImmByte() || (isRegister() && (getRegister() == CL));
  }
  virtual const MemoryRef       &getMemoryReference() const {
    throwUnsupportedOperationException(__TFUNCTION__);
    static const MemoryRef dummy(0);
    return dummy;
  }

#ifdef IS64BIT
  virtual bool  needREXByte() const {
    return (getType() == REGISTER) ? m_reg->needREXByte() : false;
  }
#endif // IS64BIT
  virtual String toString() const;

  static const OperandTypeSet R,M,IMM,RM,S,ALL,EMPTY;

  static int insOpCmp(const InstructionOperand &op1, const InstructionOperand &op2);
};

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
  inline MemoryOperand(OperandSize size, size_t addr, const SegmentRegister *segReg=NULL)
    : InstructionOperand(MEMORYOPERAND, size)
    , m_segReg(segReg)
    , m_mr(addr)
  {
    SETDEBUGSTR();
  }
  inline const SegmentRegister *getSegmentRegister() const {
    return m_segReg;
  }
  inline bool                   hasSegmentRegister() const {
    return m_segReg != NULL;
  }
  const MemoryRef              &getMemoryReference() const {
    return m_mr;
  }
  inline bool containsSize(OperandSize immSize) const {
    return Register::sizeContainsSrcSize(getSize(), immSize);
  }

#ifdef IS64BIT
  bool needREXByte() const {
    return m_mr.needREXByte();
  }
#endif // IS64BIT
  String toString() const;

  static int memOpCmp(const MemoryOperand &mem1, const MemoryOperand &mem2);
};

template<OperandSize size> class MemoryPtr : public MemoryOperand {
protected:
  MemoryPtr(const MemoryOperand &mem)
    : MemoryOperand(size, mem.getMemoryReference(), mem.getSegmentRegister())
  {
  }
public:
  MemoryPtr(const MemoryRef &mr) : MemoryOperand(size, mr) {
  }
  MemoryPtr(const SegmentRegister &segReg, const MemoryRef &mr) : MemoryOperand(size, mr, &segReg) {
  }
  MemoryPtr(const SegmentRegister &segReg, size_t addr) : MemoryOperand(size, addr, &segReg) {
  }
#ifdef IS64BIT
  MemoryPtr(size_t addr) : MemoryOperand(size, addr) {
  }
#endif // IS64BIT
};

typedef MemoryPtr<REGSIZE_BYTE >   BYTEPtr;
typedef MemoryPtr<REGSIZE_WORD >   WORDPtr;
typedef MemoryPtr<REGSIZE_DWORD>   DWORDPtr;
typedef MemoryPtr<REGSIZE_QWORD>   QWORDPtr;
typedef MemoryPtr<REGSIZE_MMWORD>  MMWORDPtr;
typedef MemoryPtr<REGSIZE_XMMWORD> XMMWORDPtr;
typedef MemoryPtr<REGSIZE_TBYTE>   TBYTEPtr;

class VOIDPtr : public MemoryPtr<REGSIZE_VOID> {
public:
  VOIDPtr(const MemoryOperand &op) : MemoryPtr(op) {
  }
};

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
#define REGTYPE_GPR0_ALLOWED   0x00000001
#define REGTYPE_GPR_ALLOWED    0x00000002
#define REGTYPE_SEG_ALLOWED    0x00000004
#define REGTYPE_FPU_ALLOWED    0x00000008
#define REGTYPE_XMM_ALLOWED    0x00000010
#define BYTEGPR_ALLOWED        0x00000020
#define WORDGPR_ALLOWED        0x00000040
#define DWORDGPR_ALLOWED       0x00000080
#ifdef IS32BIT
#define QWORDGPR_ALLOWED       0
#else // IS64BIT
#define QWORDGPR_ALLOWED       0x00000100
#endif // IS64BIT
#define BYTEPTR_ALLOWED        0x00000200
#define WORDPTR_ALLOWED        0x00000400
#define DWORDPTR_ALLOWED       0x00000800
#define QWORDPTR_ALLOWED       0x00001000
#define TBYTEPTR_ALLOWED       0x00002000
#define MMWORDPTR_ALLOWED      0x00004000
#define XMMWORDPTR_ALLOWED     0x00008000
#define VOIDPTR_ALLOWED        0x00010000
#define IMM8_ALLOWED           0x00020000
#define IMM16_ALLOWED          0x00040000
#define IMM32_ALLOWED          0x00080000
#ifdef IS32BIT
#define IMM64_ALLOWED          0
#else  // IS64BIT
#define IMM64_ALLOWED          0x00100000
#endif // IS64BIT

#define IMMMADDR_ALLOWED       0x00200000
#define NO_MODEBYTE            0x00400000
#define HAS_BYTE_SIZEBIT       0x00800000
#define HAS_IMM_XBIT           0x01000000
#define HAS_WORDPREFIX         0x02000000
#ifdef IS32BIT
#define HAS_REXQSIZEBIT        0x00000000
#else  // IS64BIT
#define HAS_REXQSIZEBIT        0x04000000
#endif // IS64BIT
#define HAS_DIRECTIONBIT1      0x08000000
#define HAS_DIRECTIONBIT0      0x10000000
#define FIRSTOP_REGONLY        0x20000000
#define FIRSTOP_GPR0ONLY       0x40000000
#define LASTOP_IMMONLY         0x80000000

#define IMMEDIATEVALUE_ALLOWED      (IMM8_ALLOWED         | IMM16_ALLOWED | IMM32_ALLOWED)


#ifdef IS32BIT
#define NONBYTE_GPRPTR_ALLOWED      (WORDPTR_ALLOWED      | DWORDPTR_ALLOWED)
#define INDEXGPR_ALLOWED             DWORDGPR_ALLOWED
#define INDEXPTR_ALLOWED             DWORDPTR_ALLOWED
#else  // IS64BIT
#define NONBYTE_GPRPTR_ALLOWED      (WORDPTR_ALLOWED      | DWORDPTR_ALLOWED | QWORDPTR_ALLOWED)
#define INDEXGPR_ALLOWED             QWORDGPR_ALLOWED
#define INDEXPTR_ALLOWED             QWORDPTR_ALLOWED
#endif // IS64BIT


#define NONBYTE_GPRSIZE_ALLOWED     (WORDGPR_ALLOWED      | DWORDGPR_ALLOWED | QWORDGPR_ALLOWED)
#define WORDINDEX_GPRSIZE_ALLOWED   (WORDGPR_ALLOWED      | INDEXGPR_ALLOWED         )
#define ALL_GPRSIZE_ALLOWED         (BYTEGPR_ALLOWED      | NONBYTE_GPRSIZE_ALLOWED  )

#define NONBYTE_GPR_ALLOWED         (REGTYPE_GPR_ALLOWED  | NONBYTE_GPRSIZE_ALLOWED  )
#define WORDINDEX_GPR_ALLOWED       (REGTYPE_GPR_ALLOWED  | WORDINDEX_GPRSIZE_ALLOWED)
#define ALL_GPR_ALLOWED             (REGTYPE_GPR_ALLOWED  | ALL_GPRSIZE_ALLOWED      )
#define ALL_GPR0_ALLOWED            (REGTYPE_GPR0_ALLOWED | ALL_GPRSIZE_ALLOWED      )

#define ALL_GPRPTR_ALLOWED          (BYTEPTR_ALLOWED      | NONBYTE_GPRPTR_ALLOWED   )
#define WORDINDEXPTR_ALLOWED        (WORDPTR_ALLOWED      | INDEXPTR_ALLOWED         )
#define HAS_NONBYTE_SIZEBITS        (HAS_WORDPREFIX       | HAS_REXQSIZEBIT          )
#define HAS_ALL_SIZEBITS            (HAS_BYTE_SIZEBIT     | HAS_NONBYTE_SIZEBITS)
#define ALLPTR_ALLOWED              (BYTEPTR_ALLOWED      | WORDPTR_ALLOWED     \
                                   | DWORDPTR_ALLOWED     | QWORDPTR_ALLOWED    \
                                   | TBYTEPTR_ALLOWED                           \
                                   | MMWORDPTR_ALLOWED    | XMMWORDPTR_ALLOWED  \
                                   | VOIDPTR_ALLOWED)

#define ALLIMM_MASK                 (IMMEDIATEVALUE_ALLOWED                     \
                                   | IMM64_ALLOWED                              \
                                   | IMMMADDR_ALLOWED                           \
                                   | HAS_IMM_XBIT                               \
                                   | LASTOP_IMMONLY)

#define ALLNONIMM_MASK             (~ALLIMM_MASK)

class OpcodeBase {
private:
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
  OpcodeBase(const String &mnemonic,   const InstructionBase &src, BYTE extension=0, UINT flags=0);
  void throwInvalidOperandCombination( const InstructionOperand &op1, const InstructionOperand &op2) const;
  void throwInvalidOperandCombination( const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
  void throwInvalidOperandType(        const InstructionOperand &op, BYTE index) const;
  void throwUnknownOperandType(        const InstructionOperand &op, BYTE index) const;
  static void throwUnknownRegisterType(const TCHAR *method, RegType      type);
  // Should only be used for GPR-registers
  bool isGPRegisterSizeAllowed(   RegSize     size) const;
  bool isMemoryOperandSizeAllowed(OperandSize size) const;
  bool isImmediateSizeAllowed(    OperandSize size) const;

  inline bool isImmAddrAllowed() const {
    return (getFlags() & IMMMADDR_ALLOWED) != 0;
  }
  bool isRegisterAllowed(const Register &reg) const;
  inline bool isMemoryOperandAllowed(const MemoryOperand &mem) const {
    return isMemoryOperandSizeAllowed(mem.getSize())
        && (isImmAddrAllowed() || isDword(mem.getMemoryReference().getAddr()));
  }
  bool validateOpCount(                  int                       count                               , bool throwOnError) const;
  bool validateRegisterAllowed(          const Register           &reg                                 , bool throwOnError) const;
  bool validateMemoryOperandAllowed(     const MemoryOperand      &mem                                 , bool throwOnError) const;
  bool validateImmediateOperandAllowed(  const InstructionOperand &imm                                 , bool throwOnError) const;
  bool validateImmediateValue(           const Register           &reg , const InstructionOperand &imm , bool throwOnError) const;
  bool validateImmediateValue(           const MemoryOperand      &mem , const InstructionOperand &imm , bool throwOnError) const;
  bool validateImmediateValue(           const InstructionOperand &dst , const InstructionOperand &imm , bool throwOnError) const;
  bool validateSameSize(                 const Register           &reg1, const Register           &reg2, bool throwOnError) const;
  virtual bool validateSameSize(         const Register           &reg , const InstructionOperand &op  , bool throwOnError) const;
  bool validateSameSize(                 const InstructionOperand &op1 , const InstructionOperand &op2 , bool throwOnError) const;
  bool validateIsRegisterOperand(        const InstructionOperand &op  , BYTE  index                   , bool throwOnError) const;
  bool validateIsMemoryOperand(          const InstructionOperand &op  , BYTE  index                   , bool throwOnError) const;
  bool validateIsRegisterOrMemoryOperand(const InstructionOperand &op  , BYTE  index                   , bool throwOnError) const;
  bool validateIsImmediateOperand(       const InstructionOperand &op  , BYTE  index                   , bool throwOnError) const;
  bool validateIsShiftAmountOperand(     const InstructionOperand &op  , BYTE  index                   , bool throwOnError) const;
  void validateOperandIndex(                                             BYTE  index) const {
    if((index < 1) || (index > getMaxOpCount())) {
      throwInvalidArgumentException(__TFUNCTION__,_T("index=%u. Valid range=[1..%u]"),index,getMaxOpCount());
    }
  }
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
  virtual BYTE getMaxOpCount() const {
    return getOpCount();
  }
  // Raw opcode bytes
  inline const BYTE *getBytes() const {
    return (BYTE*)&m_bytes;
  }
  // Various attributes
  inline UINT getFlags() const {
    return m_flags;
  }
  // Returns by default RM for index 1,2; S for 3, or else EMPTY
  virtual const OperandTypeSet &getValidOperandTypes(BYTE index) const;
  // Operands are indexed from 1
  virtual bool isValidOperandType(       const InstructionOperand &op, BYTE index) const;
  virtual bool isValidOperand(           const InstructionOperand &op                                                               , bool throwOnError=false) const;
  virtual bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2                               , bool throwOnError=false) const;
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
  inline Opcode1Arg(const String &mnemonic, UINT op, BYTE extension, UINT flags=ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | HAS_ALL_SIZEBITS)
    : OpcodeBase(mnemonic, op, extension, 1, flags)
  {
  }
  InstructionBase operator()(const InstructionOperand &op) const;
};

// encoding MR/RM. op1=reg/mem, op2/reg/mem. at least 1 operand must be reg
class Opcode2Arg : public OpcodeBase {
public :
  Opcode2Arg(const String &mnemonic, UINT op, UINT flags=ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | IMMEDIATEVALUE_ALLOWED | HAS_ALL_SIZEBITS | HAS_DIRECTIONBIT1)
    : OpcodeBase(mnemonic, op, 0, 2, flags)
  {
  }
  Opcode2Arg(const String &mnemonic, UINT op, BYTE extension, UINT flags)
    : OpcodeBase(mnemonic, op, extension, 2, flags)
  {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

// encoding FD/TD. op1=al/ax,eax,rax, op2=b/w/dw/qw ptr[memory offset], or vice versa
class Opcode2ArgTF : public Opcode2Arg {
public:
  Opcode2ArgTF(const String &mnemonic, BYTE op, UINT flags=0)
    : Opcode2Arg(mnemonic,op,flags | ALL_GPR0_ALLOWED | IMMMADDR_ALLOWED | ALL_GPRPTR_ALLOWED | HAS_ALL_SIZEBITS | HAS_DIRECTIONBIT1)
  {
  }
};
// encoding I. op1=AL/AX/EAX/RAX. op2=imm
class Opcode2ArgI : public OpcodeBase {
public:
  Opcode2ArgI(const String &mnemonic, BYTE op)
    : OpcodeBase(mnemonic, op, 0, 2, ALL_GPR0_ALLOWED | HAS_ALL_SIZEBITS | IMMEDIATEVALUE_ALLOWED | FIRSTOP_REGONLY | LASTOP_IMMONLY)
  {
  }
  InstructionBase operator()(const InstructionOperand &dst, const InstructionOperand &imm) const;
};

// encoding OI. op1=r8/r16/r32/r64. op2=imm8/imm16/imm32
class Opcode2ArgOI : public OpcodeBase {
public:
  Opcode2ArgOI(const String &mnemonic, BYTE op, UINT flags=0)
    : OpcodeBase(mnemonic, op, 0, 2, flags | (ALL_GPR_ALLOWED|HAS_NONBYTE_SIZEBITS|IMMEDIATEVALUE_ALLOWED|FIRSTOP_REGONLY|LASTOP_IMMONLY))
  {
  }
  InstructionBase operator()(const InstructionOperand &dst, const InstructionOperand &imm) const;
};

// encoding MI. op1=mem/reg, op2=imm
class Opcode2ArgMI : public OpcodeBase {
public:
  Opcode2ArgMI(const String &mnemonic, UINT op, BYTE extension, UINT flags=IMMEDIATEVALUE_ALLOWED)
    : OpcodeBase(mnemonic, op, extension, 2, flags | (ALL_GPR_ALLOWED|ALL_GPRPTR_ALLOWED|HAS_ALL_SIZEBITS|LASTOP_IMMONLY))
  {
  }
  InstructionBase operator()(const InstructionOperand &dst, const InstructionOperand &imm) const;
};

// encoding MI. op1=mem/reg, op2=imm8
class Opcode2ArgMI8 : public Opcode2ArgMI {
public:
  Opcode2ArgMI8(const String &mnemonic, UINT op, BYTE extension)
    : Opcode2ArgMI(mnemonic, op, extension, IMM8_ALLOWED)
  {
  }
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

// encoding M. op1=AL/AX/EAX/RAX. op2=RM
class Opcode2ArgM : public OpcodeBase {
public:
  Opcode2ArgM(const String &mnemonic, UINT op, BYTE extension)
    : OpcodeBase(mnemonic, op, extension, 2, ALL_GPR_ALLOWED|ALL_GPRPTR_ALLOWED|HAS_ALL_SIZEBITS|FIRSTOP_GPR0ONLY)
  {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class Opcode3Arg : public OpcodeBase {
public :
  Opcode3Arg(const String &mnemonic, UINT op, UINT flags=NONBYTE_GPR_ALLOWED | NONBYTE_GPRPTR_ALLOWED | FIRSTOP_REGONLY | LASTOP_IMMONLY | IMMEDIATEVALUE_ALLOWED | HAS_NONBYTE_SIZEBITS | HAS_IMM_XBIT)
    : OpcodeBase(mnemonic, op, 0, 3, flags) {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
};

class OpcodeIMul : public Opcode1Arg {
private:
  const Opcode2Arg m_imul2ArgCode;
  const Opcode3Arg m_imul3ArgCode;
public :
  OpcodeIMul(const String &mnemonic)
    : Opcode1Arg(    mnemonic, 0xF6,5)
    , m_imul2ArgCode(mnemonic, 0x0FAF, NONBYTE_GPR_ALLOWED | NONBYTE_GPRPTR_ALLOWED | FIRSTOP_REGONLY | HAS_NONBYTE_SIZEBITS)
    , m_imul3ArgCode(mnemonic, 0x69)
  {
  }
  BYTE getMaxOpCount() const {
    return 3;
  }
  bool isValidOperandType(       const InstructionOperand &op, BYTE index) const;
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2,                                bool throwOnError=false) const;
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError=false) const;
  InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2) const;
  InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
};

class OpcodeXchg : public Opcode2Arg {
private:
  const OpcodeBase m_eaxRegCode;
public:
  OpcodeXchg(const String &mnemonic)
    : Opcode2Arg(  mnemonic, 0x86, ALL_GPR_ALLOWED | ALL_GPRPTR_ALLOWED | HAS_ALL_SIZEBITS)
    , m_eaxRegCode(mnemonic, 0x90,0,1,NO_MODEBYTE)
  {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeStd2Arg : public Opcode2Arg {
private:
  const Opcode2ArgI  m_codeI;
  const Opcode2ArgMI m_codeMI;
public :
  OpcodeStd2Arg(const String &mnemonic, UINT opStd, BYTE opI, UINT opMI, BYTE extMI
               ,UINT flags=0)
    : Opcode2Arg(mnemonic, opStd     , (flags&ALLNONIMM_MASK)|(ALL_GPR_ALLOWED|ALL_GPRPTR_ALLOWED|HAS_ALL_SIZEBITS))
    , m_codeI(   mnemonic, opI)
    , m_codeMI(  mnemonic, opMI,extMI, (flags&ALLIMM_MASK)|IMMEDIATEVALUE_ALLOWED)
  {
  }
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeMov : public Opcode2Arg {
private:
  const Opcode2ArgTF m_GPR0tf;
  const Opcode2Arg   m_movSegCode;
  const Opcode2ArgOI m_regImmCode;
  const Opcode2ArgMI m_memImmCode;
public :
  OpcodeMov(const String &mnemonic)
    : Opcode2Arg(    mnemonic, 0x88)
    , m_GPR0tf(      mnemonic, 0xA0)
    , m_movSegCode(  mnemonic, 0x8C, NONBYTE_GPR_ALLOWED | REGTYPE_SEG_ALLOWED    | WORDPTR_ALLOWED      | HAS_DIRECTIONBIT1)
    , m_regImmCode(  mnemonic, 0xB0, IMM64_ALLOWED)
    , m_memImmCode(  mnemonic, 0xC6, 0)
  {
  }
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeLea : public Opcode2Arg {
public :
  OpcodeLea(const String &mnemonic, BYTE op)
    : Opcode2Arg(mnemonic, op, NONBYTE_GPR_ALLOWED | ALLPTR_ALLOWED | HAS_NONBYTE_SIZEBITS)
  {
  }
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodePushPop : public Opcode1Arg {
private:
  Opcode1Arg m_memCode;
  Opcode1Arg m_immCode;
public:
  inline OpcodePushPop(const String &mnemonic, BYTE opreg, BYTE opmem, BYTE extmem, BYTE opImm)
    : Opcode1Arg(mnemonic, opreg, 0        , REGTYPE_GPR_ALLOWED | INDEXGPR_ALLOWED | WORDGPR_ALLOWED | HAS_WORDPREFIX | NO_MODEBYTE)
    , m_memCode( mnemonic, opmem, extmem,                          INDEXPTR_ALLOWED | WORDPTR_ALLOWED | HAS_WORDPREFIX)
    , m_immCode( mnemonic, opImm, 0        , opImm?(IMMEDIATEVALUE_ALLOWED|HAS_IMM_XBIT):0)
  {
  }
  bool isValidOperand(       const InstructionOperand &op, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op) const;
};

#ifdef IS32BIT
class OpcodeIncDec : public Opcode1Arg {
private:
  const OpcodeBase m_opReg32;
public :
  OpcodeIncDec(const String &mnemonic, BYTE op, BYTE extension)
    : Opcode1Arg(mnemonic,op,extension)
    , m_opReg32(mnemonic,0x40|(extension<<3),0,1,0)
  {
  }
  InstructionBase operator()(const InstructionOperand &op) const;
};
#else  // IS64BIT
typedef Opcode1Arg OpcodeIncDec;
#endif // IS64BIT

class OpcodeShiftRot : public Opcode2Arg {
private:
  Opcode2ArgMI8 m_immCode;
public:
  OpcodeShiftRot(const String &mnemonic, BYTE extension);
  bool isValidOperandType(const InstructionOperand &op, BYTE index) const {
    return (index == 2) ? op.isShiftAmountOperand() : __super::isValidOperandType(op,index);
  }
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeDoubleShift : public OpcodeBase {
private:
  Opcode2Arg m_clCode;
public:
  OpcodeDoubleShift(const String &mnemonic, UINT opCL, UINT opImm);
  bool isValidOperandType(const InstructionOperand &op, BYTE index) const {
    return (index == 3) ? op.isShiftAmountOperand() : __super::isValidOperandType(op,index);
  }
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) const;
};

class OpcodeBitScan : public Opcode2Arg {
public:
  OpcodeBitScan(const String &mnemonic, UINT op);
};

class OpcodeSetcc : public Opcode1Arg {
public:
  OpcodeSetcc(const String &mnemonic, UINT op) : Opcode1Arg(mnemonic, op, 0, REGTYPE_GPR_ALLOWED | BYTEGPR_ALLOWED | BYTEPTR_ALLOWED) {
  }
};

class OpcodeJmpImm : public Opcode1Arg {
public:
  OpcodeJmpImm(const String &mnemonic);
  InstructionBase operator()(const InstructionOperand &op) const;
};

class OpcodeJmp : public Opcode1Arg {
private:
  const OpcodeJmpImm m_jmpRelImm;
public:
  OpcodeJmp(const String &mnemonic);
  bool isValidOperand(const InstructionOperand &op, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op) const;
};

class OpcodeCallImm : public Opcode1Arg {
public:
  OpcodeCallImm(const String &mnemonic)
    : Opcode1Arg(mnemonic, 0xE8, 0, IMMEDIATEVALUE_ALLOWED) // near relative displacement (imm16/imm32)
  {
  }
  InstructionBase operator()(const InstructionOperand &op) const;
};

class OpcodeCall : public Opcode1Arg {
private:
  const OpcodeCallImm m_callNearRelImm;
public:
  OpcodeCall(const String &mnemonic)
    : Opcode1Arg(      mnemonic, 0xFF, 2, WORDINDEX_GPR_ALLOWED  | WORDINDEXPTR_ALLOWED | HAS_WORDPREFIX) // near absolute indirect addr given by reg/m16/m32/m64
    , m_callNearRelImm(mnemonic)
  {
  }
  bool isValidOperand(const InstructionOperand &op, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op) const;
};

class StringInstruction : public Opcode0Arg {
public:
  StringInstruction(const String &mnemonic, UINT op) : Opcode0Arg(mnemonic, op, ALL_GPRPTR_ALLOWED | HAS_ALL_SIZEBITS) {
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

class OpcodeFPUTransfer : public Opcode1Arg {
private:
  const Opcode1Arg m_dwordCode;
  const Opcode1Arg m_qwordCode;
  const Opcode1Arg m_tbyteCode;
public:
  OpcodeFPUTransfer(const String &mnemonic
    , UINT opreg, BYTE opdw, BYTE extdw, BYTE opqw, BYTE extqw, BYTE optb, BYTE exttb)
    : Opcode1Arg( mnemonic, opreg, 0    , REGTYPE_FPU_ALLOWED)
    , m_dwordCode(mnemonic, opdw , extdw, DWORDPTR_ALLOWED   )
    , m_qwordCode(mnemonic, opqw , extqw, QWORDPTR_ALLOWED   )
    , m_tbyteCode(mnemonic, optb , exttb, optb?TBYTEPTR_ALLOWED:0)
  {
  }
  bool isValidOperand(const InstructionOperand &op, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op) const;
};

class OpcodeFPU2Reg : public Opcode2Arg {
private:
  const OpcodeBase m_st0iCode;
public:
  OpcodeFPU2Reg(const String &mnemonic, UINT opi0, UINT op0i)
    : Opcode2Arg(mnemonic,opi0    ,REGTYPE_FPU_ALLOWED)
    , m_st0iCode(mnemonic,op0i,0,1,REGTYPE_FPU_ALLOWED)
  {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class OpcodeFPUArithm : public Opcode0Arg {
private:
  const Opcode1Arg    m_dwordCode;
  const Opcode1Arg    m_qwordCode;
  const OpcodeFPU2Reg m_2regCode;
public:
  OpcodeFPUArithm(const String &mnemonic
    , UINT opp, UINT opi0, UINT op0i, BYTE opdw, BYTE extdw, BYTE opqw, BYTE extqw)
    : Opcode0Arg( mnemonic, opp                                   )
    , m_2regCode( mnemonic, opi0,op0i                             )
    , m_dwordCode(mnemonic, opdw, extdw, DWORDPTR_ALLOWED         )
    , m_qwordCode(mnemonic, opqw, extqw, QWORDPTR_ALLOWED         )
  {
  }
  BYTE getMaxOpCount() const {
    return 2;
  }
  bool isValidOperand(           const InstructionOperand &op                                , bool throwOnError=false) const;
  bool isValidOperandCombination(const InstructionOperand &op1, const InstructionOperand &op2, bool throwOnError=false) const;
  InstructionBase operator()(    const InstructionOperand &op                                                         ) const;
  InstructionBase operator()(    const InstructionOperand &op1, const InstructionOperand &op2                         ) const;
};

class OpcodeFPUCompare : public Opcode1Arg {
private:
  const Opcode1Arg m_dwordCode;
  const Opcode1Arg m_qwordCode;
public:
  OpcodeFPUCompare(const String &mnemonic
    , UINT opreg, BYTE opdw, BYTE extdw, BYTE opqw, BYTE extqw)
    : Opcode1Arg( mnemonic, opreg, 0    , REGTYPE_FPU_ALLOWED)
    , m_dwordCode(mnemonic, opdw , extdw, DWORDPTR_ALLOWED   )
    , m_qwordCode(mnemonic, opqw , extqw, QWORDPTR_ALLOWED   )
  {
  }
  bool isValidOperand(const InstructionOperand &op, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op) const;
};

class OpcodeFPUIArithm : public Opcode1Arg {
private:
  const Opcode1Arg m_dwordCode;
  const Opcode1Arg m_qwordCode;
public:
  OpcodeFPUIArithm(const String &mnemonic
    , UINT opw, BYTE extw, BYTE opdw, BYTE extdw, BYTE opqw, BYTE extqw)
    : Opcode1Arg( mnemonic, opw , extw ,      WORDPTR_ALLOWED   )
    , m_dwordCode(mnemonic, opdw, extdw,      DWORDPTR_ALLOWED  )
    , m_qwordCode(mnemonic, opqw, extqw, opqw?QWORDPTR_ALLOWED:0)
  {
  }
  bool isValidOperand(const InstructionOperand &op, bool throwOnError=false) const;
  InstructionBase operator()(const InstructionOperand &op) const;
};

class Opcode2ArgPfxF2 : public Opcode2Arg {
public:
  Opcode2ArgPfxF2(const String &mnemonic, UINT op, UINT flags)
    : Opcode2Arg(mnemonic, op, flags | REGTYPE_XMM_ALLOWED)
  {
  }
  InstructionBase operator()(const InstructionOperand &op1, const InstructionOperand &op2) const;
};

class Opcode2ArgPfxF2SD : public Opcode2ArgPfxF2 {
protected:
  bool validateSameSize(const Register &reg , const InstructionOperand &op, bool throwOnError) const;
public:
  Opcode2ArgPfxF2SD(const String &mnemonic, UINT op, UINT flags = 0)
    : Opcode2ArgPfxF2(mnemonic, op, flags | MMWORDPTR_ALLOWED)
  {
  }
};

// --------------------------------- Opcode mnemonics ------------------------------

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

extern OpcodeStd2Arg     ADD;                              // Integer Addition
extern OpcodeStd2Arg     ADC;                              // Integer Addition with Carry
extern OpcodeStd2Arg     OR;                               // Logical Inclusive OR
extern OpcodeStd2Arg     AND;                              // Logical AND
extern OpcodeStd2Arg     SUB;                              // Integer Subtraction
extern OpcodeStd2Arg     SBB;                              // Integer Subtraction with Borrow
extern OpcodeStd2Arg     XOR;                              // Logical Exclusive OR
extern OpcodeStd2Arg     CMP;                              // Compare Two Operands

extern OpcodeXchg        XCHG;                             // Exchange Two operands
extern OpcodeStd2Arg     TEST;                             // Logical Compare. same as AND but doesn't change dst. set SF,ZF,PF according to result
extern OpcodeMov         MOV;                              // Move data (copying)
extern OpcodeLea         LEA;                              // Load effective address

extern OpcodePushPop     PUSH;
extern OpcodePushPop     POP;

extern OpcodeIncDec      INC;
extern OpcodeIncDec      DEC;

extern Opcode1Arg        NOT;                              // Negate the operand, logical NOT
extern Opcode1Arg        NEG;                              // Two's complement negation
extern OpcodeIMul        IMUL;                             // Signed multiply   ah:al=al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src

extern Opcode1Arg        IDIV;                             // Signed divide     ah :al  /= src, al  = quot, ah  = rem. ah  must contain sign extension of al.
                                                           //                   dx :ax  /= src, ax  = quot, dx  = rem. dx  must contain sign extension of ax.
                                                           //                   edx:eax /= src, eax = quot, edx = rem. edx must contain sign extension of eax.
                                                           //                   rdx:rax /= src, rax = quot, rdx = rem. rdx must contain sign extension of rax.

extern Opcode2ArgM       MUL;                              // Unsigned multiply ah:al=al*src, dx:ax=ax*src, edx:eax=eax*src, rdx:rax=rax*src
extern Opcode2ArgM       DIV;                              // Unsigned divide   ah :al  /= src, al  = quot, ah  = rem
                                                           //                   dx :ax  /= src, ax  = quot, dx  = rem
                                                           //                   edx:eax /= src, eax = quot, edx = rem
                                                           //                   rdx:rax /= src, rax = quot, rdx = rem
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

#define JMPSHORT                               B1INS(0xEB)                              // Unconditional short jump. 1 byte PC relative offset
#define JMPNEAR                                B1INS(0xE9)                              // Unconditional near jump. 4 byte PC relative offset
#ifdef IS64BIT
#define JMP_QWORDPTR                           B2INS(0xFF25)                            // Unconditional jump. 4 byte PC relative offset
#endif
#ifdef IS32BIT
#define JCXZSHORT                              B2INS(0x67E3)                            // Jump short if CX  register is 0. 1 byte PC relative offset
#define JECXSHORT                              B1INS(0xE3)                              // Jump short if ECX register is 0  1 byte PC relative offset
#else
#define JEXZSHORT                              B2INS(0x67E3)                            // Jump short if ECX register is 0. 1 byte PC relative offset
#define JRCXSHORT                              B1INS(0xE3)                              // Jump short if RCX register is 0  1 byte PC relative offset
#endif // IS64BIT

// Use less/greater opcode for signed comparison. below/above for unsigned.

// 1 byte PC relative offset
#define JOSHORT                                B1INS(0x70)                              // Jump short if overflow                                 (OF==1 )
#define JNOSHORT                               B1INS(0x71)                              // Jump short if not overflow                             (OF==0 )
#define JBSHORT                                B1INS(0x72)                              // Jump short if below                (unsigned)          (CF==1 )
#define JAESHORT                               B1INS(0x73)                              // Jump short if above or equal       (unsigned)          (CF==0 )
#define JESHORT                                B1INS(0x74)                              // Jump short if equal                (signed/unsigned)   (ZF==1 )
#define JNESHORT                               B1INS(0x75)                              // Jump short if not equal            (signed/unsigned)   (ZF==0 )
#define JBESHORT                               B1INS(0x76)                              // Jump short if below or equal       (unsigned)          (CF==1 || ZF==1)
#define JASHORT                                B1INS(0x77)                              // Jump short if above                (unsigned)          (CF==0 && ZF==0)
#define JSSHORT                                B1INS(0x78)                              // Jump short if sign                                     (SF==1 )
#define JNSSHORT                               B1INS(0x79)                              // Jump short if not sign                                 (SF==0 )
#define JPESHORT                               B1INS(0x7A)                              // Jump short if parity even                              (PF==1 )
#define JPOSHORT                               B1INS(0x7B)                              // Jump short if parity odd                               (PF==0 )
#define JLSHORT                                B1INS(0x7C)                              // Jump short if less                 (signed  )          (SF!=OF)
#define JGESHORT                               B1INS(0x7D)                              // Jump short if greater or equal     (signed  )          (SF==OF)
#define JLESHORT                               B1INS(0x7E)                              // Jump short if less or equal        (signed  )          (ZF==1 || SF!=OF)
#define JGSHORT                                B1INS(0x7F)                              // Jump short if greater              (signed  )          (ZF==0 && SF==OF)

#define JNAESHORT                              JBSHORT                                  // Jump short if not above or equal   (unsigned)
#define JCSHORT                                JBSHORT                                  // Jump short if carry                (unsigned)
#define JNCSHORT                               JAESHORT                                 // Jump short if not carry            (unsigned)
#define JNBSHORT                               JAESHORT                                 // Jump short if not below            (unsigned)
#define JZSHORT                                JESHORT                                  // Jump short if zero                 (signed/unsigned)
#define JNZSHORT                               JNESHORT                                 // Jump short if not zero             (signed/unsigned)
#define JNASHORT                               JBESHORT                                 // Jump short if not above            (unsigned)
#define JNBESHORT                              JASHORT                                  // Jump short if not below or equal   (unsigned)
#define JNGESHORT                              JLSHORT                                  // Jump short if not greater or equal (signed  )
#define JNLSHORT                               JGESHORT                                 // Jump short if not less             (signed  )
#define JNGSHORT                               JLESHORT                                 // Jump short if not greater          (signed  )
#define JNLESHORT                              JGSHORT                                  // Jump short if not less or equal    (signed  )

// 4 byte PC relative offset
#define JONEAR                                 B2INS(0x0F80)                            // Jump near  if overflow
#define JNONEAR                                B2INS(0x0F81)                            // Jump near  if not overflow
#define JBNEAR                                 B2INS(0x0F82)                            // Jump near  if below                 (unsigned)
#define JAENEAR                                B2INS(0x0F83)                            // Jump near  if above or equal        (unsigned)
#define JENEAR                                 B2INS(0x0F84)                            // Jump near  if equal                 (signed/unsigned)
#define JNENEAR                                B2INS(0x0F85)                            // Jump near  if not equal             (signed/unsigned)
#define JBENEAR                                B2INS(0x0F86)                            // Jump near  if below or equal        (unsigned)
#define JANEAR                                 B2INS(0x0F87)                            // Jump near  if above                 (unsigned)
#define JSNEAR                                 B2INS(0x0F88)                            // Jump near  if sign
#define JNSNEAR                                B2INS(0x0F89)                            // Jump near  if not sign
#define JPENEAR                                B2INS(0x0F8A)                            // Jump near  if parity even
#define JPONEAR                                B2INS(0x0F8B)                            // Jump near  if parity odd
#define JLNEAR                                 B2INS(0x0F8C)                            // Jump near  if less                  (signed  )
#define JGENEAR                                B2INS(0x0F8D)                            // Jump near  if greater or equal      (signed  )
#define JLENEAR                                B2INS(0x0F8E)                            // Jump near  if less or equal         (signed  )
#define JGNEAR                                 B2INS(0x0F8F)                            // Jump near  if greater               (signed  )

#define JNAENEAR                               JBNEAR                                   // Jump near  if not above or equal    (unsigned)
#define JCNEAR                                 JBNEAR                                   // Jump near  if carry                 (unsigned)
#define JNCNEAR                                JAENEAR                                  // Jump near  if not carry             (unsigned)
#define JNBNEAR                                JAENEAR                                  // Jump near  if not below             (unsigned)
#define JZNEAR                                 JENEAR                                   // Jump near  if 0                     (signed/unsigned)
#define JNZNEAR                                JNENEAR                                  // Jump near  if not zero              (signed/unsigned)
#define JNANEAR                                JBENEAR                                  // Jump near  if not above             (unsigned)
#define JNBENEAR                               JANEAR                                   // Jump near  if not below or equal    (unsigned)
#define JNGENEAR                               JLNEAR                                   // Jump near  if not greater or equal  (signed  )
#define JNLNEAR                                JGENEAR                                  // Jump near  if not less              (signed  )
#define JNGNEAR                                JLENEAR                                  // Jump near  if not greater           (signed  )
#define JNLENEAR                               JGNEAR                                   // Jump near  if not less or equal     (signed  )



// Set Byte on Condition
extern OpcodeSetcc       SETO;                             // Set byte   if overflow         (OF==1 )
extern OpcodeSetcc       SETNO;                            // Set byte   if not overflow     (OF==0 )
extern OpcodeSetcc       SETB;                             // Set byte   if below            (CF==1 )          (unsigned)
extern OpcodeSetcc       SETAE;                            // Set byte   if above or equal   (CF==0 )          (unsigned)
extern OpcodeSetcc       SETE;                             // Set byte   if equal            (ZF==1 )          (signed/unsigned)
extern OpcodeSetcc       SETNE;                            // Set byte   if not equal        (ZF==0 )          (signed/unsigned)
extern OpcodeSetcc       SETBE;                            // Set byte   if below or equal   (CF==1 || ZF=1)   (unsigned)
extern OpcodeSetcc       SETA;                             // Set byte   if above            (CF==0 && ZF=0)   (unsigned)
extern OpcodeSetcc       SETS;                             // Set byte   if sign             (SF==1 )
extern OpcodeSetcc       SETNS;                            // Set byte   if not sign         (SF==0 )
extern OpcodeSetcc       SETP;                             // Set byte   if parity even      (PF==1 )
extern OpcodeSetcc       SETNP;                            // Set byte   if parity odd       (PF==0 )
extern OpcodeSetcc       SETL;                             // Set byte   if less             (SF!=OF)          (signed  )
extern OpcodeSetcc       SETGE;                            // Set byte   if greater or equal (SF==OF)          (signed  )
extern OpcodeSetcc       SETLE;                            // Set byte   if less or equal    (ZF==1 || SF!=OF) (signed  )
extern OpcodeSetcc       SETG;                             // Set byte   if greater          (ZF==0 && SF==OF) (signed  )

#define                  SETNAE         SETB               // Set byte   if not above or equal                 (unsigned)
#define                  SETC           SETB               // Set byte   if carry                              (unsigned)
#define                  SETNC          SETAE              // Set byte   if not carry                          (unsigned)
#define                  SETNB          SETAE              // Set byte   if not below                          (unsigned)
#define                  SETZ           SETE               // Set byte   if 0                                  (signed/unsigned)
#define                  SETNZ          SETNE              // Set byte   if not zero                           (signed/unsigned)
#define                  SETNA          SETBE              // Set byte   if not above                          (unsigned)
#define                  SETNBE         SETA               // Set byte   if not below or equal                 (unsigned)
#define                  SETNGE         SETL               // Set byte   if not greater or equal               (signed  )
#define                  SETNL          SETGE              // Set byte   if not less                           (signed  )
#define                  SETNG          SETLE              // Set byte   if not greater                        (signed  )
#define                  SETNLE         SETG               // Set byte   if not less or equal                  (signed  )

extern OpcodeJmp         JMP;
extern OpcodeCall        CALL;

extern Opcode0Arg        CWDE;                             // Convert word  to dword. Sign extend AX  into EAX.     Copy sign (bit 15) of AX  into higher 16 bits of EAX
extern Opcode0Arg        CDQ;                              // Convert dword to qword. Sign extend EAX into EDX:EAX. Copy sign (bit 31) of EAX into every     bit  of EDX
extern Opcode0Arg        CBW;                              // Convert byte  to word.  Sign extend AL  into AX.      Copy sign (bit  7) of AL  into higher  8 bits of AX
extern Opcode0Arg        CWD;                              // Convert word  to dword. Sign extend AX  into DX:AX.   Copy sign (bit 15) of AX  into every     bit  of DX

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

// ------------------------------ FPU opcodes ----------------------------------

// ---------------- See http://www.ray.masmcode.com/tutorial/ for documentation ---------

// ----------------------------- FPU transfer opcodes ----------------------------
extern OpcodeFPUTransfer FLD;                              // fld  (src). Push src into st(0).  src={st0-st7,dword ptr,qword ptr,tbyte ptr}
extern OpcodeFPUTransfer FSTP;                             // fstp (dst). Pop st(0) into dst.   dst={st0-st7,dword ptr,qword ptr,tbyte ptr}
extern OpcodeFPUTransfer FST;                              // fst  (dst). Store st(0) into dst. dst={st0-st7,dword ptr,qword ptr}

extern Opcode1Arg        FBLD;                             // fbld (src). Push BCD data from src into st(0).   src=tbyte ptr
extern Opcode1Arg        FBSTP;                            // fbstp(dst). Pop st(0) as BCD data into dst.      dst=tbyte ptr

// ----------------------------- FPU arithmetic opcodes ----------------------------
// For opcode={FADD,FMUL,FSUB,FDIV,FSUBR,FDIVR}, the following rules apply:
// No operand means opcodeP(st(1)).
// If dst not specified, then dst=st(0) and src must be Real4/Real8 in memory.
// If both src and dst are specified, then both must be st(i), and at least 1 must be st(0).
extern OpcodeFPUArithm   FADD;                             // fadd |fadd (src)|fadd (dst,src). dst += src
extern OpcodeFPUArithm   FMUL;                             // fmul |fmul (src)|fmul (dst,src). dst *= src
extern OpcodeFPUArithm   FSUB;                             // fsub |fsub (src)|fsub (dst,src). dst -= src
extern OpcodeFPUArithm   FDIV;                             // fdiv |fdiv (src)|fdiv (dst,src). dst /= src
extern OpcodeFPUArithm   FSUBR;                            // fsubr|fsubr(src)|fsubr(dst,src). dst = src-dst
extern OpcodeFPUArithm   FDIVR;                            // fdivr|fdivr(src)|fdivr(dst,src). dst = src/dst

extern Opcode1Arg        FADDP;                            // faddp (st(i)). st(i) += st(0); pop st(0);
extern Opcode1Arg        FMULP;                            // fmulp (st(i)). st(i) *= st(0); pop st(0);
extern Opcode1Arg        FSUBP;                            // fsubp (st(i)). st(i) -= st(0); pop st(0);
extern Opcode1Arg        FDIVP;                            // fdivp (st(i)). st(i) /= st(0); pop st(0);
extern Opcode1Arg        FSUBRP;                           // fsubrp(st(i)). st(i) =  st(0)-st(i); pop st(0);
extern Opcode1Arg        FDIVRP;                           // fdivrp(st(i)). st(i) =  st(0)/st(i); pop st(0);

// ----------------------------- FPU compare opcodes ----------------------------
extern OpcodeFPUCompare  FCOM;                             // fcom  (src  ). Compare st(0) to src, src={st0..st7,Real4/Real8 in memory}
extern Opcode1Arg        FCOMI;                            // fcomi (st(i)). Compare st(0) to st(i) and set CPU-flags
extern Opcode1Arg        FUCOM;                            // fucom (st(i)). Unordered compare st(0) to st(i)
extern Opcode1Arg        FUCOMI;                           // fucomi(st(i)). Unordered compare st(0) to st(i) and set CPU-flags

extern OpcodeFPUCompare  FCOMP;                            // Same as fcom,   but pop st(0) after compare
extern Opcode1Arg        FCOMIP;                           // Same as fcomi,  but pop st(0) after compare
extern Opcode1Arg        FUCOMP;                           // Same as fucom,  but pop st(0) after compare
extern Opcode1Arg        FUCOMIP;                          // Same as fucomi, but pop st(0) after compare

extern Opcode0Arg        FCOMPP;                           // Compare st(0) to st(1); pop both
extern Opcode0Arg        FUCOMPP;                          // Unordered compare st(0) to st(1); pop both

// ------------------------ FPU integer opcodes ---------------------------------
extern OpcodeFPUIArithm  FILD;                             // fild  (src). Push src into st(0).  src=(signed short/int/__int64 in memory)
extern OpcodeFPUIArithm  FISTP;                            // fistp (dst). Pop   st(0) into dst, rounding according to RC-field FPU control word. dst=(signed short/int/__int64 in memory)
extern OpcodeFPUIArithm  FISTTP;                           // fisttp(dst). Same as fistp, but truncates to nearest integer, regardless of RC-field in FPU control word
extern OpcodeFPUIArithm  FIST;                             // fist  (dst). Store st(0) into dst, rounding according to RC-field FPU control word. dst=(signed short/int in memory)
extern OpcodeFPUIArithm  FIADD;                            // fiadd (src). st(0) += src.         src=(signed short/int in memory)
extern OpcodeFPUIArithm  FIMUL;                            // fimul (src). st(0) *= src.         src=(signed short/int in memory)
extern OpcodeFPUIArithm  FISUB;                            // fisub (src). st(0) -= src.         src=(signed short/int in memory)
extern OpcodeFPUIArithm  FIDIV;                            // fidiv (src). st(0) /= src.         src=(signed short/int in memory)
extern OpcodeFPUIArithm  FISUBR;                           // fisubr(src). st(0) = src-st(0).    src=(signed short/int in memory)
extern OpcodeFPUIArithm  FIDIVR;                           // fidivr(src). st(0) = src/st(0).    src=(signed short/int in memory)
extern OpcodeFPUIArithm  FICOM;                            // ficom (src). Compare st(0) to an integer in memory (signed short/int)
extern OpcodeFPUIArithm  FICOMP;                           // Same as ficom, but pop st(0) after compare

// ------------------- Conditional Copy st(i) to st(0) --------------------------
extern Opcode1Arg        FCMOVB;                           // Copy       if below            (CF==1 )
extern Opcode1Arg        FCMOVAE;                          // Copy       if above or equal   (CF==0 )
extern Opcode1Arg        FCMOVE;                           // Copy       if equal            (ZF==1 )
extern Opcode1Arg        FCMOVNE;                          // Copy       if not equal        (ZF==0 )
extern Opcode1Arg        FCMOVBE;                          // Copy       if below or equal   (CF==1 || ZF==1)
extern Opcode1Arg        FCMOVA;                           // Copy       if above            (CF==0 && ZF==0)
extern Opcode1Arg        FCMOVU;                           // Copy       if unordered        (PF==1 )
extern Opcode1Arg        FCMOVNU;                          // Copy       if not unordered    (PF==0 )

extern Opcode1Arg        FFREE;                            // ffree(st(i)). Free a data register
extern Opcode1Arg        FXCH;                             // fxch(st(i).   Swap st(0) and st(i)
extern Opcode0Arg        FNSTSWAX;                         // Store status word into CPU register AX
extern Opcode0Arg        FWAIT;                            // Wait while FPU is busy
extern Opcode0Arg        FNOP;                             // No operation
extern Opcode0Arg        FCHS;                             // st(0) = -st(0)
extern Opcode0Arg        FABS;                             // st(0) = abs(st(0))
extern Opcode0Arg        FTST;                             // Compare st(0) to 0.0
extern Opcode0Arg        FXAM;                             // Examine the content of st(0)
extern Opcode0Arg        FLD1;                             // push 1.0
extern Opcode0Arg        FLDL2T;                           // push log2(10)
extern Opcode0Arg        FLDL2E;                           // push log2(e)
extern Opcode0Arg        FLDPI;                            // push pi
extern Opcode0Arg        FLDLG2;                           // push log10(2)
extern Opcode0Arg        FLDLN2;                           // push ln(2)
extern Opcode0Arg        FLDZ;                             // push 0.0
extern Opcode0Arg        F2XM1;                            // st(0) = 2^st(0)-1, assume -1 <= st(0) <= 1
extern Opcode0Arg        FYL2X;                            // st(1) = log2(st(0))*st(1); pop st(0)
extern Opcode0Arg        FPTAN;                            // st(0) = tan(st(0)); push 1.0
extern Opcode0Arg        FPATAN;                           // st(1) = atan(st(1)/st(0)); pop st(0)
extern Opcode0Arg        FXTRACT;                          // st(0) = unbiased exponent in floating point format of st(0). then push signinificant wiht exponent 0
extern Opcode0Arg        FPREM1;                           // As FPREM. Magnitude of the remainder <= ST(1) / 2
extern Opcode0Arg        FDECSTP;                          // Decrement stack pointer. st0->st1, st7->st0, ..., st1->st2
extern Opcode0Arg        FINCSTP;                          // Increment stack pointer. st0->st7, st1->st0, ..., st7->st6
extern Opcode0Arg        FPREM;                            // Partial remainder. st(0) %= st(1). Exponent of st(0) reduced with at most 63
extern Opcode0Arg        FYL2XP1;                          // st(1) = log2(st(0)+1)*st(1); pop st(0)
extern Opcode0Arg        FSQRT;                            // st(0) = sqrt(st(0))
extern Opcode0Arg        FSINCOS;                          // Sine and cosine of the angle value in ST(0), st(0)=sin; push(cos)
extern Opcode0Arg        FRNDINT;                          // st(0) = nearest integral value according to the rounding mode
extern Opcode0Arg        FSCALE;                           // st(0) *= 2^int(st(1))
extern Opcode0Arg        FSIN;                             // st(0) = sin(ST(0))
extern Opcode0Arg        FCOS;                             // st(0) = cos(ST(0))

extern Opcode1Arg        FLDCW;                            // load control word.  op=word ptr
extern Opcode1Arg        FNSTCW;                           // store control word. op=word ptr
extern Opcode1Arg        FNSTSW;                           // store status word.  op=word ptr

// ---------------------------- XMM opcodes -----------------------------------------

extern Opcode2Arg        MOVAPS;
extern Opcode2ArgPfxF2SD MOVSD1;
extern Opcode2ArgPfxF2SD ADDSD;
extern Opcode2ArgPfxF2SD MULSD;
extern Opcode2ArgPfxF2SD SUBSD;
extern Opcode2ArgPfxF2SD DIVSD;
