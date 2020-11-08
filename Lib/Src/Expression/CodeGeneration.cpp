#include "pch.h"
#include "MachineCode.h"
#include "CodeGeneration.h"

namespace Expr {

#if defined(IS64BIT)
#define LOCALSTACKSPACE   72
#define RESERVESTACKSPACE 40
#endif

CodeGeneration::CodeGeneration(MachineCode *code, const CompactRealArray &valueTable, const StringArray &nameCommentArray, FILE *listFile)
  : m_code(*code)
  , m_addressTable(valueTable)
  , m_listFile(listFile, m_addressTable, nameCommentArray, TABLEREF_REG)
  , m_listEnabled(listFile!=nullptr)
  , m_FPUOptimizeCount(0)
  , m_lastFPUOptimizeCount(0)
{
  m_FPUEmulator.setContainer(this);
#if defined(IS64BIT)
  m_functionTableStart = 0;
  resetStack(RESERVESTACKSPACE);
#endif // IS64BIT

  if(m_listFile.isOpen()) {
    const char offset = m_addressTable.getESITableOffset();
    list(_T(";%s offset from &valueTable[0] (in bytes):%d (%s), &value[%u]\n")
        ,TABLEREF_REG.getName().cstr()
        ,offset
        ,formatHexValue(offset,false).cstr()
        ,m_addressTable.esiOffsetToIndex(0)
    );
  }
}

UINT CodeGeneration::size() const {
  return (UINT)m_code.size();
}

InstructionInfo CodeGeneration::insertIns(UINT pos, const InstructionBase &ins) {
  assert(pos <= size());
  const UINT n = ins.size();
  if(pos < size()) {
    insertZeroes(pos,n);
    m_code.setBytes(pos, ins.getBytes(), n);
  } else {
    m_code.append(ins.getBytes(), n);
  }
  return InstructionInfo(pos,ins);
}

InstructionInfo CodeGeneration::insert(UINT pos, const Opcode0Arg &opcode) {
  if(pos == size()) m_FPUEmulator.execute(opcode);
  const InstructionInfo info = insertIns(pos, opcode);
  if(listEnabled()) m_listFile.add(pos,opcode);
  return info;
}

InstructionInfo CodeGeneration::insert(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg) {
  if(pos == size()) m_FPUEmulator.execute(opcode,arg);
  const InstructionInfo info = insertIns(pos,opcode(arg));
  if(listEnabled()) m_listFile.add(pos,opcode,arg);
  return info;
}

InstructionInfo CodeGeneration::insert(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
  if(pos == size()) m_FPUEmulator.execute(opcode,arg1,arg2);
  const InstructionInfo info = insertIns(pos, opcode(arg1,arg2));
  if(listEnabled()) m_listFile.add(pos, opcode,arg1,arg2);
  return info;
}

InstructionInfo CodeGeneration::insert(UINT pos, const StringPrefix &prefix, const StringInstruction &strins) {
  const InstructionInfo info = insertIns(pos, prefix(strins));
  if(listEnabled()) m_listFile.add(pos, prefix,strins);
  return info;
}

InstructionInfo CodeGeneration::insertLEA(UINT pos, const IndexRegister &dst, const MemoryOperand &mem) {
  const MemoryRef &mr = mem.getMemoryReference();
  if(mr.hasOffset()) {
    return insert(pos, LEA, dst, mem);
  } else {
    return insert(pos, MOV, dst, *mr.getBase());
  }
}

void CodeGeneration::emitFPUOpMem(const OpcodeBase &opcode, const MemoryOperand &mem) {
#if defined(FPU_OPTIMIZE)
  switch(FPUEmulator::codeLookup(opcode)) {
  case _FADD :
  case _FMUL :
  case _FSUB :
  case _FDIV :
  case _FSUBR:
  case _FDIVR:
    { const int stIndex = findFPURegIndex(mem);
      if(stIndex >= 0) {
        m_FPUOptimizeCount++;
        emit(opcode, ST0, ST(stIndex));
        return;
      }
    }
    break;
  case _FLD  :
  case _FSTP :
  case _FCOMP:
    { const int stIndex = findFPURegIndex(mem);
      if(stIndex >= 0) {
        m_FPUOptimizeCount++;
        emit(opcode, ST(stIndex));
        return;
      }
    }
    break;
  }
#endif // FPU_OPTIMIZE
  emit(opcode, mem);
}

int CodeGeneration::findFPURegIndex(const MemoryOperand &mem) const {
  const int valueIndex = getValueIndex(mem);
  return (valueIndex >= 0) ? m_FPUEmulator.findRegisterWithValueIndex(valueIndex) : -1;
}

int CodeGeneration::getValueIndex(const InstructionOperand &op) const {
  if(!op.isMemoryRef()) return -1;
  const MemoryRef &mr = op.getMemoryReference();
  if(mr.hasBase() && (mr.getBase() == &TABLEREF_REG)) {
    const UINT valueIndex = m_addressTable.esiOffsetToIndex(mr.getOffset());
    if(valueIndex < m_addressTable.getValueCount()) {
      return valueIndex;
    }
  }
  return -1;
}

void CodeGeneration::emitLabel(CodeLabel label) {
  const FPUState *state = m_labelStateMap.get(label);
  if(state) { // set the emulators state to be as it was at the jump.
    m_FPUEmulator.setState(*state);
  } else {    // Jump not generated yet. will come later as a backward jump.
              // save emulators state at this point, so it can be checked,
              // that it will be the same (height and bottom-position),
              // when jump is generated. If they don't match, there will be
              // something wrong with codegenerator.
    state = &m_FPUEmulator.getState();
    m_labelStateMap.put(label, *state);
  }
  listLabel(label, *state);
}

UINT CodeGeneration::emitJump(const OpcodeBase &opcode, CodeLabel label) {
  const UINT            pos = size();
  const InstructionBase ins = opcode(0);
  JumpFixup             jf(opcode, pos, label, 0, ins.size());
  const FPUState       *state = m_labelStateMap.get(label);
  if(state) {
    if(*state != m_FPUEmulator.getState()) {
      throwException(_T("FPUState mismatch. State saved at label %s:<%s>, current:<%s>")
                    ,labelToString(label).cstr()
                    ,state->toString().cstr()
                    ,m_FPUEmulator.getState().toString().cstr());
    }
  } else {
    state = &m_FPUEmulator.getState();
    m_labelStateMap.put(label, *state);
  }
  insertIns(pos, ins);
  if(listEnabled()) m_listFile.add(pos, opcode, 0, label, *state);
  const UINT result = (UINT)m_jumpFixups.size();
  m_jumpFixups.add(jf);
  return result;
}

void CodeGeneration::fixupJumps(const JumpList &list, bool b) {
  const CompactUIntArray &jumps = list.getJumps(b);
  const size_t            n     = jumps.size();
  if(n) {
    const UINT jmpTo = size();
    for(size_t i = 0; i < n; i++) {
      fixupJump(jumps[i],jmpTo);
    }
    emitLabel(b?list.m_trueLabel:list.m_falseLabel);
  }
}

CodeGeneration &CodeGeneration::fixupJump(UINT index, int jmpTo) {
  if(jmpTo == -1) {
    jmpTo = size();
  }
  JumpFixup &jf = m_jumpFixups[index];
  if(jf.m_fixed) {
    throwException(_T("Jump already fixed (%s)"), jf.toString().cstr());
  }
  jf.m_jmpTo = jmpTo;
  jf.m_fixed = true;
  return *this;
}

void CodeGeneration::finalJumpFixup() {
  bool stable;
  do {
    stable = true;
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      JumpFixup &jf = m_jumpFixups[i];
      if(jf.m_isShortJump) {
        const int v = jf.m_jmpTo - jf.m_instructionPos - jf.m_instructionSize;
        if(!isByte(v)) {
          changeShortJumpToNearJump(jf);
          stable = false;
        }
      }
    }
  } while(!stable);

