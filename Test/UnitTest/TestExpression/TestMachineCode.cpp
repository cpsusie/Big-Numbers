#include "stdafx.h"
#include <Math/Expression/NewOpcode.h>

#define TEST_MACHINECODE

#ifdef TEST_MACHINECODE

#define TEST_ALLGPREGISTERS

static const GPRegister r8List[] = {
    AL
#ifndef TEST_ALLGPREGISTERS
   ,AH  ,CH
#else
   ,CL  ,DL  ,BL  ,AH  ,CH  ,DH  ,BH
#endif // TEST_ALLGPREGISTERS
#ifdef IS64BIT
   ,R8B
#ifdef TEST_ALLGPREGISTERS
   ,R9B  ,R10B ,R11B ,R12B ,R13B ,R14B ,R15B
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
  };

  const GPRegister r16List[] = {
    AX
#ifndef TEST_ALLGPREGISTERS
   ,SP   ,BP
#else
   ,CX   ,DX   ,BX   ,SP   ,BP   ,SI  ,DI
#endif // TEST_ALLGPREGISTERS

#ifdef IS64BIT
   ,R8W
#ifdef TEST_ALLGPREGISTERS
   ,R9W  ,R10W ,R11W ,R12W ,R13W ,R14W ,R15W
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
  };

  const GPRegister r32List[] = {
    EAX
#ifndef TEST_ALLGPREGISTERS
   ,ESP  ,EBP
#else
   ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLGPREGISTERS

#ifdef IS64BIT
   ,R8D
#ifdef TEST_ALLGPREGISTERS
   ,R9D  ,R10D ,R11D ,R12D ,R13D ,R14D ,R15D
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
  };

#ifdef IS32BIT
  const IndexRegister indexRegList[] = {
    EAX
#ifndef TEST_ALLGPREGISTERS
   ,ESP  ,EBP
#else
   ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLGPREGISTERS

#ifdef IS64BIT
   ,R8D
#ifdef TEST_ALLGPREGISTERS
   ,R9D  ,R10D ,R11D ,R12D ,R13D ,R14D ,R15D
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
  };

#else // IS64BIT

  const GPRegister r64List[] = {
    RAX
#ifndef TEST_ALLGPREGISTERS
   ,RSP  ,RBP
#else
   ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
#endif // TEST_ALLGPREGISTERS
   ,R8
#ifdef TEST_ALLGPREGISTERS
   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLGPREGISTERS
  };

  const IndexRegister indexRegList[] = {
    RAX
#ifndef TEST_ALLGPREGISTERS
   ,RSP  ,RBP
#else
   ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
#endif // TEST_ALLGPREGISTERS
   ,R8
#ifdef TEST_ALLGPREGISTERS
   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLGPREGISTERS
  };

#endif // IS64BIT

  const GPRegister *GPRegListArray[] = {
    r8List, r16List, r32List
#ifdef IS64BIT
   ,r64List
#endif
  };

#define GPREGISTER_COUNT    ARRAYSIZE(r32List)
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

class AllGPRegisters : public CompactArray<const GPRegister*> {
public:
  AllGPRegisters();
};

AllGPRegisters::AllGPRegisters() {
  for(int i = 0; i < ARRAYSIZE(GPRegListArray); i++) {
    const GPRegister *gpreg = GPRegListArray[i];
    for(int j = 0; j < GPREGISTER_COUNT; j++) {
      add(&gpreg[j]);
    }
  }
}

static const int allImmValues[] = {    0x7f, 0x7fff, 0x7fffffff     };

class InstructionOperandArray : public CompactArray<const InstructionOperand*> {
public:
  virtual ~InstructionOperandArray();
};

InstructionOperandArray::~InstructionOperandArray() {
  for(size_t i = size(); i--;) delete (*this)[i];
  clear();
}

class AllImmOperands : public InstructionOperandArray {
public:
  AllImmOperands();
};

AllImmOperands::AllImmOperands() {
  for(int i = 0; i < ARRAYSIZE(allImmValues); i++) {
    add(new InstructionOperand(allImmValues[i]));
  }
}

static const int   allImmAddr[]   = { 0, 0x7fffffff };
static const int   allOffset[]    = { 0, 0x7f, 0x7fffffff, -1 };

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

