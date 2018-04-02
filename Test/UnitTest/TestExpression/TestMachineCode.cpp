#include "stdafx.h"
#include <HashSet.h>
#include <Math/Expression/NewOpcode.h>

#define TEST_MACHINECODE

#ifdef TEST_MACHINECODE

// #define TEST_ALLGPREGISTERS

static const GPRegister r8List[] = {
#ifndef TEST_ALLGPREGISTERS
    AL   ,CL                                 ,BH
#else
    AL   ,CL   ,DL   ,BL   ,AH   ,CH   ,DH   ,BH
#endif // TEST_ALLGPREGISTERS
#ifdef IS64BIT
#ifndef TEST_ALLGPREGISTERS
                           ,SPL              ,DIL
   ,R8B                                      ,R15B
#else
                           ,SPL  ,BPL  ,SIL  ,DIL
   ,R8B  ,R9B  ,R10B ,R11B ,R12B ,R13B ,R14B ,R15B
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
};

const GPRegister r16List[] = {
#ifndef TEST_ALLGPREGISTERS
    AX                                       ,DI
#else
    AX   ,CX   ,DX   ,BX   ,SP   ,BP   ,SI   ,DI
#endif // TEST_ALLGPREGISTERS
#ifdef IS64BIT
#ifndef TEST_ALLGPREGISTERS
   ,R8W                                      ,R15W
#else
   ,R8W  ,R9W  ,R10W ,R11W ,R12W ,R13W ,R14W ,R15W
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
};

const GPRegister r32List[] = {
#ifndef TEST_ALLGPREGISTERS
    EAX                                      ,EDI
#else
    EAX  ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLGPREGISTERS
#ifdef IS64BIT
#ifndef TEST_ALLGPREGISTERS
   ,R8D                                      ,R15D
#else
   ,R8D  ,R9D  ,R10D ,R11D ,R12D ,R13D ,R14D ,R15D
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
};

const IndexRegister indexRegList[] = {
#ifdef IS32BIT
#ifndef TEST_ALLGPREGISTERS
    EAX                    ,ESP  ,EBP        ,EDI
#else
    EAX  ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLGPREGISTERS
#else // IS64BIT
#ifndef TEST_ALLGPREGISTERS
    RAX                    ,RSP  ,RBP        ,RDI
   ,R8                     ,R12  ,R13        ,R15
#else
    RAX  ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
   ,R8   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
};

#ifdef IS64BIT
const GPRegister r64List[] = {
#ifndef TEST_ALLGPREGISTERS
    RAX                                      ,RDI
   ,R8                                       ,R15
#else
    RAX  ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
   ,R8   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif //  TEST_ALLGPREGISTERS
};
#endif // IS64BIT

const SegmentRegister segregList[] = {
#ifdef IS32BIT
  ES,CS,SS,DS,FS,GS
#else  // IS64BIT
  FS,GS
#endif // IS64BIT
};

#define INDEXREGISTER_COUNT ARRAYSIZE(indexRegList)

#define UNKNOWN_OPCODE(  dst)               B2INSA(0x8700 + ((dst)<<3))                     // Build src with MEM_ADDR-macros, REGREG
//Real FMOD(Real x, Real y) {
//  return fmod(x,y);
//}

static const RegSize allRegSize[] = {
  REGSIZE_BYTE     /* 8-bit   */
 ,REGSIZE_WORD     /* 16-bit  */
 ,REGSIZE_DWORD    /* 32-bit  */
 ,REGSIZE_QWORD    /* 64-bit  */
 ,REGSIZE_TBYTE    /* 80-bit  */
 ,REGSIZE_OWORD    /* 128-bit */
};

class InstructionOperandArray : public CompactArray<const InstructionOperand*> {
public:
  virtual ~InstructionOperandArray();
};

InstructionOperandArray::~InstructionOperandArray() {
  for(size_t i = size(); i--;) delete (*this)[i];
  clear();
}

class AllGPRegisters : public InstructionOperandArray {
private:
  void addRegArray(const GPRegister *list, size_t n) {
    for(size_t i = 0; i < n; i++) {
      add(new InstructionOperand(list[i]));
    }
  }
  void addRegArray(const SegmentRegister *list, size_t n) {
    for(size_t i = 0; i < n; i++) {
      add(new InstructionOperand(list[i]));
    }
  }
public:
  AllGPRegisters();
};