  for(size_t i = 0; i < m_jumpFixups.size(); i++) {
    JumpFixup &jf = m_jumpFixups[i];
    const int ipRel = jf.m_jmpTo - jf.m_instructionPos - jf.m_instructionSize;
    if(jf.m_isShortJump) {
      assert(isByte(ipRel));
    }
    const InstructionBase ins = (*jf.m_op)(ipRel);
    m_code.setBytes(jf.m_instructionPos,ins.getBytes(),ins.size());

    if(m_listFile.isOpen()) {
      ListLine *ll = m_listFile.findLineByPos(jf.m_instructionPos);
      if(ll) {
        ll->setIPrelativeOffset(ipRel);
      }
    }
  }
}

void CodeGeneration::changeShortJumpToNearJump(JumpFixup &jf) {
  assert(jf.m_isShortJump);

  const int             pos        = jf.m_instructionPos;
  const int             oldInsSize = jf.m_instructionSize;
  const InstructionBase newIns     = jf.makeInstruction();
  const int             newInsSize = newIns.size();
  const int             bytesAdded = newInsSize - oldInsSize;
  if(bytesAdded > 0) {
    insertZeroes(pos+1, bytesAdded);
    jf.m_isShortJump     = false;
    jf.m_instructionSize = newInsSize;
  }
}

