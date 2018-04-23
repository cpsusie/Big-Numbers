#include "pch.h"
#include "ExpressionCompile.h"

DEFINECLASSNAME(MachineCode);

MachineCode::MachineCode(const CompactRealArray &valueTable, FILE *listFile) : m_valueTable(valueTable) {
  setValueCount(m_valueTable.size());
  m_entryPoint   = NULL;
  m_lastCodeSize = 0;
  m_listFile     = listFile;
  if(isListFileOpen()) {
    list(_T("ESI offset from &valueTable[0] (in bytes):%d (%#02x)\n"), m_esiOffset, m_esiOffset);
  }
#ifdef TRACE_CALLS
  m_callsGenerated = false;
#endif
#ifdef IS64BIT
#ifndef LONGDOUBLE
  m_referenceFunction = (BYTE*)(BuiltInFunction1)exp;
#else // LONGDOUBLE
  m_referenceFunction = (BYTE*)(BuiltInFunctionRef1)exp;
#endif // LONGDOUBLE
#endif // IS64BIT
}

MachineCode::~MachineCode() {
  clear();
}

void MachineCode::clear() {
  __super::clear();
  clearJumpTable();
#ifdef IS32BIT
  clearFunctionCalls();
#endif
  setValueCount(0);
}

void MachineCode::finalize() {
  fixupJumps();
#ifdef IS32BIT
  linkFunctionCalls();
#endif

  m_entryPoint = (ExpressionEntryPoint)getData();
  clearJumpTable();
#ifdef IS32BIT
  clearFunctionCalls();
#endif
  flushInstructionCache();
}

int MachineCode::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  append((BYTE*)bytes,count);
  return ret;
}

void MachineCode::setValueCount(size_t valueCount) {
  if(valueCount == 0) {
    m_esiOffset = 0;
    m_esi       = NULL;
  } else {
    const int maxOffset = 127-127%sizeof(Real);
    m_esiOffset = (char)min(maxOffset, (valueCount / 2) * sizeof(Real));
    m_esi = (BYTE*)m_valueTable.getBuffer() + m_esiOffset;
  }
}

void MachineCode::list(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  _vftprintf(m_listFile, format, argptr);
  va_end(argptr);
}

void MachineCode::listIns(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format,argptr);
  va_end(argptr);
  list(_T("    %-4d:%-30s   %s\n"), m_lastCodeSize, m_insStr.cstr(), str.cstr());
  m_lastCodeSize = (int)size();
}

int MachineCode::emitIns(const InstructionBase &ins) {
  const int pos = (int)size();
  append(ins.getBytes(), ins.size());
  if(isListFileOpen()) m_insStr = ins.toString();
  return pos;
}

int MachineCode::emit(const Opcode0Arg &opCode) {
  const int ret = emitIns(opCode);
  if(isListFileOpen()) listIns(_T("%s"), opCode.getMnemonic().cstr());
  return ret;
}

int  MachineCode::emitJmpWithLabel(const OpcodeBase &opCode, CodeLabel label) {
  const int ret = emitIns(opCode(0));
  if(isListFileOpen()) listIns(_T("%s %s"), opCode.getMnemonic().cstr(), labelToString(label).cstr());
  return ret;
}

int MachineCode::emit(const OpcodeBase &opCode, const InstructionOperand &op) {
  const int ret = emitIns(opCode(op));
  if(isListFileOpen()) listIns(_T("%s %s"), opCode.getMnemonic().cstr(), op.toString().cstr());
  return ret;
}
int MachineCode::emit(const OpcodeBase &opCode, const InstructionOperand &op1, const InstructionOperand &op2) {
  const int ret = emitIns(opCode(op1,op2));
  if(isListFileOpen()) listIns(_T("%s %s,%s"), opCode.getMnemonic().cstr(), op1.toString().cstr(), op2.toString().cstr());
  return ret;
}
int MachineCode::emit(const StringPrefix &prefix, const StringInstruction &strins) {
  const int ret = emitIns(prefix(strins));
  if(isListFileOpen()) listIns(_T("%s %s"), prefix.getMnemonic().cstr(), strins.getMnemonic().cstr());
  return ret;
}