static int memRefCmp(const MemoryRef &mr1, const MemoryRef &mr2) {
  int c;
  if(c = boolCmp(mr2.isImmediateAddr(),mr1.isImmediateAddr())) return c;
  if(!mr1.isImmediateAddr()) {
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
  if(c = memRefCmp(*o1->getMemoryReference(), *o2->getMemoryReference())) return c;
  if(c = boolCmp(o1->hasSegmentRegister(),o2->hasSegmentRegister())) return c;
  if(!o1->hasSegmentRegister()) return 0; // && !o2->hasSegmentRegister()
  return registerCmp(o1->getSegmentRegister(), o2->getSegmentRegister());
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
        add(new BYTEPtr( factor*inxReg + allOffset[i]));
        add(new WORDPtr( factor*inxReg + allOffset[i]));
        add(new DWORDPtr(factor*inxReg + allOffset[i]));
        add(new QWORDPtr(factor*inxReg + allOffset[i]));
      }
    }
  }
  for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
    for(int j = 0; j < INDEXREGISTER_COUNT; j++) {
      const IndexRegister &baseReg = indexRegList[j];
      add(new BYTEPtr( baseReg + allOffset[i]));
      add(new WORDPtr( baseReg + allOffset[i]));
      add(new DWORDPtr(baseReg + allOffset[i]));
      add(new QWORDPtr(baseReg + allOffset[i]));
      for(int k = 0; k < INDEXREGISTER_COUNT; k++) {
        const IndexRegister &inxReg = indexRegList[k];
        if(!inxReg.isValidIndexRegister()) continue;
        for(int factor = 1; factor <= 8; factor *= 2) {
          add(new BYTEPtr( baseReg + factor*inxReg + allOffset[i]));
          add(new WORDPtr( baseReg + factor*inxReg + allOffset[i]));
          add(new DWORDPtr(baseReg + factor*inxReg + allOffset[i]));
          add(new QWORDPtr(baseReg + factor*inxReg + allOffset[i]));
        }
      }
    }
  }
  sort(memOpCmp);

  redirectDebugLog();
  for(size_t i = 0; i < size(); i++) {
    debugLog(_T("%s\n"),(*this)[i]->toString().cstr());
  }

}

class CodeArray : public ExecutableByteArray {
private:
  DECLARECLASSNAME;
public:
  int        addBytes(const void *bytes, int count);
  inline int emit(const InstructionBase &ins) {
    return addBytes(ins.getBytes(), ins.size());
  }
};

int CodeArray::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  append((BYTE*)bytes,count);
  return ret;
}

class TestMachineCode : public CodeArray {
private:
  AllMemoryOperands m_allMemOperands;
  AllGPRegisters    m_allGPReg;
  AllImmOperands    m_allImmOperands;
  String            m_currentName;
  void clear();
  int emit(                const Instruction0Arg &ins   );
  int emit(                const Opcode1Arg      &opcode, const InstructionOperand &arg);
  int emit(                const Opcode2Arg      &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2);
  void testInstruction0Arg(const Instruction0Arg &ins   );
  void testOpcode1Arg(     const Opcode1Arg      &opcode);
  void testOpcode2Arg(     const Opcode2Arg      &opcode);
public:
  void testOpcode(const OpcodeBase      &opcode, const String &name);
  void testOpcode(const Instruction0Arg &ins   , const String &name);
  TestMachineCode();
};

void TestMachineCode::clear() {
  __super::clear();
  redirectDebugLog();
}

int TestMachineCode::emit(const Instruction0Arg &ins) {
  debugLog(_T("%-26s %s\n"), ins.toString().cstr(), m_currentName.cstr());
  return __super::emit(ins);
}

int TestMachineCode::emit(const Opcode1Arg &opcode, const InstructionOperand &arg) {
  const InstructionBase ins = opcode(arg);
  debugLog(_T("%-26s %s %s\n"), ins.toString().cstr(), m_currentName.cstr(), arg.toString().cstr());
  return __super::emit(ins);
}
int TestMachineCode::emit(const Opcode2Arg &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
  const InstructionBase ins = opcode(arg1,arg2);
  debugLog(_T("%-26s %s %s, %s\n"), ins.toString().cstr(), m_currentName.cstr(), arg1.toString().cstr(), arg2.toString().cstr());
  return __super::emit(ins);
}

#define TESTOPCODE(opcode) testOpcode(opcode, _T(#opcode))
void TestMachineCode::testOpcode(const Instruction0Arg &ins, const String &name) {
  m_currentName = toLowerCase(name);
  emit(ins);
}
void TestMachineCode::testOpcode(const OpcodeBase &opcode, const String &name) {
  clear();
  m_currentName = toLowerCase(name);
  const OpcodeType type = opcode.getType();
  switch(type) {
  case OPCODE1ARG: testOpcode1Arg((Opcode1Arg&)opcode); break;
  case OPCODE2ARG:
  case OPCODEMOV : testOpcode2Arg((Opcode2Arg&)opcode); break;
  default        : throwInvalidArgumentException(__TFUNCTION__,_T("type=%d, name=%s"), type, name.cstr());
  }
}

