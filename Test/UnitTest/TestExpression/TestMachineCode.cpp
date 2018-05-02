#include "stdafx.h"
#include <HashSet.h>
#include <Date.h>
#include <ExecutableByteArray.h>
#include <Opcode.h>

#define TEST_MACHINECODE

#ifdef TEST_MACHINECODE

// #define TEST_ALLGPREGISTERS  // If defined, all 24 (16*3+4=52 in x64) GPR registers will be used
                                // If not defined, only the following GPR-register will be used
                                // when doing register-addressing, or in combination with RM/MR-opcodes
                                //   AL ,CL ,BH ,SPL ,DIL,R8B,R15B
                                //   AX              ,DI ,R8W,R15W
                                //   EAX             ,EDI,R8D,R15D
                                //   RAX             ,RDI,R8 ,R15
// #define TEST_ALLFPUREGISTERS // If defined, all 8 FPU-registers will be used.
                                // If not defined, only st(0),st(7) will be used
// #define TEST_ALLXMMREGISTERS // If defined, all 8(16) XMM-registers will be used
                                // If not defined, only XMM0,XMM7(+XMM8,XMM15 in x64) will be used
// #define TEST_ALLBASEREGISTER // If defined, all GPR-registers will be used as base-register
                                // in SIB-byte. If not defined, only RAX,R15 will be used
// #define TEST_ALLINXREGISTERS // If defined, all GPR-registers will be used as index-register
                                // in SIB-byte. If not defined, only RAX,RBP,R15 will be used
// #define TEST_ALLSCALEFACTORS // If defined, all four values, 1,2,4,8 of factor in SIB-byte will be used
                                // If not defined, only 1 and 8 will be used

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
#else  // TEST_ALLGPREGISTERS
   ,R8D  ,R9D  ,R10D ,R11D ,R12D ,R13D ,R14D ,R15D
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
};

const IndexRegister indexRegList[] = {
#ifdef IS32BIT
#ifndef TEST_ALLGPREGISTERS
    EAX                    ,ESP  ,EBP        ,EDI
#else // TEST_ALLGPREGISTERS
    EAX  ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLGPREGISTERS
#else // IS64BIT
#ifndef TEST_ALLGPREGISTERS
    RAX                    ,RSP  ,RBP        ,RDI
   ,R8                     ,R12  ,R13        ,R15
#else // TEST_ALLGPREGISTERS
    RAX  ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
   ,R8   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLGPREGISTERS
#endif // IS64BIT
};

const IndexRegister baseRegList[] = {
#ifdef IS32BIT
#ifndef TEST_ALLBASEREGISTERS
    EAX                    ,ESP
#else // TEST_ALLBASEREGISTERS
    EAX  ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLGPREGISTERS
#else // IS64BIT
#ifndef TEST_ALLBASREGISTERS
    RAX                                      ,R15
#else // TEST_ALLBASREGISTERS
    RAX  ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
   ,R8   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLBASEREGISTERS
#endif // IS64BIT
};

const IndexRegister inxRegList[] = {
#ifdef IS32BIT
#ifndef TEST_ALLINXREGISTERS
    EAX                          ,EBP        ,EDI
#else // TEST_ALLINXREGISTERS
    EAX  ,ECX  ,EDX  ,EBX  ,ESP  ,EBP  ,ESI  ,EDI
#endif // TEST_ALLINXREGISTERS
#else // IS64BIT
#ifndef TEST_ALLINXREGISTERS
    RAX                          ,RBP        ,R15
#else // TEST_ALLINXREGISTERS
    RAX  ,RCX  ,RDX  ,RBX  ,RSP  ,RBP  ,RSI  ,RDI
   ,R8   ,R9   ,R10  ,R11  ,R12  ,R13  ,R14  ,R15
#endif // TEST_ALLINXREGISTERS
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
#endif // TEST_ALLGPREGISTERS
};
#endif // IS64BIT

const SegmentRegister segregList[] = {
#ifdef IS32BIT
  ES,CS,SS,DS,FS,GS
#else  // IS64BIT
  FS,GS
#endif // IS64BIT
};