void MachineCode::emitFLoad(const ExpressionNode *n) {
  if(n->isOne()) {
    emit(FLD1);
  } else if (n->isPi()) {
    emit(FLDPI);
  } else if(n->isZero()) {
    emit(FLDZ);
  } else {
    emit(FLD, getValPtr(n));
  }
}

#ifdef IS64BIT
bool MachineCode::emitFLoad(const ExpressionNode *n, const ExpressionDestination &dst) {
  bool returnValue = true;
  switch(dst.getType()) {
  case RESULT_IN_ADDRRDI   :
  case RESULT_ON_STACK     :
  case RESULT_IN_VALUETABLE:
    returnValue = false; // false  indicates that value needs to be moved from FPU to desired destination
    // NB continue case
  case RESULT_IN_FPU       :
    emitFLoad(n);
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM     :
    emitValToXMM(dst.getXMMReg(), n->getValueIndex());
    return true;
#endif // LONGDOUBLE
  }
  return returnValue;
}
#endif // IS64BIT

void MachineCode::emitLoadAddr(const IndexRegister &dst, const IndexRegister &src, int offset) {
  if(offset == 0) {
    emit(MOV,dst, src);
  } else {
    emit(LEA,dst, RealPtr(src+offset));
  }
}

#ifdef IS32BIT
void MachineCode::emitAddESP(int n) {
  if(n == 0) return;
  emit(ADD,ESP,n);
}

void MachineCode::emitSubESP(int n) {
  if(n == 0) return;
  emit(SUB,ESP,n);
}

#else // IS64BIT

void MachineCode::emitAddRSP(int n) {
  emitAddR64(RSP,n);
}

void MachineCode::emitSubRSP(int n) {
  emitSubR64(RSP,n);
}

void MachineCode::emitAddR64(const GPRegister &r64, int value) {
  if(value == 0) return;
  emit(ADD,r64,value);
}

void MachineCode::emitSubR64(const GPRegister &r64, int value) {
  if(value == 0) return;
  emit(SUB,r64,value);
}

BYTE MachineCode::pushTmp() {
  const BYTE offset = m_stackTop;
  m_stackTop += sizeof(Real);
  return offset;
}

BYTE MachineCode::popTmp()  {
  m_stackTop -= sizeof(Real);
  return m_stackTop;
}

#endif // IS64BIT

InstructionBase JumpFixup::makeInstruction() const {
  for(int lastSize = m_instructionSize, jmpTo = m_jmpTo;;) {
    const int ipRel = jmpTo - m_instructionPos - lastSize;
    const InstructionBase ins = m_op(ipRel);
    if(ins.size() == lastSize) {
      return ins;
    }
    if(jmpTo > m_instructionPos) {
      jmpTo += ins.size() - lastSize;
    }
    lastSize = ins.size();
  }
}

int MachineCode::emitJmp(const OpcodeBase &op, CodeLabel lbl) {
  const int result = (int)m_jumpFixups.size();
  JumpFixup jf(op, (int)size());
  emitJmpWithLabel(op,lbl);
  jf.m_instructionSize = (BYTE)((int)size() - jf.m_instructionPos);
  m_jumpFixups.add(jf);
  return result;
}

void MachineCode::fixupJumps(const CompactIntArray &jumps, int jmpTo) {
  for(size_t i = 0; i < jumps.size(); i++) {
    fixupJump(jumps[i],jmpTo);
  }
}

void MachineCode::fixupJumps() {
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
    const InstructionBase ins = jf.m_op(ipRel);
    setBytes(jf.m_instructionPos,ins.getBytes(),ins.size());
  }
}

void MachineCode::changeShortJumpToNearJump(JumpFixup &jf) {
  assert(jf.m_isShortJump);

  const int             pos        = jf.m_instructionPos;
  const int             oldInsSize = jf.m_instructionSize;
  const InstructionBase newIns     = jf.makeInstruction();
  const int             newInsSize = newIns.size();
  const int             bytesAdded = newInsSize - oldInsSize;
  if(bytesAdded > 0) {
    insertZeroes(pos, bytesAdded);
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      JumpFixup &jf1 = m_jumpFixups[i];
      if(jf1.m_instructionPos > pos) jf1.m_instructionPos += bytesAdded;
      if(jf1.m_jmpTo          > pos) jf1.m_jmpTo          += bytesAdded;
    }
    jf.m_isShortJump     = false;
    jf.m_instructionSize = newInsSize;
#ifdef IS32BIT
    adjustFunctionCalls(pos, bytesAdded);
#endif
  }
}