void TestMachineCode::testInstruction0Arg(  const Instruction0Arg &ins) {
}

void TestMachineCode::testOpcode1Arg(const Opcode1Arg &opcode) {
  if(opcode.isRegisterAllowed()) {
    for(Iterator<const GPRegister*> regIt = m_allGPReg.getIterator(); regIt.hasNext();) {
      const GPRegister &reg = *regIt.next();
      if(opcode.isRegisterAllowed(reg)) {
        emit(opcode,reg);
      }
    }
  }
//  clear();
  if(opcode.isMemoryOperandAllowed()) {
    for(Iterator<const InstructionOperand*> memIt = m_allMemOperands.getIterator(); memIt.hasNext();) {
      const InstructionOperand &operand = *memIt.next();
      if(opcode.isMemoryOperandAllowed((MemoryOperand&)operand)) {
        emit(opcode,operand);
      }
    }
  }
}

void TestMachineCode::testOpcode2Arg(const Opcode2Arg &opcode) {
  if(opcode.isRegisterAllowed()) {
    for(Iterator<const GPRegister*> regDstIt = m_allGPReg.getIterator(); regDstIt.hasNext();) {
      const GPRegister &regDst = *regDstIt.next();
      if(!opcode.isRegisterAllowed(regDst)) {
        continue;
      }
      for(Iterator<const GPRegister*> regSrcIt = m_allGPReg.getIterator(); regSrcIt.hasNext();) {
        const GPRegister &regSrc = *regSrcIt.next();
        if(regSrc.getSize() != regDst.getSize()) {
          continue;
        }
        emit(opcode,regDst,regSrc);
      }
    }
  }
//  clear();
  if(opcode.isMemoryOperandAllowed()) {
    for(int i = 0; i < 2; i++) { // i=0:mem<-reg, 1:reg<-mem
      for(Iterator<const InstructionOperand*> memIt = m_allMemOperands.getIterator(); memIt.hasNext();) {
        const InstructionOperand &memOp = *memIt.next();
        if(opcode.isMemoryOperandAllowed((MemoryOperand&)memOp)) {
          for(Iterator<const GPRegister*> regIt = m_allGPReg.getIterator(); regIt.hasNext();) {
            const GPRegister &regOp = *regIt.next();
            if(regOp.getSize() != memOp.getSize()) {
              continue;
            }
            if(i==0) {
              emit(opcode,memOp,regOp);
            } else {
              emit(opcode,regOp,memOp);
            }
          }
        }
      }
    }
  }
  if(opcode.isImmediateValueAllowed()) {
    for(Iterator<const InstructionOperand*> immIt = m_allImmOperands.getIterator(); immIt.hasNext();) {
      const InstructionOperand &immOp = *immIt.next();
      for(Iterator<const GPRegister*> regDstIt = m_allGPReg.getIterator(); regDstIt.hasNext();) {
        const GPRegister &regOp = *regDstIt.next();
        if(!opcode.isValidOperandCombination(regOp, immOp)) {
          continue;
        }
        emit(opcode,regOp,immOp);
      }
      if(opcode.isMemoryOperandAllowed()) {
        for(Iterator<const InstructionOperand*> memIt = m_allMemOperands.getIterator(); memIt.hasNext();) {
          const InstructionOperand &memOp = *memIt.next();
          if(!opcode.isValidOperandCombination(memOp, immOp)) {
            continue;
          }
          emit(opcode,memOp,immOp);
        }
      }
    }
  }
}

TestMachineCode::TestMachineCode() {
  TESTOPCODE(RET    );
  TESTOPCODE(CMC    );
  TESTOPCODE(CLC    );
  TESTOPCODE(STC    );
  TESTOPCODE(CLI    );
  TESTOPCODE(STI    );
  TESTOPCODE(CLD    );
  TESTOPCODE(STD    );
  TESTOPCODE(PUSHF  );
  TESTOPCODE(POPF   );
  TESTOPCODE(SAHF   );
  TESTOPCODE(LAHF   );
  TESTOPCODE(PUSHAD );
  TESTOPCODE(POPAD  );
  TESTOPCODE(NOOP   );

  TESTOPCODE(SETE   );
  TESTOPCODE(MOV    );
  TESTOPCODE(ADD    );
  TESTOPCODE(ADC    );
  TESTOPCODE(XOR    );
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
//    mov startIP, es
    jmp         End


    cbw
    cwde
    cwd
    cdq
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
