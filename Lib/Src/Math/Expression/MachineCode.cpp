#include "pch.h"
#include "ExpressionCompile.h"

DEFINECLASSNAME(MachineCode);

MachineCode::MachineCode(ParserTree &tree, FILE *listFile)
  : m_valueTable(tree.getValueTable())
{
  setValueCount(m_valueTable.size());
  m_entryPoint   = NULL;

  initValueStr(tree.getAllVariables());
  m_lastCodeSize = 0;
  m_listFile     = listFile;
  m_listComment  = NULL;
  if(hasListFile()) {
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
  finalJumpFixup();
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

void MachineCode::initValueStr(const ExpressionVariableArray &variables) {
  for(size_t i = 0; i < getValueCount(); i++) {
    m_valueStr.add(EMPTYSTRING);
  }
  for(size_t i = 0; i < variables.size(); i++) {
    const ExpressionVariable &var = variables[i];
    m_valueStr[var.getValueIndex()] = var.getName();
  }
  int tmpCounter = 0;
  for(size_t i = 0; i < m_valueStr.size(); i++) {
    if(m_valueStr[i].isEmpty()) {
      if(isNan(m_valueTable[i])) {
        m_valueStr[i] = format(_T("$tmp%d"), tmpCounter++);
      } else {
        m_valueStr[i] = toString(m_valueTable[i]);
      }
    }
  }
  for(size_t i = 0; i < m_valueStr.size(); i++) {
    m_valueStr[i] = format(_T("value[%2d]:%s"), i, m_valueStr[i].cstr());
  }
}

void MachineCode::list(const TCHAR *format, ...) const {
  va_list argptr;
  va_start(argptr,format);
  _vftprintf(m_listFile, format, argptr);
  va_end(argptr);
  fflush(m_listFile);
}

const TCHAR *MachineCode::findListComment(const InstructionOperand &op) const {
  if(op.isMemoryRef()) {
    const MemoryRef &mr = op.getMemoryReference();
    if(mr.hasBase() && (mr.getBase() == &TABLEREF_REG)) {
      const size_t index = esiOffsetToIndex(mr.getOffset());
      if(index < m_valueStr.size()) {
        return m_valueStr[index].cstr();
      }
    }
  }
  return NULL;
}

void MachineCode::listIns(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format,argptr);
  va_end(argptr);
  if(m_listComment) {
    list(_T("    %-4d:%-36s %-40s ;%s\n"), m_lastCodeSize, m_insStr.cstr(), str.cstr(), m_listComment);
    m_listComment = NULL;
  } else {
    list(_T("    %-4d:%-36s %s\n"), m_lastCodeSize, m_insStr.cstr(), str.cstr());
  }
  m_lastCodeSize = (int)size();
}

void MachineCode::listFixupTable() const {
  if(hasListFile() && !m_jumpFixups.isEmpty()) {
    list(_T("Jump table:\n"));
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      list(_T("  %s\n"), m_jumpFixups[i].toString().cstr());
    }
    list(_T("\n"));
  }
}

int MachineCode::emitIns(const InstructionBase &ins) {
  const int pos = (int)size();
  append(ins.getBytes(), ins.size());
  if(hasListFile()) m_insStr = ins.toString();
  return pos;
}

int MachineCode::emit(const Opcode0Arg &opCode) {
  const int ret = emitIns(opCode);
  if(hasListFile()) listIns(_T("%s"), opCode.getMnemonic().cstr());
  return ret;
}

int MachineCode::emit(const OpcodeBase &opCode, const InstructionOperand &op) {
  const int ret = emitIns(opCode(op));
  if(hasListFile()) {
    m_listComment = findListComment(op);
    listIns(_T("%-6s %s"), opCode.getMnemonic().cstr(), op.toString().cstr());
  }
  return ret;
}
int MachineCode::emit(const OpcodeBase &opCode, const InstructionOperand &op1, const InstructionOperand &op2) {
  const int ret = emitIns(opCode(op1,op2));
  if(hasListFile()) {
    m_listComment = findListComment(op1);
    if(m_listComment == NULL) m_listComment = findListComment(op2);
    listIns(_T("%-6s %s,%s"), opCode.getMnemonic().cstr(), op1.toString().cstr(), op2.toString().cstr());
  }
  return ret;
}
int MachineCode::emit(const StringPrefix &prefix, const StringInstruction &strins) {
  const int ret = emitIns(prefix(strins));
  if(hasListFile()) listIns(_T("%-6s %s"), prefix.getMnemonic().cstr(), strins.getMnemonic().cstr());
  return ret;
}

