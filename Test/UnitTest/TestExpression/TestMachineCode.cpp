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

static const int   allOffset[]    = { 0, 0x7f, 0x7fff, 0x7fffffff, -1 };

class AllMemoryOperands : public InstructionOperandArray {
public:
  AllMemoryOperands();
};

AllMemoryOperands::AllMemoryOperands() {
  for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
    add(new BYTEPtr( allOffset[i]));
    add(new WORDPtr( allOffset[i]));
    add(new DWORDPtr(allOffset[i]));
    add(new QWORDPtr(allOffset[i]));
  }
  for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
    for(int j = 0; j < INDEXREGISTER_COUNT; j++) {
      add(new BYTEPtr( indexRegList[j] + allOffset[i]));
      add(new WORDPtr( indexRegList[j] + allOffset[i]));
      add(new DWORDPtr(indexRegList[j] + allOffset[i]));
      add(new QWORDPtr(indexRegList[j] + allOffset[i]));
    }
  }
  for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
    for(int j = 0; j < INDEXREGISTER_COUNT; j++) {
      const IndexRegister &baseReg = indexRegList[j];
      for(int k = 0; k < INDEXREGISTER_COUNT; k++) {
        const IndexRegister &addReg = indexRegList[k];
        if(!addReg.isValidAddRegister()) continue;
        for(int factor = 1; factor <= 8; factor *= 2) {
          add(new BYTEPtr( baseReg + factor*addReg + allOffset[i]));
          add(new WORDPtr( baseReg + factor*addReg + allOffset[i]));
          add(new DWORDPtr(baseReg + factor*addReg + allOffset[i]));
          add(new QWORDPtr(baseReg + factor*addReg + allOffset[i]));
        }
      }
    }
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
  void testOpcodeNoArg(  const OpcodeBase    &opcode);
  void testOpcodeStd1Arg(const OpcodeStd1Arg &opcode);
  void testOpcodeStd2Arg(const OpcodeStd2Arg &opcode);
public:
  void testOpcode(const OpcodeBase &opcode);
  TestMachineCode();
};

void TestMachineCode::testOpcode(const OpcodeBase &opcode) {
  clear();
  const OpcodeType type = opcode.getType();
  switch(type) {
  case OPCODENOARG  : testOpcodeNoArg(                  opcode); break;
  case OPCODESTD1ARG: testOpcodeStd1Arg((OpcodeStd1Arg&)opcode); break;
  case OPCODESTD2ARG: testOpcodeStd2Arg((OpcodeStd2Arg&)opcode); break;
  default:return;
  }
}

void TestMachineCode::testOpcodeNoArg(  const OpcodeBase    &opcode) {
}

void TestMachineCode::testOpcodeStd1Arg(const OpcodeStd1Arg &opcode) {
  if(opcode.isRegisterAllowed()) {
    for(Iterator<const GPRegister*> regIt = m_allGPReg.getIterator(); regIt.hasNext();) {
      const GPRegister &reg = *regIt.next();
      if(opcode.isOperandSizeAllowed(reg.getSize())) {
        emit(opcode(reg));
      }
    }
  }
  clear();
  if(opcode.isMemoryReferenceAllowed()) {
    for(Iterator<const InstructionOperand*> memIt = m_allMemOperands.getIterator(); memIt.hasNext();) {
      const InstructionOperand &operand = *memIt.next();
      if(opcode.isOperandSizeAllowed(operand.getSize())) {
        emit(opcode(operand));
      }
    }
  }
}