AllGPRegisters::AllGPRegisters() {
  addRegArray(segregList, ARRAYSIZE(segregList));
  addRegArray(r8List    , ARRAYSIZE(r8List    ));
  addRegArray(r16List   , ARRAYSIZE(r16List   ));
  addRegArray(r32List   , ARRAYSIZE(r32List   ));
#ifdef IS64BIT
  addRegArray(r64List   , ARRAYSIZE(r64List   ));
#endif
}

#ifdef IS32BIT
static const int   allImmValues[] = {    0x7f, 0x7fff, 0x7fffffff     };
#else // IS64BIT
static const INT64 allImmValues[] = {    0x7f, 0x7fff, 0x7fffffff, 0x7fffffffffffffffi64 };
#endif // IS64BIT

class AllImmOperands : public InstructionOperandArray {
public:
  AllImmOperands();
};

AllImmOperands::AllImmOperands() {
  for(int i = 0; i < ARRAYSIZE(allImmValues); i++) {
    add(new InstructionOperand(allImmValues[i]));
  }
}

#ifdef IS32BIT
static const size_t allImmAddr[]   = { 0, 0x7fffffff };
#else  // IS64BIT
static const size_t allImmAddr[]   = { 0, 0x7fffffff, 0x7fffffffffffffff };
#endif // IS64BIT

static const int    allOffset[]    = { 0, 0x7f, 0x7fffffff, -1 };

class AllMemoryOperands : public InstructionOperandArray {
public:
  AllMemoryOperands();
};

static inline int boolCmp(bool b1, bool b2) {
  return (char)b1 - (char)b2;
}

static inline int registerCmp(const Register &reg1, const Register &reg2) {
  return (int)reg1.getIndex() - (int)reg2.getIndex();
}

static int offsetCmp(const MemoryRef &mr1, const MemoryRef &mr2) {
  int c;
  if(c = boolCmp(mr1.hasOffset(), mr2.hasOffset())) return c;
  if(!mr1.hasOffset()) return 0;
  if(c = boolCmp(isByte(mr2.getOffset()), isByte(mr1.getOffset()))) return c;
  return sign((INT64)mr1.getOffset() - (INT64)mr2.getOffset());
}

static int addrCmp(const MemoryRef &mr1, const MemoryRef &mr2) {
  int c;
  if(c = boolCmp(mr1.hasOffset(), mr2.hasOffset())) return c;
  if(!mr1.hasOffset()) return 0;
  if(c = boolCmp(isByte(mr2.getAddr()), isByte(mr1.getAddr()))) return c;
  if(c = boolCmp(isDword(mr2.getAddr()), isDword(mr1.getAddr()))) return c;
  return sign((INT64)mr1.getAddr() - (INT64)mr2.getAddr());
}

static int memRefCmp(const MemoryRef &mr1, const MemoryRef &mr2) {
  int c;
  if(c = boolCmp(mr2.isDisplaceOnly(),mr1.isDisplaceOnly())) return c;
  if(mr1.isDisplaceOnly()) {
    return addrCmp(mr1,mr2);
  } else {
    if(c = boolCmp(mr1.hasInx(),mr2.hasInx())) return c;
    if(!mr1.hasInx()) {
      if(c = offsetCmp(mr1,mr2)) return c;
    }
    if(c = boolCmp(mr1.hasBase(), mr2.hasBase())) return c;
    if(mr1.hasBase()) { // && mr2.hasBase()
      if(c = registerCmp(*mr1.getBase(), *mr2.getBase())) return c;
    }
    if(mr1.hasInx()) { // && mr2.hasInx()
      if(c = offsetCmp(mr1,mr2)) return c;
      if(c = (int)mr1.getShift() - (int)mr2.getShift()) return c;
      if(c = registerCmp(*mr1.getInx(),*mr2.getInx())) return c;
    }
  }
  return offsetCmp(mr1,mr2);
}

static int memOpCmp(const InstructionOperand * const &o1, const InstructionOperand * const &o2) {
  int c = (int)(o1->getSize()) - (int)(o2->getSize());
  if(c) return c;
  if(c = memRefCmp(o1->getMemoryReference(), o2->getMemoryReference())) return c;
  if(c = boolCmp(o1->hasSegmentRegister(),o2->hasSegmentRegister())) return c;
  if(!o1->hasSegmentRegister()) return 0; // && !o2->hasSegmentRegister()
  return registerCmp(*o1->getSegmentRegister(), *o2->getSegmentRegister());
}