const FPURegister fpuregList[] = {
#ifndef TEST_ALLFPUREGISTERS
  ST0,                               ST7
#else  // TEST_ALLFPUREGISTERS
  ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7
#endif // TEST_ALLFPUREGISTERS
};

const XMMRegister XMMregList[] = {
#ifndef TEST_ALLXMMREGISTERS
  XMM0,                                   XMM7
#ifdef IS64BIT
 ,XMM8,                                   XMM15
#endif  // IS64BIT
#else   // TEST_ALLXMMREGISTERS
  XMM0,XMM1,XMM2 ,XMM3 ,XMM4 ,XMM5 ,XMM6 ,XMM7
#ifdef IS64BIT
 ,XMM8,XMM9,XMM10,XMM11,XMM12,XMM13,XMM14,XMM15
#endif  // IS64BIT
#endif  // TEST_ALLXMMREGISTERS
};

#define INDEXREGISTER_COUNT ARRAYSIZE(indexRegList)
#define BASEREGISTER_COUNT  ARRAYSIZE(baseRegList )
#define INXREGISTER_COUNT   ARRAYSIZE(inxRegList  )

#define UNKNOWN_OPCODE(  dst)               B2INSA(0x8700 + ((dst)<<3))                     // Build src with MEM_ADDR-macros, REGREG
//Real FMOD(Real x, Real y) {
//  return fmod(x,y);
//}

class InstructionOperandArray : public CompactArray<const InstructionOperand*> {
public:
  virtual ~InstructionOperandArray();
  String toString() const;
};

InstructionOperandArray::~InstructionOperandArray() {
  for(size_t i = size(); i--;) {
    delete (*this)[i];
  }
  clear();
}

String InstructionOperandArray::toString() const {
  String result;
  for(size_t i = 0; i < size(); i++) {
    const InstructionOperand &e = *(*this)[i];
    result += format(_T("%-40s %s\n"), e.toString().cstr(), ::toString(e.getSize()).cstr());
  }
  return result;
}

class AllRegisters : public InstructionOperandArray {
private:
  template<class REG> void addRegArray(const REG *list, size_t n) {
    for(size_t i = 0; i < n; i++) {
      add(new InstructionOperand(list[i]));
    }
  }
public:
  AllRegisters();
};

AllRegisters::AllRegisters() {
  addRegArray(segregList, ARRAYSIZE(segregList));
  addRegArray(r8List    , ARRAYSIZE(r8List    ));
  addRegArray(r16List   , ARRAYSIZE(r16List   ));
  addRegArray(r32List   , ARRAYSIZE(r32List   ));
#ifdef IS64BIT
  addRegArray(r64List   , ARRAYSIZE(r64List   ));
#endif
  addRegArray(fpuregList, ARRAYSIZE(fpuregList));
  addRegArray(XMMregList, ARRAYSIZE(XMMregList));
}

#ifdef IS32BIT
static const int   allImmValues[] = {    0x7f, 0x7fff, 0x7fffffff, -1 };
#else // IS64BIT
static const INT64 allImmValues[] = {    0x7f, 0x7fff, 0x7fffffff, 0x7fffffffffffffffi64, -1 };
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

static const int    allOffset[]    = { 0, 0x7fffffff, -1 };

class AllMemoryOperands : public InstructionOperandArray {
private:
  void addAllMemPtrTypes(const MemoryRef &mr);
public:
  AllMemoryOperands();
};

void AllMemoryOperands::addAllMemPtrTypes(const MemoryRef &mr) {
  add(new BYTEPtr(   mr));
  add(new WORDPtr(   mr));
  add(new DWORDPtr(  mr));
  add(new QWORDPtr(  mr));
  add(new TBYTEPtr(  mr));
  add(new MMWORDPtr( mr));
  add(new XMMWORDPtr(mr));
}

static inline int insOpCmp(const InstructionOperand * const &op1, const InstructionOperand * const &op2) {
  return InstructionOperand::insOpCmp(*op1,*op2);
}