void TestMachineCode::testOpcodeStd2Arg(const OpcodeStd2Arg &opcode) {
  if(opcode.isRegisterAllowed()) {
    for(Iterator<const GPRegister*> regDstIt = m_allGPReg.getIterator(); regDstIt.hasNext();) {
      const GPRegister &regDst = *regDstIt.next();
      if(!opcode.isOperandSizeAllowed(regDst.getSize())) {
        continue;
      }
      for(Iterator<const GPRegister*> regSrcIt = m_allGPReg.getIterator(); regSrcIt.hasNext();) {
        const GPRegister &regSrc = *regSrcIt.next();
        if(regSrc.getSize() != regDst.getSize()) {
          continue;
        }
        emit(opcode(regDst,regSrc));
      }
    }
  }
  clear();
  if(opcode.isMemoryReferenceAllowed()) {
    for(int i = 0; i < 2; i++) { // i=0:mem<-reg, 1:reg<-mem
      for(Iterator<const InstructionOperand*> memIt = m_allMemOperands.getIterator(); memIt.hasNext();) {
        const InstructionOperand &memOp = *memIt.next();
        if(opcode.isOperandSizeAllowed(memOp.getSize())) {
          for(Iterator<const GPRegister*> regIt = m_allGPReg.getIterator(); regIt.hasNext();) {
            const GPRegister &regOp = *regIt.next();
            if(regOp.getSize() != memOp.getSize()) {
              continue;
            }
            if(i==0) {
              emit(opcode(memOp,regOp));
            } else {
              emit(opcode(regOp,memOp));
            }
          }
        }
      }
    }
  }
  clear();
  if(opcode.isImmediateValueAllowed()) {
    for(Iterator<const InstructionOperand*> immIt = m_allImmOperands.getIterator(); immIt.hasNext();) {
      const InstructionOperand &immOp = *immIt.next();
      for(Iterator<const GPRegister*> regDstIt = m_allGPReg.getIterator(); regDstIt.hasNext();) {
        const GPRegister &regOp = *regDstIt.next();
        if(!opcode.isOperandSizeAllowed(regOp.getSize())) {
          continue;
        }
        if(!opcode.isValidOperandCombination(regOp, immOp)) {
          continue;
        }
        emit(opcode(regOp,immOp));
      }
      if(opcode.isMemoryReferenceAllowed()) {
        for(Iterator<const InstructionOperand*> memIt = m_allMemOperands.getIterator(); memIt.hasNext();) {
          const InstructionOperand &memOp = *memIt.next();
          if(!opcode.isValidOperandCombination(memOp, immOp)) {
            continue;
          }
          emit(opcode(memOp,immOp));
        }
      }
    }
  }
}