AllMemoryOperands::AllMemoryOperands() {
  for(int i = 0; i < ARRAYSIZE(allImmAddr); i++) {
    add(new BYTEPtr( allImmAddr[i]));
    add(new WORDPtr( allImmAddr[i]));
    add(new DWORDPtr(allImmAddr[i]));
    add(new QWORDPtr(allImmAddr[i]));
  }
  for(int k = 0; k < INDEXREGISTER_COUNT; k++) {
    const IndexRegister &inxReg = indexRegList[k];
    if(!inxReg.isValidIndexRegister()) continue;
    for(int factor = 1; factor <= 8; factor *= 2) {
      for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
        const int offset = allOffset[i];
        add(new BYTEPtr( factor*inxReg + offset));
        add(new WORDPtr( factor*inxReg + offset));
        add(new DWORDPtr(factor*inxReg + offset));
        add(new QWORDPtr(factor*inxReg + offset));
      }
    }
  }
  for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
    const int offset = allOffset[i];
    for(int j = 0; j < INDEXREGISTER_COUNT; j++) {
      const IndexRegister &baseReg = indexRegList[j];
      add(new BYTEPtr( baseReg + offset));
      add(new WORDPtr( baseReg + offset));
      add(new DWORDPtr(baseReg + offset));
      add(new QWORDPtr(baseReg + offset));
      for(int k = 0; k < INDEXREGISTER_COUNT; k++) {
        const IndexRegister &inxReg = indexRegList[k];
        if(!inxReg.isValidIndexRegister()) continue;
        for(int factor = 1; factor <= 8; factor *= 2) {
          add(new BYTEPtr( baseReg + factor*inxReg + offset));
          add(new WORDPtr( baseReg + factor*inxReg + offset));
          add(new DWORDPtr(baseReg + factor*inxReg + offset));
          add(new QWORDPtr(baseReg + factor*inxReg + offset));
        }
      }
    }
  }

  AllMemoryOperands copy(*this);
  clear();
  StringHashSet strSet;
  for(size_t i = 0; i < copy.size(); i++) {
    const InstructionOperand *ins = copy[i];
    const String s = ins->toString();
    if(!strSet.contains(s)) {
      strSet.add(s);
      add(ins);
    } else {
      delete ins;
    }
  }
  copy.clear();

  sort(memOpCmp);

  redirectDebugLog();

//  for(size_t i = 0; i < size(); i++) {
//    debugLog(_T("%s\n"),(*this)[i]->toString().cstr());
//  }
}

class AllVOIDPtrOperands : public InstructionOperandArray {
public:
  AllVOIDPtrOperands &operator=(const AllMemoryOperands &src);
};

AllVOIDPtrOperands &AllVOIDPtrOperands::operator=(const AllMemoryOperands &src) {
  for(Iterator<const InstructionOperand*> it = ((AllMemoryOperands&)src).getIterator(); it.hasNext();) {
    const MemoryOperand &op = (MemoryOperand&)*it.next();
    if(op.getSize() == REGSIZE_BYTE) {
      add(new VOIDPtr(op));
    }
  }
  return *this;
}

class AllStringInstructions : public CompactArray<const StringInstruction*> {
public:
  AllStringInstructions();
};

AllStringInstructions::AllStringInstructions() {
  add(&MOVSB);
  add(&CMPSB);
  add(&STOSB);
  add(&LODSB);
  add(&SCASB);

  add(&MOVSW);
  add(&CMPSW);
  add(&STOSW);
  add(&LODSW);
  add(&SCASW);

  add(&MOVSD);
  add(&CMPSD);
  add(&STOSD);
  add(&LODSD);
  add(&SCASD);

#ifdef IS64BIT
  add(&MOVSQ);
  add(&CMPSQ);
  add(&STOSQ);
  add(&LODSQ);
  add(&SCASQ);
#endif // IS64BIT
}

typedef enum {
  BREAK_GROUP
 ,BREAK_OPCODE
} InterruptType;

class UserInterrupt : public Exception {
private:
  const InterruptType m_type;
public:
  inline UserInterrupt(InterruptType type) : Exception(_T("User interrupt")), m_type(type) {
  }
  inline InterruptType getType() const {
    return m_type;
  }
};

class CodeArray : public ExecutableByteArray {
private:
  DECLARECLASSNAME;
  static UINT             s_emitCount;
  static void checkKeyboard();
  int         addBytes(const void *bytes, int count);
public:
  inline int emit(const InstructionBase &ins) {
    if((s_emitCount++ & 0x3ff) == 0) checkKeyboard();
    return addBytes(ins.getBytes(), ins.size());
  }
  void clear() {
    s_emitCount = 0;
    __super::clear();
  }
};

UINT CodeArray::s_emitCount = 0;

int CodeArray::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  append((BYTE*)bytes,count);
  return ret;
}