void MachineCode::emitFLD(const ExpressionNode *n) {
  if(n->isOne()) {
    emit(FLD1);
  } else if (n->isPi()) {
    emit(FLDPI);
  } else if(n->isZero()) {
    emit(FLDZ);
  } else {
    emitFLD(getTableRef(n->getValueIndex()));
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
    emitFLD(n);
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM     :
    emitMemToXMM(dst.getXMMReg(), getTableRef(n->getValueIndex()));
    return true;
#endif // LONGDOUBLE
  }
  return returnValue;
}
#endif // IS64BIT

void MachineCode::emitLoadAddr(const IndexRegister &dst, const MemoryRef &ref) {
  if(ref.getOffset() == 0) {
    emit(MOV,dst, *ref.getBase());
  } else {
    emit(LEA,dst, RealPtr(ref));
  }
}

InstructionBase JumpFixup::makeInstruction() const {
  for(int lastSize = m_instructionSize, jmpTo = m_jmpTo;;) {
    const int ipRel = jmpTo - m_instructionPos - lastSize;
    const InstructionBase ins = (*m_op)(ipRel);
    if(ins.size() == lastSize) {
      return ins;
    }
    if(jmpTo > m_instructionPos) {
      jmpTo += ins.size() - lastSize;
    }
    lastSize = ins.size();
  }
}

String JumpFixup::toString() const {
  return format(_T("%3d:%-4s %-4s  addr:%-5d (%-5s size:%d)%s")
               ,m_instructionPos
               ,m_op->getMnemonic().cstr()
               ,labelToString(m_jmpLabel).cstr()
               ,m_jmpTo
               ,m_isShortJump?_T("short"):_T("near")
               ,m_instructionSize
               ,m_fixed?_T(""):_T("<----- Need fixup")
               );
}

int MachineCode::emitJmp(const OpcodeBase &op, CodeLabel lbl) {
  const int result = (int)m_jumpFixups.size();
  JumpFixup jf(op, (int)size(), lbl);
  emitIns(op(0));
  if(hasListFile()) listIns(_T("%-6s %s"), op.getMnemonic().cstr(), labelToString(lbl).cstr());
  jf.m_instructionSize = (BYTE)((int)size() - jf.m_instructionPos);
  m_jumpFixups.add(jf);
  return result;
}

void MachineCode::fixupJumps(const JumpList &list, bool b) {
  const CompactIntArray &jumps = list.getJumps(b);
  const size_t           n     = jumps.size();
  if(n) {
    const int jmpTo = (int)size();
    for(size_t i = 0; i < n; i++) {
      fixupJump(jumps[i],jmpTo);
    }
    listLabel(b?list.m_trueLabel:list.m_falseLabel);
  }
}

void MachineCode::fixupJump(int index, int jmpTo) {
  JumpFixup &jf = m_jumpFixups[index];
  if(jf.m_fixed) {
    throwException(_T("Jump already fixed (%s)"), jf.toString().cstr());
  }
  jf.m_jmpTo = jmpTo;
  jf.m_fixed = true;
}

void MachineCode::finalJumpFixup() {
  listFixupTable();

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
    const intptr_t        iprel = (BYTE*)m_func - insAddr - lastSize;
    const InstructionBase ins   = CALL(iprel);
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
  emit(MOV,EAX,DWORDPtr(ESP));
  emitFLD(EAX);
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
    emitXMMToMem(XMM0,getStackRef(0));                                // XMM0 -> FPU-top
    emitFLD(getStackRef(0));
    break;
  case RESULT_IN_XMM       :
    if(dst.getXMMReg() == XMM1) {
      emit(MOVAPS,XMM1, XMM0);                                        // XMM0 -> XMM1
    } // else do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    emitXMMToMem(XMM0, RDI);                                          // XMM0 -> *RDI
    break;
  case RESULT_ON_STACK:
    emitXMMToMem(XMM0, getStackRef(dst.getStackOffset()));            // XMM0 -> RSP[dst.stackOffset]
    break;
  case RESULT_IN_VALUETABLE:
    emitXMMToMem(XMM0, getTableRef(dst.getTableIndex()));             // XMM0 -> QWORD PTR[RSI+tableOffset]
    break;
  }
}

#else // LONGDOUBLE

void MachineCode::emitCall(BuiltInFunction f, const ExpressionDestination &dst) {
  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitLoadAddr(RCX, getTableRef(0));                             // RCX = RSI + getESIOffset(0);
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MOV,RCX, RDI);                                            // RCX = RDI
    break;
  case RESULT_ON_STACK:
    emitLoadAddr(RCX, getStackRef(dst.getStackOffset()));          // RCX = RSP + dst.stackOffset
    break;
  case RESULT_IN_VALUETABLE:
    emitLoadAddr(RCX, getTableRef(dst.getTableIndex()));           // RCX = RSI + getESIOffset(dst.tableIndex))
    break;
  }

  emitCall(f);

  switch(dst.getType()) {
  case RESULT_IN_FPU       :
    emitFLD(RAX); // push *rax into FPU
    break;
  }
}

#endif // LONGDOUBLE

#endif // IS64BIT