#ifdef IS32BIT
void MachineCode::adjustFunctionCalls(int pos, int bytesAdded) {
  for(size_t i = 0; i < m_callArray.size(); i++) {
    FunctionCall &fc = m_callArray[i];
    if(fc.m_pos > pos) {
      fc.m_pos += bytesAdded;
    }
  }
}

InstructionBase FunctionCall::makeInstruction(const MachineCode *code) const {
  int lastSize = m_instructionSize;
  const BYTE    *insAddr  = code->getData() + m_pos;
  for(;;) {
    const intptr_t iprel    = (BYTE*)m_func - insAddr - lastSize;
    const InstructionBase ins = CALL(iprel);
    if(ins.size() == lastSize) {
      return ins;
    }
    lastSize = ins.size();
  }
}

void MachineCode::linkFunctionCalls() {
  for(size_t i = 0; i < m_callArray.size(); i++) {
    linkFunctionCall(m_callArray[i]);
  }
}

void MachineCode::linkFunctionCall(const FunctionCall &fc) {
  const InstructionBase ins = fc.makeInstruction(this);
  assert(ins.size() == fc.m_instructionSize);
  setBytes(fc.m_pos, ins.getBytes(), ins.size());
}

void MachineCode::emitCall(BuiltInFunction f) {
#ifdef TRACE_CALLS
  m_callsGenerated = true;
#endif
  const int pos = emit(CALL,(intptr_t)f);
  m_callArray.add(FunctionCall(pos, (BYTE)(size()-pos), f));
}

void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dummy) {
  emitCall(f);
#ifdef LONGDOUBLE
  emit(MOV(EAX,DWORDPtr(ESP)));
  emit(FLD_REAL(EAX));
#endif // LONGDOUBLE
}

#else // IS64BIT

void MachineCode::emitCall(BuiltInFunction f) {
#ifdef TRACE_CALLS
  m_callsGenerated = true;
#endif
  emit(LEA,RAX,QWORDPtr(RBP+(int)((BYTE*)f-m_referenceFunction)));
  emit(CALL,RAX);
}

#ifndef LONGDOUBLE
void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dst) {
  emitCall(f);
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitXMMToStack(XMM0,0);                                // XMM0 -> FPU-top
    emit(FLD, getValStackPtr(0));
    break;
  case RESULT_IN_XMM       :
    if(dst.getXMMReg() == XMM1) {
      emit(MOVAPS,XMM1, XMM0);                             // XMM0 -> XMM1
    } // else do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MOVSD1,MMWORDPtr(RDI),XMM0);                      // XMM0 -> *RDI
    break;
  case RESULT_ON_STACK:
    emitXMMToStack(XMM0, dst.getStackOffset());            // XMM0 -> RSP[dst.stackOffset]
    break;
  case RESULT_IN_VALUETABLE:
    emitXMMToVal(XMM0, dst.getTableIndex());               // XMM0 -> QWORD PTR[RSI+tableOffset]
    break;
  }
}

#else // LONGDOUBLE

void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dst) {
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitLoadAddr(RCX, RSI, getESIOffset(0));                       // RCX = RSI + getESIOffset(0);
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MOV,RCX, RDI);                                            // RCX = RDI
    break;
  case RESULT_ON_STACK:
    emitLoadAddr(RCX, RSP, dst.getStackOffset());                  // RCX = RSP + dst.stackOffset
    break;
  case RESULT_IN_VALUETABLE:
    emitLoadAddr(RCX, RSI, getESIOffset(dst.getTableIndex()));     // RCX = RSI + getESIOffset(dst.tableIndex))
    break;
  }

  emitCall(f);

  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emit(FLD, RealPtr(RAX)); // push *rax into FPU
    break;
  }
}

#endif // LONGDOUBLE

#endif // IS64BIT