void CodeArray::checkKeyboard() { // static
  if(shiftKeyPressed()) {
    throw UserInterrupt(BREAK_GROUP);
  }
  if(ctrlKeyPressed()) {
    throw UserInterrupt(BREAK_OPCODE);
  }
}

class TestMachineCode : public CodeArray {
private:
  InstructionOperandArray m_allOperands;
  AllVOIDPtrOperands      m_allVOIDPtrOperands;
  AllStringInstructions   m_allStringInstructions;
  String                  m_currentName;
  void initAllOperands();
  void clear();
  int  emit(               const InstructionBase &ins   );
  int  emit(               const OpcodeBase      &opcode, const InstructionOperand &op);
  int  emit(               const OpcodeBase      &opcode, const InstructionOperand &op1, const InstructionOperand &op2);
  int  emit(               const OpcodeBase      &opcode, const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3);
  void testOpcode1Arg(     const OpcodeBase      &opcode);
  void testOpcode2Arg(     const OpcodeBase      &opcode, bool selectVOIDPtr);
  void testOpcode3Arg(     const OpcodeBase      &opcode);
public:
  void testOpcode(         const OpcodeBase      &opcode, bool selectVOIDPtr = false);
  void testOpcode(         const OpcodeLea       &opcode);
  void testOpcode(         const StringPrefix    &prefix);
  void testArg0Opcodes();
  void testArg1Opcodes();
  void testArg2Opcodes();
  void testArg3Opcodes();
  void testSetccOpcodes();
  void testBitOperations();
  void testStringInstructions();
  TestMachineCode();
};

void TestMachineCode::initAllOperands() {
  AllGPRegisters    m_allGPReg;
  AllMemoryOperands m_allMemOperands;
  AllImmOperands    m_allImmOperands;
  m_allVOIDPtrOperands = m_allMemOperands;
  m_allOperands.addAll(m_allGPReg      ); m_allGPReg.clear();
  m_allOperands.addAll(m_allMemOperands); m_allMemOperands.clear();
  m_allOperands.addAll(m_allImmOperands); m_allImmOperands.clear();
}

void TestMachineCode::clear() {
  __super::clear();
  redirectDebugLog();
}

int TestMachineCode::emit(const InstructionBase &ins) {
  debugLog(_T("%-36s %s\n"), ins.toString().cstr(), m_currentName.cstr());
  return __super::emit(ins);
}

int TestMachineCode::emit(const OpcodeBase &opcode, const InstructionOperand &op) {
  const InstructionBase ins = opcode(op);
  debugLog(_T("%-36s %s %s\n"), ins.toString().cstr(), m_currentName.cstr(), op.toString().cstr());
  return __super::emit(ins);
}

int TestMachineCode::emit(const OpcodeBase &opcode, const InstructionOperand &op1, const InstructionOperand &op2) {
  const InstructionBase ins = opcode(op1,op2);
  debugLog(_T("%-36s %s %s, %s\n"), ins.toString().cstr(), m_currentName.cstr(), op1.toString().cstr(), op2.toString().cstr());
  return __super::emit(ins);
}

int TestMachineCode::emit(const OpcodeBase &opcode, const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) {
  const InstructionBase ins = opcode(op1,op2,op3);
  debugLog(_T("%-36s %s %s,%s,%s\n"), ins.toString().cstr(), m_currentName.cstr(), op1.toString().cstr(), op2.toString().cstr(),op3.toString().cstr());
  return __super::emit(ins);
}

void TestMachineCode::testOpcode(const OpcodeBase &opcode, bool selectVOIDPtr) {
  try {
    m_currentName = opcode.getMnemonic();
    if(opcode.getMaxOpCount() == 0) {
      emit((Opcode0Arg&)opcode            );
    } else {
      clear();
      for(int args = opcode.getOpCount(); args <= opcode.getMaxOpCount(); args++) {
        switch(args) {
        case 1 : testOpcode1Arg(opcode               ); break;
        case 2 : testOpcode2Arg(opcode, selectVOIDPtr); break;
        case 3 : testOpcode3Arg(opcode               ); break;
        default: throwInvalidArgumentException(__TFUNCTION__,_T("%s.getMaxOpCount()=%d"), opcode.getMnemonic().cstr(), opcode.getMaxOpCount());
        }
      }
    }
  } catch(UserInterrupt u) {
    if(u.getType() != BREAK_OPCODE) {
      throw;
    }
  }
}

void TestMachineCode::testOpcode(const OpcodeLea &opcode) {
  testOpcode(opcode,true);
}