void CodeGeneration::insertZeroes(UINT pos, UINT count) {
  m_code.insertZeroes(pos, count);
  for(size_t i = 0; i < m_jumpFixups.size(); i++) {
    JumpFixup &jf1 = m_jumpFixups[i];
    if(jf1.m_instructionPos > pos) jf1.m_instructionPos += count;
    if(jf1.m_jmpTo          > pos) jf1.m_jmpTo          += count;
  }
  for(size_t i = 0; i < m_callTable.size(); i++) {
    FunctionCallInfo &fi = m_callTable[i];
    if(fi.m_instructionPos > pos) {
      fi.m_instructionPos += count;
    }
  }
  if(listEnabled()) m_listFile.adjustPositions(pos, count);
}

InstructionBase JumpFixup::makeInstruction() const {
  for(UINT lastSize = m_instructionSize, jmpTo = m_jmpTo;;) {
    const int ipRel = (int)jmpTo - (int)m_instructionPos - (int)lastSize;
    const InstructionBase ins = (*m_op)(ipRel);
    if(ins.size() == lastSize) {
      return ins;
    }
    if(jmpTo > m_instructionPos) {
      jmpTo += ins.size() - (int)lastSize;
    }
    lastSize = ins.size();
  }
}

void CodeGeneration::finalize() {
  finalJumpFixup();

#if defined(IS64BIT)
  if(hasCalls()) {
    emitAddStack(LOCALSTACKSPACE + RESERVESTACKSPACE);
    emit(POP,RBX);
  }
#endif // IS64BIT
  emit(RET);

  linkFunctionCalls();
  m_code.finalize(m_addressTable.getESIValue());

  listAll();
}

#if defined(IS32BIT)
void CodeGeneration::linkFunctionCalls() {
  for(size_t i = 0; i < m_callTable.size(); i++) {
    linkFunctionCall(m_callTable[i]);
  }
}

void CodeGeneration::linkFunctionCall(const FunctionCallInfo &fci) {
  const InstructionBase ins = fci.makeInstruction(m_code);
  assert(ins.size() == fci.m_instructionSize);
  m_code.setBytes(fci.m_instructionPos, ins.getBytes(), ins.size());
}

InstructionBase FunctionCallInfo::makeInstruction(const MachineCode &code) const {
  int         lastSize = m_instructionSize;
  const BYTE *insAddr  = code.getData() + m_instructionPos;
  for(;;) {
    const intptr_t        iprel = (BYTE*)m_fp - insAddr - lastSize;
    const InstructionBase ins   = CALL(iprel);
    if(ins.size() == lastSize) {
      return ins;
    }
    lastSize = ins.size();
  }
}

#else // IS64BIT

#define FUNCENTRYSIZE sizeof(void*)
void CodeGeneration::linkFunctionCalls() {
  if(hasCalls()) {
    UINT rest8 = size()%8;
    if(rest8==0) rest8 = 8;
    const UINT fillers = 24 - rest8;
    m_code.addZeroes(fillers); // to have some fillers

    m_functionTableStart = size();

    m_code.addZeroes(m_uniqueFunctionCall.size() * FUNCENTRYSIZE); // for functionTable
// At entry in x64-mode, RCX contains address of the first instruction
    UINT pos = 0;
    pos += insert(   pos,PUSH,RBX).size();
    pos += insertLEA(pos,RBX ,QWORDPtr(RCX + m_functionTableStart)).size();
    pos += insert(   pos,SUB ,STACK_REG,(LOCALSTACKSPACE + RESERVESTACKSPACE)).size();

    assert(pos <= fillers);

    for(UINT i = 0; i < m_uniqueFunctionCall.size(); i++) {
      const UINT          rbxOffset = i*FUNCENTRYSIZE;
      const FunctionCall &fc        = m_uniqueFunctionCall[i];
      const UINT          pos       = m_functionTableStart + rbxOffset;
      m_code.setBytes(pos, (BYTE*)&fc.m_fp, FUNCENTRYSIZE);
      if(listEnabled()) {
        m_listFile.add(pos, rbxOffset, fc);
      }
    }
  }
}

UINT CodeGeneration::getFunctionRefIndex(const FunctionCall &fc) {
  const UINT *p = m_fpMap.get(fc.m_fp);
  if(p) return *p;
  const UINT index = (UINT)m_uniqueFunctionCall.size();
  m_uniqueFunctionCall.add(fc); // add new entry
  m_fpMap.put(FunctionKey(fc.m_fp), index);
  return index;
}

#endif // IS64BIT