TestMachineCode::TestMachineCode() {
  testOpcode(SETE);
  testOpcode(ADD);
  testOpcode(ADC);
  testOpcode(XOR);
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
void assemblerCode() {
  __asm {
    jmp         End

    add al              , 0x7f
    add eax             , 0x7f
    add eax             , 0x7fffffff
    add ax              , 0x7f
    add ax              , 0x7fff
    add cl              , 0x7f
    add ecx             , 0x7f
    add ecx             , 0x7fffffff
    add cx              , 0x7f
    add cx              , 0x7fff
    add al              , cl
    add al              , dl
    add al              , bl
    add al              , ah
    add al              , ch
    add al              , dh
    add al              , bh
    add al              , byte  ptr [REG1]
    add eax             , ecx
    add eax             , edx
    add eax             , ebx
    add eax             , esi
    add eax             , edi
    add eax             , dword ptr [REG1]
    add cl              , al
    add dl              , al
    add bl              , al
    add ah              , al
    add ch              , al
    add dh              , al
    add bh              , al
    add byte  ptr [REG1], al
    add ecx             , eax
    add edx             , eax
    add ebx             , eax
    add esi             , eax
    add edi             , eax
    add dword ptr [REG1], eax
    add word  ptr [REG1], ax
    add ax              , word  ptr [REG1]

    add ah              , cl
    add ch              , cl
    add dh              , cl
    add bh              , cl
    add byte  ptr [REG1], cl
    add byte  ptr [REG1], dl
    add byte  ptr [REG1], bl
    add byte  ptr [REG1], ah
    add byte  ptr [REG1], ch

    add dword ptr [REG1], ecx
    add dword ptr [REG1], edx
    add dword ptr [REG1], ebx
    add dword ptr [REG1], esp
    add dword ptr [REG1], ebp

    add cl                      , byte  ptr [REG1]
    add dl                      , byte  ptr [REG1]
    add bl                      , byte  ptr [REG1]
    add ah                      , byte  ptr [REG1]
    add ch                      , byte  ptr [REG1]

    add ecx                     , dword ptr [REG1]
    add edx                     , dword ptr [REG1]
    add ebx                     , dword ptr [REG1]
    add esp                     , dword ptr [REG1]
    add ebp                     , dword ptr [REG1]

    add word  ptr [REG1], cx
    add word  ptr [REG1], dx
    add word  ptr [REG1], bx
    add word  ptr [REG1], sp
    add word  ptr [REG1], bp

    add cx                      , word  ptr [REG1]
    add dx                      , word  ptr [REG1]
    add bx                      , word  ptr [REG1]
    add sp                      , word  ptr [REG1]
    add bp                      , word  ptr [REG1]

    sete al
    sete cl
    sete dl
    sete es:byte ptr[eax]
    sete ss:byte ptr[eax]
    sete cs:byte ptr[eax]
    sete ds:byte ptr[eax]
    sete fs:byte ptr[eax]
    sete gs:byte ptr[eax]
    sete byte ptr[eax]
    sete byte ptr[eax + 127]
    sete byte ptr[eax - 128]
    sete byte ptr[eax + 0xffff]
    sete byte ptr[eax - 0xffff]
    sete byte ptr[eax + esi]
    sete byte ptr[eax + esi]
    sete byte ptr[eax + esi + 127]
    sete byte ptr[eax + esi - 128]
    sete byte ptr[eax + esi + 0xffff]
    sete byte ptr[eax + esi - 0xffff]
    sete byte ptr[esi+8*edi]
    sete byte ptr[esi+8*edi]
    sete byte ptr[esi+8*edi+127]
    sete byte ptr[esi+8*edi-128]
    sete byte ptr[esi+8*edi+0xffff]
    sete byte ptr[esi+8*edi-0xffff]
    sete byte ptr[esi+0xff]
    sete byte ptr es:[12345678h]
    sete byte ptr cs:[12345678h]
    sete byte ptr ss:[12345678h]
    _emit 0x0F
    _emit 0x94;
    _emit 0x05;
    _emit 0x78;
    _emit 0x56;
    _emit 0x34;
    _emit 0x12;
    sete byte ptr ds:[12345678h]
    sete byte ptr fs:[12345678h]
    sete byte ptr gs:[12345678h]

    mov al                      , byte  ptr es:[12345678h]
    mov eax                     , dword ptr es:[12345678h]
    mov byte  ptr es:[12345678h], al
    mov dword ptr es:[12345678h], eax
    mov ax                      , word  ptr es:[12345678h]
    mov word  ptr es:[12345678h], ax

    mov byte  ptr es:[12345678h], cl
    mov byte  ptr es:[12345678h], dl
    mov byte  ptr es:[12345678h], bl
    mov byte  ptr es:[12345678h], ah
    mov byte  ptr es:[12345678h], ch

    mov dword ptr es:[12345678h], ecx
    mov dword ptr es:[12345678h], edx
    mov dword ptr es:[12345678h], ebx
    mov dword ptr es:[12345678h], esp
    mov dword ptr es:[12345678h], ebp

    mov cl                      , byte  ptr es:[12345678h]
    mov dl                      , byte  ptr es:[12345678h]
    mov bl                      , byte  ptr es:[12345678h]
    mov ah                      , byte  ptr es:[12345678h]
    mov ch                      , byte  ptr es:[12345678h]

    mov ecx                     , dword ptr es:[12345678h]
    mov edx                     , dword ptr es:[12345678h]
    mov ebx                     , dword ptr es:[12345678h]
    mov esp                     , dword ptr es:[12345678h]
    mov ebp                     , dword ptr es:[12345678h]

    mov word  ptr es:[12345678h], cx
    mov word  ptr es:[12345678h], dx
    mov word  ptr es:[12345678h], bx
    mov word  ptr es:[12345678h], sp
    mov word  ptr es:[12345678h], bp

    mov cx                      , word  ptr es:[12345678h]
    mov dx                      , word  ptr es:[12345678h]
    mov bx                      , word  ptr es:[12345678h]
    mov sp                      , word  ptr es:[12345678h]
    mov bp                      , word  ptr es:[12345678h]




    mov al                      , byte  ptr cs:[12345678h]
    mov eax                     , dword ptr cs:[12345678h]
    mov byte  ptr cs:[12345678h], al
    mov dword ptr cs:[12345678h], eax
    mov ax                      , word  ptr cs:[12345678h]
    mov word  ptr cs:[12345678h], ax

    mov byte  ptr cs:[12345678h], cl
    mov byte  ptr cs:[12345678h], dl
    mov byte  ptr cs:[12345678h], bl
    mov byte  ptr cs:[12345678h], ah
    mov byte  ptr cs:[12345678h], ch

    mov dword ptr cs:[12345678h], ecx
    mov dword ptr cs:[12345678h], edx
    mov dword ptr cs:[12345678h], ebx
    mov dword ptr cs:[12345678h], esp
    mov dword ptr cs:[12345678h], ebp

    mov cl                      , byte  ptr cs:[12345678h]
    mov dl                      , byte  ptr cs:[12345678h]
    mov bl                      , byte  ptr cs:[12345678h]
    mov ah                      , byte  ptr cs:[12345678h]
    mov ch                      , byte  ptr cs:[12345678h]

    mov ecx                     , dword ptr cs:[12345678h]
    mov edx                     , dword ptr cs:[12345678h]
    mov ebx                     , dword ptr cs:[12345678h]
    mov esp                     , dword ptr cs:[12345678h]
    mov ebp                     , dword ptr cs:[12345678h]

    mov word  ptr cs:[12345678h], cx
    mov word  ptr cs:[12345678h], dx
    mov word  ptr cs:[12345678h], bx
    mov word  ptr cs:[12345678h], sp
    mov word  ptr cs:[12345678h], bp

    mov cx                      , word  ptr cs:[12345678h]
    mov dx                      , word  ptr cs:[12345678h]
    mov bx                      , word  ptr cs:[12345678h]
    mov sp                      , word  ptr cs:[12345678h]
    mov bp                      , word  ptr cs:[12345678h]




    mov al                      , byte  ptr ss:[12345678h]
    mov eax                     , dword ptr ss:[12345678h]
    mov byte  ptr ss:[12345678h], al
    mov dword ptr ss:[12345678h], eax
    mov ax                      , word  ptr ss:[12345678h]
    mov word  ptr ss:[12345678h], ax

    mov byte  ptr ss:[12345678h], cl
    mov byte  ptr ss:[12345678h], dl
    mov byte  ptr ss:[12345678h], bl
    mov byte  ptr ss:[12345678h], ah
    mov byte  ptr ss:[12345678h], ch

    mov dword ptr ss:[12345678h], ecx
    mov dword ptr ss:[12345678h], edx
    mov dword ptr ss:[12345678h], ebx
    mov dword ptr ss:[12345678h], esp
    mov dword ptr ss:[12345678h], ebp

    mov cl                      , byte  ptr ss:[12345678h]
    mov dl                      , byte  ptr ss:[12345678h]
    mov bl                      , byte  ptr ss:[12345678h]
    mov ah                      , byte  ptr ss:[12345678h]
    mov ch                      , byte  ptr ss:[12345678h]

    mov ecx                     , dword ptr ss:[12345678h]
    mov edx                     , dword ptr ss:[12345678h]
    mov ebx                     , dword ptr ss:[12345678h]
    mov esp                     , dword ptr ss:[12345678h]
    mov ebp                     , dword ptr ss:[12345678h]

    mov word  ptr ss:[12345678h], cx
    mov word  ptr ss:[12345678h], dx
    mov word  ptr ss:[12345678h], bx
    mov word  ptr ss:[12345678h], sp
    mov word  ptr ss:[12345678h], bp

    mov cx                      , word  ptr ss:[12345678h]
    mov dx                      , word  ptr ss:[12345678h]
    mov bx                      , word  ptr ss:[12345678h]
    mov sp                      , word  ptr ss:[12345678h]
    mov bp                      , word  ptr ss:[12345678h]




    mov al                      , byte  ptr ds:[12345678h]
    mov eax                     , dword ptr ds:[12345678h]
    mov byte  ptr ds:[12345678h], al
    mov dword ptr ds:[12345678h], eax
    mov ax                      , word  ptr ds:[12345678h]
    mov word  ptr ds:[12345678h], ax

    mov byte  ptr ds:[12345678h], cl
    mov byte  ptr ds:[12345678h], dl
    mov byte  ptr ds:[12345678h], bl
    mov byte  ptr ds:[12345678h], ah
    mov byte  ptr ds:[12345678h], ch

    mov dword ptr ds:[12345678h], ecx
    mov dword ptr ds:[12345678h], edx
    mov dword ptr ds:[12345678h], ebx
    mov dword ptr ds:[12345678h], esp
    mov dword ptr ds:[12345678h], ebp

    mov cl                      , byte  ptr ds:[12345678h]
    mov dl                      , byte  ptr ds:[12345678h]
    mov bl                      , byte  ptr ds:[12345678h]
    mov ah                      , byte  ptr ds:[12345678h]
    mov ch                      , byte  ptr ds:[12345678h]

    mov ecx                     , dword ptr ds:[12345678h]
    mov edx                     , dword ptr ds:[12345678h]
    mov ebx                     , dword ptr ds:[12345678h]
    mov esp                     , dword ptr ds:[12345678h]
    mov ebp                     , dword ptr ds:[12345678h]

    mov word  ptr ds:[12345678h], cx
    mov word  ptr ds:[12345678h], dx
    mov word  ptr ds:[12345678h], bx
    mov word  ptr ds:[12345678h], sp
    mov word  ptr ds:[12345678h], bp

    mov cx                      , word  ptr ds:[12345678h]
    mov dx                      , word  ptr ds:[12345678h]
    mov bx                      , word  ptr ds:[12345678h]
    mov sp                      , word  ptr ds:[12345678h]
    mov bp                      , word  ptr ds:[12345678h]




    mov al                      , byte  ptr fs:[12345678h]
    mov eax                     , dword ptr fs:[12345678h]
    mov byte  ptr fs:[12345678h], al
    mov dword ptr fs:[12345678h], eax
    mov ax                      , word  ptr fs:[12345678h]
    mov word  ptr fs:[12345678h], ax

    mov byte  ptr fs:[12345678h], cl
    mov byte  ptr fs:[12345678h], dl
    mov byte  ptr fs:[12345678h], bl
    mov byte  ptr fs:[12345678h], ah
    mov byte  ptr fs:[12345678h], ch

    mov dword ptr fs:[12345678h], ecx
    mov dword ptr fs:[12345678h], edx
    mov dword ptr fs:[12345678h], ebx
    mov dword ptr fs:[12345678h], esp
    mov dword ptr fs:[12345678h], ebp

    mov cl                      , byte  ptr fs:[12345678h]
    mov dl                      , byte  ptr fs:[12345678h]
    mov bl                      , byte  ptr fs:[12345678h]
    mov ah                      , byte  ptr fs:[12345678h]
    mov ch                      , byte  ptr fs:[12345678h]

    mov ecx                     , dword ptr fs:[12345678h]
    mov edx                     , dword ptr fs:[12345678h]
    mov ebx                     , dword ptr fs:[12345678h]
    mov esp                     , dword ptr fs:[12345678h]
    mov ebp                     , dword ptr fs:[12345678h]

    mov word  ptr fs:[12345678h], cx
    mov word  ptr fs:[12345678h], dx
    mov word  ptr fs:[12345678h], bx
    mov word  ptr fs:[12345678h], sp
    mov word  ptr fs:[12345678h], bp

    mov cx                      , word  ptr fs:[12345678h]
    mov dx                      , word  ptr fs:[12345678h]
    mov bx                      , word  ptr fs:[12345678h]
    mov sp                      , word  ptr fs:[12345678h]
    mov bp                      , word  ptr fs:[12345678h]



    mov al                      , byte  ptr gs:[12345678h]
    mov eax                     , dword ptr gs:[12345678h]
    mov byte  ptr gs:[12345678h], al
    mov dword ptr gs:[12345678h], eax
    mov ax                      , word  ptr gs:[12345678h]
    mov word  ptr gs:[12345678h], ax

    mov byte  ptr gs:[12345678h], cl
    mov byte  ptr gs:[12345678h], dl
    mov byte  ptr gs:[12345678h], bl
    mov byte  ptr gs:[12345678h], ah
    mov byte  ptr gs:[12345678h], ch

    mov dword ptr gs:[12345678h], ecx
    mov dword ptr gs:[12345678h], edx
    mov dword ptr gs:[12345678h], ebx
    mov dword ptr gs:[12345678h], esp
    mov dword ptr gs:[12345678h], ebp

    mov cl                      , byte  ptr gs:[12345678h]
    mov dl                      , byte  ptr gs:[12345678h]
    mov bl                      , byte  ptr gs:[12345678h]
    mov ah                      , byte  ptr gs:[12345678h]
    mov ch                      , byte  ptr gs:[12345678h]

    mov ecx                     , dword ptr gs:[12345678h]
    mov edx                     , dword ptr gs:[12345678h]
    mov ebx                     , dword ptr gs:[12345678h]
    mov esp                     , dword ptr gs:[12345678h]
    mov ebp                     , dword ptr gs:[12345678h]

    mov word  ptr gs:[12345678h], cx
    mov word  ptr gs:[12345678h], dx
    mov word  ptr gs:[12345678h], bx
    mov word  ptr gs:[12345678h], sp
    mov word  ptr gs:[12345678h], bp

    mov cx                      , word  ptr gs:[12345678h]
    mov dx                      , word  ptr gs:[12345678h]
    mov bx                      , word  ptr gs:[12345678h]
    mov sp                      , word  ptr gs:[12345678h]
    mov bp                      , word  ptr gs:[12345678h]



    add         byte ptr es:[12345678h],al
    add         byte ptr cs:[12345678h],al
    add         byte ptr ss:[12345678h],al
    add         byte ptr ds:[12345678h],al
    add         byte ptr gs:[12345678h],al
    add         byte ptr fs:[12345678h],al

    add         byte ptr es:[12345678h],cl
    add         byte ptr cs:[12345678h],cl
    add         byte ptr ss:[12345678h],cl
    add         byte ptr ds:[12345678h],cl
    add         byte ptr gs:[12345678h],cl
    add         byte ptr fs:[12345678h],cl

    imul ecx, dword ptr[esi+8*edi+0xabcddbca],0x12345678
    imul cx ,  word ptr[esi+8*edi+0xabcddbca],0x1234

    cbw
    cwde
    cwd
    cdq
  }
End:;
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