void TestMachineCode::testOpcode1Arg(const OpcodeBase &opcode) {
  for(Iterator<const InstructionOperand*> opIt = m_allOperands.getIterator(); opIt.hasNext();) {
    const InstructionOperand &op = *opIt.next();
    if(opcode.isValidOperand(op)) {
      emit(opcode,op);
    }
  }
}

void TestMachineCode::testOpcode2Arg(const OpcodeBase &opcode, bool selectVOIDPtr) {
  Iterator<const InstructionOperand*> opIt2 = selectVOIDPtr?m_allVOIDPtrOperands.getIterator() : m_allOperands.getIterator();
  while(opIt2.hasNext()) {
    const InstructionOperand &op2 = *opIt2.next();
    for(Iterator<const InstructionOperand*> opIt1 = m_allOperands.getIterator(); opIt1.hasNext();) {
      const InstructionOperand &op1 = *opIt1.next();
      if(opcode.isValidOperandCombination(op1,op2)) {
        emit(opcode,op1,op2);
      }
    }
  }
}

void TestMachineCode::testOpcode3Arg(const OpcodeBase &opcode) {
  for(Iterator<const InstructionOperand*> opIt3 = m_allOperands.getIterator(); opIt3.hasNext();) {
    const InstructionOperand &op3 = *opIt3.next();
    if(!opcode.isValidOperandType(op3,3)) continue;
    for(Iterator<const InstructionOperand*> opIt2 = m_allOperands.getIterator(); opIt2.hasNext();) {
      const InstructionOperand &op2 = *opIt2.next();
      for(Iterator<const InstructionOperand*> opIt1 = m_allOperands.getIterator(); opIt1.hasNext();) {
        const InstructionOperand &op1 = *opIt1.next();
        if(opcode.isValidOperandCombination(op1,op2,op3)) {
          emit(opcode,op1,op2,op3);
        }
      }
    }
  }
}

void TestMachineCode::testOpcode(const StringPrefix &prefix) {
  m_currentName = prefix.getMnemonic();
  for(Iterator<const StringInstruction*> it = m_allStringInstructions.getIterator(); it.hasNext();) {
    const StringInstruction &ins = *it.next();
    emit(prefix(ins));
  }
}

void TestMachineCode::testArg0Opcodes() {
  clear();
  testOpcode(RET    );
  testOpcode(CMC    );
  testOpcode(CLC    );
  testOpcode(STC    );
  testOpcode(CLI    );
  testOpcode(STI    );
  testOpcode(CLD    );
  testOpcode(STD    );
  testOpcode(SAHF   );
  testOpcode(LAHF   );
  testOpcode(PUSHF  );
  testOpcode(POPF   );
#ifdef IS32BIT
  testOpcode(PUSHFD );
  testOpcode(POPFD  );
  testOpcode(PUSHAD );
  testOpcode(POPAD  );
#else // IS64BIT
  testOpcode(PUSHFQ );
  testOpcode(POPFQ  );
#endif // IS64BIT
  testOpcode(NOOP   );
  testOpcode(CBW    );
  testOpcode(CWDE   );
  testOpcode(CWD    );
  testOpcode(CDQ    );

#ifdef IS64BIT
  testOpcode(CDQE   );
  testOpcode(CQO    );
  testOpcode(CLGI   );
  testOpcode(STGI   );
#endif // IS64BIT
}

void TestMachineCode::testArg1Opcodes() {
  testOpcode(PUSH );
  testOpcode(POP  );
  testOpcode(INC  );
  testOpcode(DEC  );
  testOpcode(NOT  );
  testOpcode(NEG  );
  testOpcode(MUL  );
  testOpcode(IMUL );
  testOpcode(DIV  );
  testOpcode(IDIV );
}

void TestMachineCode::testArg2Opcodes() {
  testOpcode(ADD    );
  testOpcode(ADC    );
  testOpcode(OR     );
  testOpcode(AND    );
  testOpcode(SUB    );
  testOpcode(SBB    );
  testOpcode(XOR    );
  testOpcode(CMP    );
  testOpcode(XCHG   );
  testOpcode(MOV    );
  testOpcode(LEA    );
}

void TestMachineCode::testArg3Opcodes() {
  testOpcode(SHLD   );
  testOpcode(SHRD   );
}

void TestMachineCode::testSetccOpcodes() {
  testOpcode(SETO );
  testOpcode(SETNO);
  testOpcode(SETB );
  testOpcode(SETAE);
  testOpcode(SETE );
  testOpcode(SETNE);
  testOpcode(SETBE);
  testOpcode(SETA );
  testOpcode(SETS );
  testOpcode(SETNS);
  testOpcode(SETPE);
  testOpcode(SETPO);
  testOpcode(SETL );
  testOpcode(SETGE);
  testOpcode(SETLE);
  testOpcode(SETG );
}