#ifdef TEST_ALLSCALEFACTORS
#define SCALE_FACTORSTEP 2
#else  // TEST_ALLSCALEFACTORS
#define SCALE_FACTORSTEP 8
#endif // TEST_ALLSCALEFACTORS

AllMemoryOperands::AllMemoryOperands() {
  for(int i = 0; i < ARRAYSIZE(allImmAddr); i++) {
    addAllMemPtrTypes(allImmAddr[i]);
  }
  for(int i = 0; i < INXREGISTER_COUNT; i++) {
    const IndexRegister &inxReg = inxRegList[i];
    if(!inxReg.isValidIndexRegister()) continue;
    for(int factor = 1; factor <= 8; factor *= SCALE_FACTORSTEP) {
      for(int i = 0; i < ARRAYSIZE(allOffset); i++) {
        const int offset = allOffset[i];
        addAllMemPtrTypes(factor*inxReg + offset);
      }
    }
  }
  for(int f = 0; f < ARRAYSIZE(allOffset); f++) {
    const int offset = allOffset[f];
    for(int b = 0; b < BASEREGISTER_COUNT; b++) {
      const IndexRegister &baseReg = baseRegList[b];
      addAllMemPtrTypes(baseReg + offset);
      for(int i = 0; i < INXREGISTER_COUNT; i++) {
        const IndexRegister &inxReg = inxRegList[i];
        if(!inxReg.isValidIndexRegister()) continue;
        for(int factor = 1; factor <= 8; factor *= SCALE_FACTORSTEP) {
          addAllMemPtrTypes(baseReg + factor*inxReg + offset);
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
  sort(insOpCmp);
  return *this;
}

class AllStringInstructions : public CompactArray<const StringInstruction*> {
public:
  AllStringInstructions();
};

AllStringInstructions::AllStringInstructions() {
  add(&MOVSB);  add(&CMPSB);  add(&STOSB);  add(&LODSB);  add(&SCASB);
  add(&MOVSW);  add(&CMPSW);  add(&STOSW);  add(&LODSW);  add(&SCASW);
  add(&MOVSD);  add(&CMPSD);  add(&STOSD);  add(&LODSD);  add(&SCASD);
#ifdef IS64BIT
  add(&MOVSQ);  add(&CMPSQ);  add(&STOSQ);  add(&LODSQ);  add(&SCASQ);
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
  CodeArray() {
    setCapacity(0x1000000);
  }
  inline int emit(const InstructionBase &ins) {
    if((s_emitCount++ & 0x3ff) == 0) checkKeyboard();
    return addBytes(ins.getBytes(), ins.size());
  }
  void clear() {
    s_emitCount = 0;
    if(size() > 0) {
      memset((BYTE*)getData(),0,size());
    }
    __super::clear(-1);
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
  const vprintFunction    m_vpf;
  InstructionOperandArray m_allOperands;
  AllVOIDPtrOperands      m_allVOIDPtrOperands;
  AllStringInstructions   m_allStringInstructions;
  String                  m_currentMnemonic;
  int                     m_emitCount;
  FILE                   *m_emitCountLog;
  bool                    m_debugLogOn;
  bool                    m_clearOn;
  void initAllOperands();
  void clear(bool force = false);
  inline void redirectDebugLog() {
    if(!m_debugLogOn) return;
    ::redirectDebugLog();
  }
  void debugLog(const TCHAR *format,...);
  int  emit(               const InstructionBase &ins   );
  int  emit(               const OpcodeBase      &opcode, const InstructionOperand &op);
  int  emit(               const OpcodeBase      &opcode, const InstructionOperand &op1, const InstructionOperand &op2);
  int  emit(               const OpcodeBase      &opcode, const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3);
  void testOpcode1Arg(     const OpcodeBase      &opcode);
  void testOpcode2Arg(     const OpcodeBase      &opcode, bool selectVOIDPtr);
  void testOpcode3Arg(     const OpcodeBase      &opcode);
  inline bool setDebugLogOn(bool on) {
    const bool old = m_debugLogOn;
    m_debugLogOn = on;
    return old;
  }
  inline bool setClearOn(bool on) {
    const bool old = m_clearOn;
    m_clearOn = on;
    return old;
  }
  inline void newMnemonic(const String &mnemonic) {
    m_currentMnemonic = mnemonic;
    m_emitCount = 0;
  }
  void logEmitCount();
  void printf(const TCHAR *format,...) const;
public:
  TestMachineCode(vprintFunction vpf);
  ~TestMachineCode();
  void testAll();
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
  void testFPUOpcodes();
  void testXMMOpcodes();
};

TestMachineCode::TestMachineCode(vprintFunction vpf) : m_vpf(vpf) {
  setClearOn(true);
  m_debugLogOn = true;
  initAllOperands();
}

TestMachineCode::~TestMachineCode() {
  fclose(m_emitCountLog);
}

void TestMachineCode::initAllOperands() {
  AllRegisters      m_allRegisters;
  AllMemoryOperands m_allMemOperands;
  AllImmOperands    m_allImmOperands;
  m_allVOIDPtrOperands = m_allMemOperands;

  m_allOperands.addAll(m_allRegisters  ); m_allRegisters.clear();
  m_allOperands.addAll(m_allMemOperands); m_allMemOperands.clear();
  m_allOperands.addAll(m_allImmOperands); m_allImmOperands.clear();
  m_allOperands.sort(insOpCmp);

  redirectDebugLog();
//  setDebugLogOn(false);

  debugLog(_T("All operands:\n%s"),m_allOperands.toString().cstr());
  debugLog(_T("All VOIDPtr:\n%s") ,m_allVOIDPtrOperands.toString().cstr());
  redirectDebugLog();
#ifdef IS32BIT
#define NAMEPREFIX _T("x32")
#else // IS64BIT
#define NAMEPREFIX _T("x64")
#endif // IS64BIT
  const String logName = format(_T("c:\\temp\\emitLogs\\%s%s.log")
                               ,NAMEPREFIX
                               ,Timestamp().toString(_T("yyyyMMddhhmmss")).cstr());
  m_emitCountLog = MKFOPEN(logName,_T("w"));
  _ftprintf(m_emitCountLog, _T("Operands:%3zu. void Operands:%3zu\n")
           ,m_allOperands.size()
           ,m_allVOIDPtrOperands.size());
}

void TestMachineCode::testAll() {
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
      case 7 : testFPUOpcodes();          break;
      case 8 : testXMMOpcodes();          break;
      default: i = -1000; break;
      }
    } catch(UserInterrupt u) {
      if(u.getType() != BREAK_GROUP) {
        throw;
      }
    }
  }
}

void TestMachineCode::debugLog(const TCHAR *format, ...) {
  if(!m_debugLogOn) return;
  va_list argptr;
  va_start(argptr,format);
  vdebugLog(format, argptr);
  va_end(argptr);
}

void TestMachineCode::logEmitCount() {
  _ftprintf(m_emitCountLog, _T("%-20s:%4d\n"), m_currentMnemonic.cstr(), m_emitCount);
  fflush(m_emitCountLog);
}

void TestMachineCode::printf(const TCHAR *format, ...) const {
  va_list argptr;
  va_start(argptr,format);
  m_vpf(format,argptr);
  va_end(argptr);
}

void TestMachineCode::clear(bool force) {
  if(force) {
    int breakPointHere = 1;
  }
  if(force || m_clearOn) {
    __super::clear();
//    redirectDebugLog();
  }
}

static String formatIns(const InstructionBase &ins) {
  const char stackDelta = ins.getFPUStackDelta();
  return stackDelta
       ? format(_T("%-36s FPUstDelta:%+2d"),ins.toString().cstr(),stackDelta)
       : format(_T("%-36s              "),ins.toString().cstr());
}

int TestMachineCode::emit(const InstructionBase &ins) {
  debugLog(_T("%s %s\n")
          ,formatIns(ins).cstr(), m_currentMnemonic.cstr());
  m_emitCount++;
  return __super::emit(ins);
}

int TestMachineCode::emit(const OpcodeBase &opcode, const InstructionOperand &op) {
  const InstructionBase ins = opcode(op);
  debugLog(_T("%s %s %s\n")
          ,formatIns(ins).cstr(), m_currentMnemonic.cstr()
          ,op.toString().cstr());
  m_emitCount++;
  return __super::emit(ins);
}

int TestMachineCode::emit(const OpcodeBase &opcode, const InstructionOperand &op1, const InstructionOperand &op2) {
  const InstructionBase ins = opcode(op1,op2);
  debugLog(_T("%s %s %s, %s\n")
          ,formatIns(ins).cstr(), m_currentMnemonic.cstr()
          ,op1.toString().cstr(), op2.toString().cstr());
  m_emitCount++;
  return __super::emit(ins);
}

int TestMachineCode::emit(const OpcodeBase &opcode, const InstructionOperand &op1, const InstructionOperand &op2, const InstructionOperand &op3) {
  const InstructionBase ins = opcode(op1,op2,op3);
  debugLog(_T("%s %s %s,%s,%s\n")
          ,formatIns(ins).cstr(), m_currentMnemonic.cstr()
          ,op1.toString().cstr(), op2.toString().cstr(),op3.toString().cstr());
  m_emitCount++;
  return __super::emit(ins);
}

void TestMachineCode::testOpcode(const OpcodeBase &opcode, bool selectVOIDPtr) {
  try {
    newMnemonic(opcode.getMnemonic());
    printf(_T("Testing opcode %s"), m_currentMnemonic.cstr());
//    clear();
    for(int args = opcode.getOpCount(); args <= opcode.getMaxOpCount(); args++) {
      switch(args) {
      case 0 : emit((Opcode0Arg&)opcode            ); break;
      case 1 : testOpcode1Arg(opcode               ); break;
      case 2 : testOpcode2Arg(opcode, selectVOIDPtr); break;
      case 3 : testOpcode3Arg(opcode               ); break;
      default: throwInvalidArgumentException(__TFUNCTION__,_T("%s.getMaxOpCount()=%d"), opcode.getMnemonic().cstr(), opcode.getMaxOpCount());
      }
    }
    logEmitCount();
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
  newMnemonic(prefix.getMnemonic());
  for(Iterator<const StringInstruction*> it = m_allStringInstructions.getIterator(); it.hasNext();) {
    const StringInstruction &ins = *it.next();
    emit(prefix(ins));
  }
  logEmitCount();
}

void TestMachineCode::testArg0Opcodes() {
  clear();
  setClearOn(false);

  testOpcode(RET     );
  testOpcode(CMC     );
  testOpcode(CLC     );
  testOpcode(STC     );
  testOpcode(CLI     );
  testOpcode(STI     );
  testOpcode(CLD     );
  testOpcode(STD     );
  testOpcode(SAHF    );
  testOpcode(LAHF    );
  testOpcode(PUSHF   );
  testOpcode(POPF    );
#ifdef IS32BIT
  testOpcode(PUSHFD  );
  testOpcode(POPFD   );
  testOpcode(PUSHAD  );
  testOpcode(POPAD   );
#else // IS64BIT
  testOpcode(PUSHFQ  );
  testOpcode(POPFQ   );
#endif // IS64BIT
  testOpcode(NOOP    );
  testOpcode(CBW     );
  testOpcode(CWDE    );
  testOpcode(CWD     );
  testOpcode(CDQ     );

#ifdef IS64BIT
  testOpcode(CDQE    );
  testOpcode(CQO     );
  testOpcode(CLGI    );
  testOpcode(STGI    );
#endif // IS64BIT
}

void TestMachineCode::testArg1Opcodes() {
  testOpcode(PUSH    );
  testOpcode(POP     );
  testOpcode(INC     );
  testOpcode(DEC     );
  testOpcode(NOT     );
  testOpcode(NEG     );
  testOpcode(JMP     );
  clear(true);
#ifdef IS32BIT
  testOpcode(JCXZ    );
  testOpcode(JECXZ   );
#else // IS64BIT
  testOpcode(JECXZ   );
  testOpcode(JRCXZ   );
#endif // IS64BIT

  testOpcode(LOOP    );
  testOpcode(LOOPE   );
  testOpcode(LOOPNE  );

  testOpcode(CALL    );
  testOpcode(IMUL    );
  testOpcode(IDIV    );
}

void TestMachineCode::testArg2Opcodes() {
  clear(true);
  testOpcode(MUL     );
  testOpcode(DIV     );

  testOpcode(ADD     );
  testOpcode(ADC     );
  testOpcode(OR      );
  testOpcode(AND     );
  testOpcode(SUB     );
  testOpcode(SBB     );
  testOpcode(XOR     );
  testOpcode(CMP     );
  testOpcode(XCHG    );
  testOpcode(TEST    );
  testOpcode(MOV     );
  testOpcode(MOVSX   );
  testOpcode(MOVZX   );
  testOpcode(LEA     );
}

void TestMachineCode::testArg3Opcodes() {
  testOpcode(SHLD    );
  testOpcode(SHRD    );
}

void TestMachineCode::testSetccOpcodes() {
  clear(true);
  testOpcode(SETO    );
  testOpcode(SETNO   );
  testOpcode(SETB    );
  testOpcode(SETAE   );
  testOpcode(SETE    );
  testOpcode(SETNE   );
  testOpcode(SETBE   );
  testOpcode(SETA    );
  testOpcode(SETS    );
  testOpcode(SETNS   );
  testOpcode(SETP    );
  testOpcode(SETNP   );
  testOpcode(SETL    );
  testOpcode(SETGE   );
  testOpcode(SETLE   );
  testOpcode(SETG    );

  clear(true);
  testOpcode(CMOVO   );
  testOpcode(CMOVNO  );
  testOpcode(CMOVB   );
  testOpcode(CMOVAE  );
  testOpcode(CMOVE   );
  testOpcode(CMOVNE  );
  testOpcode(CMOVBE  );
  testOpcode(CMOVA   );
  testOpcode(CMOVS   );
  testOpcode(CMOVNS  );
  testOpcode(CMOVP   );
  testOpcode(CMOVNP  );
  testOpcode(CMOVL   );
  testOpcode(CMOVGE  );
  testOpcode(CMOVLE  );
  testOpcode(CMOVG   );

//  clear(true);
  testOpcode(JO      );
  testOpcode(JNO     );
  testOpcode(JB      );
  testOpcode(JAE     );
  testOpcode(JE      );
  testOpcode(JNE     );
  testOpcode(JBE     );
  testOpcode(JA      );
  testOpcode(JS      );
  testOpcode(JNS     );
  testOpcode(JP      );
  testOpcode(JNP     );
  testOpcode(JL      );
  testOpcode(JGE     );
  testOpcode(JLE     );
  testOpcode(JG      );
}

void TestMachineCode::testBitOperations() {
  clear(true);
  testOpcode(ROL     );
  testOpcode(ROR     );
  testOpcode(RCL     );
  testOpcode(RCR     );
  testOpcode(SHL     );
  testOpcode(SHR     );
  testOpcode(SAR     );

  testOpcode(BSF     );
  testOpcode(BSR     );

  testOpcode(BT      );
  testOpcode(BTS     );
  testOpcode(BTR     );
  testOpcode(BTC     );
}

void TestMachineCode::testStringInstructions() {
  clear(true);
  testOpcode(MOVSB   );
  testOpcode(CMPSB   );
  testOpcode(STOSB   );
  testOpcode(LODSB   );
  testOpcode(SCASB   );

  testOpcode(MOVSW   );
  testOpcode(CMPSW   );
  testOpcode(STOSW   );
  testOpcode(LODSW   );
  testOpcode(SCASW   );

  testOpcode(MOVSD   );
  testOpcode(CMPSD   );
  testOpcode(STOSD   );
  testOpcode(LODSD   );
  testOpcode(SCASD   );

#ifdef IS64BIT
  testOpcode(MOVSQ   );
  testOpcode(CMPSQ   );
  testOpcode(STOSQ   );
  testOpcode(LODSQ   );
  testOpcode(SCASQ   );
#endif // IS64BIT

  testOpcode(REP     );
  testOpcode(REPE    );
  testOpcode(REPNE   );
}

void TestMachineCode::testFPUOpcodes() {
  clear(true);

  testOpcode(FNSTSWAX);
  testOpcode(FWAIT   );
  testOpcode(FNOP    );
  testOpcode(FCHS    );
  testOpcode(FABS    );
  testOpcode(FTST    );
  testOpcode(FXAM    );
  testOpcode(FLD1    );
  testOpcode(FLDL2T  );
  testOpcode(FLDL2E  );
  testOpcode(FLDPI   );
  testOpcode(FLDLG2  );
  testOpcode(FLDLN2  );
  testOpcode(FLDZ    );
  testOpcode(F2XM1   );
  testOpcode(FYL2X   );
  testOpcode(FPTAN   );
  testOpcode(FPATAN  );
  testOpcode(FXTRACT );
  testOpcode(FPREM1  );
  testOpcode(FDECSTP );
  testOpcode(FINCSTP );
  testOpcode(FPREM   );
  testOpcode(FYL2XP1 );
  testOpcode(FSQRT   );
  testOpcode(FSINCOS );
  testOpcode(FRNDINT );
  testOpcode(FSCALE  );
  testOpcode(FSIN    );
  testOpcode(FCOS    );

  testOpcode(FLDCW   );
  testOpcode(FNSTCW  );
  testOpcode(FNSTSW  );

  testOpcode(FLD     );
  testOpcode(FSTP    );
  testOpcode(FST     );
  testOpcode(FBLD    );
  testOpcode(FBSTP   );

  testOpcode(FADD    );
  testOpcode(FMUL    );
  testOpcode(FSUB    );
  testOpcode(FDIV    );
  testOpcode(FSUBR   );
  testOpcode(FDIVR   );

  testOpcode(FADDP   );
  testOpcode(FMULP   );
  testOpcode(FSUBP   );
  testOpcode(FDIVP   );
  testOpcode(FSUBRP  );
  testOpcode(FDIVRP  );

  testOpcode(FCOM    );
  testOpcode(FCOMP   );
  testOpcode(FCOMI   );
  testOpcode(FCOMIP  );
  testOpcode(FUCOM   );
  testOpcode(FUCOMP  );
  testOpcode(FUCOMI  );
  testOpcode(FUCOMIP );

  testOpcode(FCOMPP  );
  testOpcode(FUCOMPP );

  testOpcode(FILD    );
  testOpcode(FISTTP  );
  testOpcode(FIST    );
  testOpcode(FISTP   );
  testOpcode(FIADD   );
  testOpcode(FIMUL   );
  testOpcode(FICOM   );
  testOpcode(FICOMP  );
  testOpcode(FISUB   );
  testOpcode(FISUBR  );
  testOpcode(FIDIV   );
  testOpcode(FIDIVR  );

  testOpcode(FCMOVB  );
  testOpcode(FCMOVE  );
  testOpcode(FCMOVBE );
  testOpcode(FCMOVU  );
  testOpcode(FCMOVAE );
  testOpcode(FCMOVNE );
  testOpcode(FCMOVA  );
  testOpcode(FCMOVNU );
  testOpcode(FFREE   );
  testOpcode(FXCH    );
}

void TestMachineCode::testXMMOpcodes() {
  clear(true);
  testOpcode(MOVAPS  );
  testOpcode(MOVSD1  );
  testOpcode(ADDSD   );
  testOpcode(MULSD   );
  testOpcode(SUBSD   );
  testOpcode(DIVSD   );
}

#endif // TEST_MACHINECODE

void generateTestSequence(vprintFunction vpf) {
#ifdef TEST_MACHINECODE
  TestMachineCode test(vpf);
  test.testAll();
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