// public in x86/private in x64
UINT CodeGeneration::emitCall(const FunctionCall &fc) {
  const OpcodeCall &opcodeCALL = CALL;
#if defined(IS32BIT)
  const InstructionOperand arg1((intptr_t)fc.m_fp); // Call immediate addr
#else // iIS64BIT
  // Call using reference-table added after code, and during exeution
  // has RBX pointing at element 0
  const UINT index = getFunctionRefIndex(fc);
  const MemoryOperand arg1(QWORDPtr(RBX + (index*sizeof(BuiltInFunction))));
#endif // IS64BIT

  const bool old = enableListing(false);
  const UINT pos = emit(opcodeCALL,arg1).getPos();
  enableListing(old);
  if(listEnabled()) m_listFile.add(pos,opcodeCALL, arg1, fc);
  m_callTable.add(FunctionCallInfo(fc, pos, (BYTE)(size()-pos)));
  return pos;
}

#if defined(IS64BIT)
#if defined(USEXMMREG)
UINT CodeGeneration::emitCall(const FunctionCall &fc, const ExpressionDestination &dst) {
  const UINT pos = emitCall(fc);
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitXMMToMem(XMM0,getStackRef(0));                     // XMM0 -> FPU-top
    emitFLD(getStackRef(0));
    break;
  case RESULT_IN_XMM       :
    if(dst.getXMMReg() == XMM1) {
      emit(MOVAPS,XMM1, XMM0);                             // XMM0 -> XMM1
    } // else do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    emitXMMToMem(XMM0, RDI);                               // XMM0 -> *RDI
    break;
  case RESULT_ON_STACK:
    emitXMMToMem(XMM0, getStackRef(dst.getStackOffset())); // XMM0 -> RSP[dst.stackOffset]
    break;
  case RESULT_IN_VALUETABLE:
    emitXMMToMem(XMM0, getTableRef(dst.getTableIndex()));  // XMM0 -> QWORD PTR[RSI+tableOffset]
    break;
  }
  return pos;
}

#else // !USEXMMREG

UINT CodeGeneration::emitCall(const FunctionCall &fc, const ExpressionDestination &dst) {
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitLEAReal(RCX, getTableRef(0));                     // RCX = RSI + getESIOffset(0);
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MOV,RCX, RDI);                                   // RCX = RDI
    break;
  case RESULT_ON_STACK:
    emitLEAReal(RCX, getStackRef(dst.getStackOffset()));  // RCX = RSP + dst.stackOffset
    break;
  case RESULT_IN_VALUETABLE:
    emitLEAReal(RCX, getTableRef(dst.getTableIndex()));   // RCX = RSI + getESIOffset(dst.tableIndex))
    break;
  }

  const UINT pos = emitCall(fc);

  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitFLD(RAX); // push *rax into FPU
    break;
  }
  return pos;
}

#endif // USEXMREG
#endif // IS64BIT

void CodeGeneration::list(const TCHAR *format, ...) const {
  if(!listEnabled()) return;
  va_list argptr;
  va_start(argptr,format);
  m_listFile.vprintf(format,argptr);
  va_end(argptr);
}

void CodeGeneration::listFixupTable() const {
  if(listEnabled() && !m_jumpFixups.isEmpty()) {
    list(_T(";Jump table:\n"));
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      list(_T(";%*s%s\n")
          ,LF_MARGIN,EMPTYSTRING
          ,m_jumpFixups[i].toString().cstr());
    }
    list(_T("\n"));
  }
}

void CodeGeneration::listCallTable() const {
  if(listEnabled() && !m_callTable.isEmpty()) {
    list(_T(";Call table:\n"));
    for(size_t i = 0; i < m_callTable.size(); i++) {
      list(_T(";%*s%s\n")
          ,LF_MARGIN,EMPTYSTRING
          ,m_callTable[i].toString().cstr());
    }
    list(_T("\n"));
  }
}

void CodeGeneration::listAll() {
  m_listFile.flush();
  m_listFile.clear();
  listFixupTable();
  listCallTable();
  if(listEnabled()) {
    list(_T(";Register optimizations:%d\n"), m_FPUOptimizeCount);
  }
}
String JumpFixup::toString() const {
  return format(_T("%-*d:%-*s %-4s  addr:%-5d (%-5s size:%d)%s")
               ,LF_POSLEN, m_instructionPos
               ,LF_MNELEN, m_op->getMnemonic().cstr()
               ,labelToString(m_jmpLabel).cstr()
               ,m_jmpTo
               ,m_isShortJump?_T("short"):_T("near")
               ,m_instructionSize
               ,m_fixed?EMPTYSTRING:_T("<----- Need fixup")
               );
}

String FunctionCallInfo::toString() const {
  return format(_T("%-*d:%-40s  (size:%d)")
                ,LF_POSLEN,m_instructionPos
                ,__super::toString().cstr()
                ,m_instructionSize
                );
}

}; // namespace Expr