void TestMachineCode::testBitOperations() {
  testOpcode(ROL    );
  testOpcode(ROR    );
  testOpcode(RCL    );
  testOpcode(RCR    );
  testOpcode(SHL    );
  testOpcode(SHR    );
  testOpcode(SAR    );
  testOpcode(BSF    );
  testOpcode(BSR    );
}

void TestMachineCode::testStringInstructions() {
  clear();
  testOpcode(MOVSB  );
  testOpcode(CMPSB  );
  testOpcode(STOSB  );
  testOpcode(LODSB  );
  testOpcode(SCASB  );

  testOpcode(MOVSW  );
  testOpcode(CMPSW  );
  testOpcode(STOSW  );
  testOpcode(LODSW  );
  testOpcode(SCASW  );

  testOpcode(MOVSD  );
  testOpcode(CMPSD  );
  testOpcode(STOSD  );
  testOpcode(LODSD  );
  testOpcode(SCASD  );

#ifdef IS64BIT
  testOpcode(MOVSQ  );
  testOpcode(CMPSQ  );
  testOpcode(STOSQ  );
  testOpcode(LODSQ  );
  testOpcode(SCASQ  );
#endif // IS64BIT

  testOpcode(REP    );
  testOpcode(REPE   );
  testOpcode(REPNE  );
}

TestMachineCode::TestMachineCode() {
  initAllOperands();
  for(int i = 0; i >= 0;i++) {
    try {
      switch(i) {
      case 0 : testArg0Opcodes();         break;
      case 1 : testArg1Opcodes();         break;
      case 2 : testArg2Opcodes();         break;
      case 3 : testArg3Opcodes();         break;
      case 4 : testSetccOpcodes();        break;
      case 5 : testBitOperations();       break;
      case 6 : testStringInstructions();  break;
      default: i = -1000; break;
      }
    } catch(UserInterrupt u) {
      if(u.getType() != BREAK_GROUP) {
        throw;
      }
    }
  }
}

#endif // TEST_MACHINECODE

void generateTestSequence() {
#ifdef TEST_MACHINECODE
  TestMachineCode test;
#endif // TEST_MACHINECODE
}

#ifdef _DEBUG
#ifdef TEST_MACHINECODE

#ifdef IS64BIT
extern "C" void assemblerCode();
#else

#define NOP __asm { _emit 0x90 }

// Returns address of instruction following call to this fnuction (return-address)
static BYTE *getIP() {
  BYTE *ip;
  __asm {
    mov eax, dword ptr[esp+8]
    mov ip, eax
  }
  return ip;
}
void assemblerCode() {
  const BYTE *startIP = getIP();
  __asm {
    jmp         End
    nop
    imul al
    imul cl
    imul dl
    imul bl
    imul ax
    imul cx
    imul dx
    imul bx
    imul eax
    imul ecx
    imul edx
    imul ebx
    nop
    imul  ax ,  ax
    imul  ax ,  cx
    imul  ax ,  dx
    imul  ax ,  bx
    imul  cx ,  ax
    imul  cx ,  cx
    imul  cx ,  dx
    imul  cx ,  bx
    imul  dx ,  ax
    imul  dx ,  cx
    imul  dx ,  dx
    imul  dx ,  bx
    imul  bx ,  ax
    imul  bx ,  cx
    imul  bx ,  dx
    imul  bx ,  bx
    imul  ax , word ptr[eax]
    imul  ax , word ptr[ecx]
    imul  ax , word ptr[edx]
    imul  ax , word ptr[ebx]
    imul  cx , word ptr[eax]
    imul  cx , word ptr[ecx]
    imul  cx , word ptr[edx]
    imul  cx , word ptr[ebx]
    imul  dx , word ptr[eax]
    imul  dx , word ptr[ecx]
    imul  dx , word ptr[edx]
    imul  dx , word ptr[ebx]
    imul  bx , word ptr[eax]
    imul  bx , word ptr[ecx]
    imul  bx , word ptr[edx]
    imul  ax , word ptr[ebx]
    nop
    imul eax , eax
    imul eax , ecx
    imul eax , edx
    imul eax , ebx
    imul ecx , eax
    imul ecx , ecx
    imul ecx , edx
    imul ecx , ebx
    imul edx , eax
    imul edx , ecx
    imul edx , edx
    imul ebx , ebx
    imul ebx , eax
    imul ebx , ecx
    imul ebx , edx
    imul ebx , ebx
    imul eax ,dword ptr[eax]
    imul eax ,dword ptr[ecx]
    imul eax ,dword ptr[edx]
    imul eax ,dword ptr[ebx]
    imul ecx ,dword ptr[eax]
    imul ecx ,dword ptr[ecx]
    imul ecx ,dword ptr[edx]
    imul ecx ,dword ptr[ebx]
    imul edx ,dword ptr[eax]
    imul edx ,dword ptr[ecx]
    imul edx ,dword ptr[edx]
    imul edx ,dword ptr[ebx]
    imul ebx ,dword ptr[eax]
    imul ebx ,dword ptr[ecx]
    imul ebx ,dword ptr[edx]
    imul ebx ,dword ptr[ebx]
    nop
    imul  ax , 7fh
    imul  cx , 7fh
    imul  dx , 7fh
    imul  bx , 7fh
    imul eax , 7fh
    imul ecx , 7fh
    imul edx , 7fh
    imul ebx , 7fh
    imul eax , 7fffffffh
    imul ecx , 7fffffffh
    imul edx , 7fffffffh
    imul ebx , 7fffffffh
    nop
    imul  ax ,  ax          ,7fh
    imul  ax ,  cx          ,7fh
    imul  ax ,  dx          ,7fh
    imul  ax ,  bx          ,7fh
    imul  cx ,  ax          ,7fh
    imul  cx ,  cx          ,7fh
    imul  cx ,  dx          ,7fh
    imul  cx ,  bx          ,7fh
    imul  dx ,  ax          ,7fh
    imul  dx ,  cx          ,7fh
    imul  dx ,  dx          ,7fh
    imul  dx ,  bx          ,7fh
    imul  bx ,  ax          ,7fh
    imul  bx ,  cx          ,7fh
    imul  bx ,  dx          ,7fh
    imul  bx ,  bx          ,7fh
    imul  ax ,  ax          ,7fffh
    imul  ax ,  cx          ,7fffh
    imul  ax ,  dx          ,7fffh
    imul  ax ,  bx          ,7fffh
    imul  cx ,  ax          ,7fffh
    imul  cx ,  cx          ,7fffh
    imul  cx ,  dx          ,7fffh
    imul  cx ,  bx          ,7fffh
    imul  dx ,  ax          ,7fffh
    imul  dx ,  cx          ,7fffh
    imul  dx ,  dx          ,7fffh
    imul  dx ,  bx          ,7fffh
    imul  bx ,  ax          ,7fffh
    imul  bx ,  cx          ,7fffh
    imul  bx ,  dx          ,7fffh
    imul  bx ,  bx          ,7fffh
    imul  ax , word ptr[eax],7fh
    imul  ax , word ptr[ecx],7fh
    imul  ax , word ptr[edx],7fh
    imul  ax , word ptr[ebx],7fh
    imul  cx , word ptr[eax],7fh
    imul  cx , word ptr[ecx],7fh
    imul  cx , word ptr[edx],7fh
    imul  cx , word ptr[ebx],7fh
    imul  dx , word ptr[eax],7fh
    imul  dx , word ptr[ecx],7fh
    imul  dx , word ptr[edx],7fh
    imul  dx , word ptr[ebx],7fh
    imul  bx , word ptr[eax],7fh
    imul  bx , word ptr[ecx],7fh
    imul  bx , word ptr[edx],7fh
    imul  bx , word ptr[ebx],7fh
    imul  ax , word ptr[eax],7fffh
    imul  ax , word ptr[ecx],7fffh
    imul  ax , word ptr[edx],7fffh
    imul  ax , word ptr[ebx],7fffh
    imul  cx , word ptr[eax],7fffh
    imul  cx , word ptr[ecx],7fffh
    imul  cx , word ptr[edx],7fffh
    imul  cx , word ptr[ebx],7fffh
    imul  dx , word ptr[eax],7fffh
    imul  dx , word ptr[ecx],7fffh
    imul  dx , word ptr[edx],7fffh
    imul  dx , word ptr[ebx],7fffh
    imul  bx , word ptr[eax],7fffh
    imul  bx , word ptr[ecx],7fffh
    imul  bx , word ptr[edx],7fffh
    imul  bx , word ptr[ebx],7fffh
    nop
    imul eax , eax          ,7fh
    imul eax , ecx          ,7fh
    imul eax , edx          ,7fh
    imul eax , ebx          ,7fh
    imul ecx , eax          ,7fh
    imul ecx , ecx          ,7fh
    imul ecx , edx          ,7fh
    imul ecx , ebx          ,7fh
    imul edx , eax          ,7fh
    imul edx , ecx          ,7fh
    imul edx , edx          ,7fh
    imul edx , ebx          ,7fh
    imul ebx , eax          ,7fh
    imul ebx , ecx          ,7fh
    imul ebx , edx          ,7fh
    imul ebx , ebx          ,7fh
    imul eax ,dword ptr[eax],7fh
    imul eax ,dword ptr[ecx],7fh
    imul eax ,dword ptr[edx],7fh
    imul eax ,dword ptr[ebx],7fh
    imul ecx ,dword ptr[eax],7fh
    imul ecx ,dword ptr[ecx],7fh
    imul ecx ,dword ptr[edx],7fh
    imul ecx ,dword ptr[ebx],7fh
    imul edx ,dword ptr[eax],7fh
    imul edx ,dword ptr[ecx],7fh
    imul edx ,dword ptr[edx],7fh
    imul edx ,dword ptr[ebx],7fh
    imul ebx ,dword ptr[eax],7fh
    imul ebx ,dword ptr[ecx],7fh
    imul ebx ,dword ptr[edx],7fh
    imul ebx ,dword ptr[ebx],7fh
    nop
    imul eax , eax          ,7fffffffh
    imul eax , ecx          ,7fffffffh
    imul eax , edx          ,7fffffffh
    imul eax , ebx          ,7fffffffh
    imul ecx , eax          ,7fffffffh
    imul ecx , ecx          ,7fffffffh
    imul ecx , edx          ,7fffffffh
    imul ecx , ebx          ,7fffffffh
    imul edx , eax          ,7fffffffh
    imul edx , ecx          ,7fffffffh
    imul edx , edx          ,7fffffffh
    imul edx , ebx          ,7fffffffh
    imul ebx , eax          ,7fffffffh
    imul ebx , ecx          ,7fffffffh
    imul ebx , edx          ,7fffffffh
    imul ebx , ebx          ,7fffffffh
    imul eax ,dword ptr[eax],7fffffffh
    imul eax ,dword ptr[ecx],7fffffffh
    imul eax ,dword ptr[edx],7fffffffh
    imul eax ,dword ptr[ebx],7fffffffh
    imul ecx ,dword ptr[eax],7fffffffh
    imul ecx ,dword ptr[ecx],7fffffffh
    imul ecx ,dword ptr[edx],7fffffffh
    imul ecx ,dword ptr[ebx],7fffffffh
    imul edx ,dword ptr[eax],7fffffffh
    imul edx ,dword ptr[ecx],7fffffffh
    imul edx ,dword ptr[edx],7fffffffh
    imul edx ,dword ptr[ebx],7fffffffh
    imul ebx ,dword ptr[eax],7fffffffh
    imul ebx ,dword ptr[ecx],7fffffffh
    imul ebx ,dword ptr[edx],7fffffffh
    imul ebx ,dword ptr[ebx],7fffffffh
    nop
    mov ax , es
    mov cx , es
    mov dx , es
    mov bx , es
    mov ax , cs
    mov cx , cs
    mov dx , cs
    mov bx , cs

// not allowed in x86
;   mov eax , es
;   mov ecx , es
;   mov edx , es
;   mov ebx , es
;   mov eax , cs
;   mov ecx , cs
;   mov edx , cs
;   mov ebx , cs

    mov byte  ptr ds:[12345678h], al
    mov word  ptr ds:[12345678h], ax
    mov dword ptr ds:[12345678h], eax

  RET
  CMC
  CLC
  STC
  CLI
  STI
  CLD
  STD
  SAHF
  LAHF
  PUSHF
  POPF
#ifdef IS32BIT
  PUSHFD
  POPFD
  PUSHAD
  POPAD
#else // IS64BIT
  PUSHFQ
  POPFQ
#endif // IS64BIT
  NOP
  CBW
  CWDE
  CWD
  CDQ

#ifdef IS64BIT
  CDQE
  CQO
  CLGI
  STGI
#endif // IS64BIT
  }
End:
  const BYTE *endIP = getIP();
  const size_t codeSize = endIP - startIP;
  ExecutableByteArray code;
  code.append(startIP, codeSize);
}
#endif // IS64BIT
#endif // TEST_MACHINECODE
#endif // _DEBUG

void callAssemblerCode() {
#ifdef _DEBUG
#ifdef TEST_MACHINECODE
  assemblerCode();
#endif // TEST_MACHINECODE
#endif // _DEBUG
}
