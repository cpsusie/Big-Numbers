#include "pch.h"
#include "CodeGeneration.h"

#ifdef IS64BIT
#define LOCALSTACKSPACE   72
#define RESERVESTACKSPACE 40
#endif

CodeGeneration::CodeGeneration(MachineCode *code, const CompactRealArray &valueTable, const StringArray &nameCommentArray, FILE *listFile)
  : m_code(*code)
  , m_valueTable(valueTable)
  , m_nameCommentArray(nameCommentArray)
  , m_listFile(listFile)
{
  setValueCount(m_valueTable.size());
#ifdef IS64BIT
  m_functionTableStart = 0;
  resetStack(RESERVESTACKSPACE);
#endif // IS64BIT

  m_lastCodeSize = 0;
  m_listComment  = NULL;
  if(hasListFile()) {
    list(_T("ESI offset from &valueTable[0] (in bytes):%d (%#02x)\n"), m_esiOffset, m_esiOffset);
  }
}

void CodeGeneration::setValueCount(size_t valueCount) {
  if(valueCount == 0) {
    m_esiOffset = 0;
    m_esi       = NULL;
  } else {
    const int maxOffset = 127-127%sizeof(Real);
    m_esiOffset = (char)min(maxOffset, (valueCount / 2) * sizeof(Real));
    m_esi       = (BYTE*)m_valueTable.getBuffer() + m_esiOffset;
  }
}

void CodeGeneration::list(const TCHAR *format, ...) const {
  va_list argptr;
  va_start(argptr,format);
  _vftprintf(m_listFile, format, argptr);
  va_end(argptr);
  fflush(m_listFile);
}

const TCHAR *CodeGeneration::findListComment(const InstructionOperand &op) const {
  if(op.isMemoryRef()) {
    const MemoryRef &mr = op.getMemoryReference();
    if(mr.hasBase() && (mr.getBase() == &TABLEREF_REG)) {
      const UINT index = esiOffsetToIndex(mr.getOffset());
      if(index < m_nameCommentArray.size()) {
        return m_nameCommentArray[index].cstr();
      }
    }
  }
  return NULL;
}

#define LISTFILE_MARGIN 4
#define LISTFILE_POSLEN 4
#define LISTFILE_OPCLEN 6

void CodeGeneration::listIns(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format,argptr);
  va_end(argptr);
  if(m_listComment) {
    list(_T("%*s%-*d:%-36s %-40s ;%s\n")
        ,LISTFILE_MARGIN,_T("")
        ,LISTFILE_POSLEN,m_lastCodeSize
        ,m_insStr.cstr(), str.cstr()
        ,m_listComment);
    m_listComment = NULL;
  } else {
    list(_T("%*s%-*d:%-36s %s\n")
        ,LISTFILE_MARGIN,_T("")
        ,LISTFILE_POSLEN,m_lastCodeSize
        ,m_insStr.cstr(), str.cstr());
  }
  m_lastCodeSize = size();
}

void CodeGeneration::listFixupTable() const {
  if(hasListFile() && !m_jumpFixups.isEmpty()) {
    list(_T("Jump table:\n"));
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      list(_T("%*s%s\n")
          ,LISTFILE_MARGIN,_T("")
          ,m_jumpFixups[i].toString().cstr());
    }
    list(_T("\n"));
  }
}

void CodeGeneration::listCallTable() const {
  if(hasListFile() && !m_callTable.isEmpty()) {
    list(_T("Call table:\n"));
    for(size_t i = 0; i < m_callTable.size(); i++) {
      list(_T("%*s%s\n")
          ,LISTFILE_MARGIN,_T("")
          ,m_callTable[i].toString().cstr());
    }
    list(_T("\n"));
  }
}

UINT CodeGeneration::insertIns(UINT pos, const InstructionBase &ins) {
  assert(pos <= size());
  const UINT added = ins.size();
  if(pos < size()) {
    m_code.insertZeroes(pos,added);
    m_code.setBytes(pos, ins.getBytes(), added);
  } else {
    m_code.append(ins.getBytes(), added);
  }
  if(hasListFile()) m_insStr = ins.toString();
  return added;
}

UINT CodeGeneration::insert(UINT pos, const Opcode0Arg &opCode) {
  const UINT added = insertIns(pos, opCode);
  if(hasListFile()) {
    listIns(_T("%s"), opCode.getMnemonic().cstr());
  }
  return added;
}

UINT CodeGeneration::insert(UINT pos, const OpcodeBase &opCode, const InstructionOperand &op) {
  const UINT added = insertIns(pos,opCode(op));
  if(hasListFile()) {
    if(m_listComment == NULL) m_listComment = findListComment(op);
    listIns(_T("%-*s %s")
           ,LISTFILE_OPCLEN, opCode.getMnemonic().cstr()
           ,op.toString().cstr());
  }
  return added;
}

UINT CodeGeneration::insert(UINT pos, const OpcodeBase &opCode, const InstructionOperand &op1, const InstructionOperand &op2) {
  const UINT added = insertIns(pos, opCode(op1,op2));
  if(hasListFile()) {
    if(m_listComment == NULL) m_listComment = findListComment(op1);
    if(m_listComment == NULL) m_listComment = findListComment(op2);
    listIns(_T("%-*s %s,%s")
           ,LISTFILE_OPCLEN, opCode.getMnemonic().cstr()
           ,op1.toString().cstr(), op2.toString().cstr());
  }
  return added;
}

UINT CodeGeneration::insert(UINT pos, const StringPrefix &prefix, const StringInstruction &strins) {
  const UINT added = insertIns(pos, prefix(strins));
  if(hasListFile()) {
    listIns(_T("%-*s %s")
           ,LISTFILE_OPCLEN, prefix.getMnemonic().cstr()
           ,strins.getMnemonic().cstr());
  }
  return added;
}

UINT CodeGeneration::insertLEA(UINT pos, const IndexRegister &dst, const MemoryOperand &mem) {
  const MemoryRef &mr = mem.getMemoryReference();
  if(mr.hasOffset()) {
    return insert(pos, LEA, dst, mem);
  } else {
    return insert(pos, MOV, dst, *mr.getBase());
  }
}

UINT CodeGeneration::insertJmp(UINT pos, const OpcodeBase &opCode, CodeLabel lbl) {
  const InstructionBase ins = opCode(0);
  JumpFixup jf(opCode, pos, lbl, 0, ins.size());
  insertIns(pos, ins);
  if(hasListFile()) {
    listIns(_T("%-*s %s")
           ,LISTFILE_OPCLEN, opCode.getMnemonic().cstr()
           ,labelToString(lbl).cstr());
  }
  const UINT result = (UINT)m_jumpFixups.size();
  m_jumpFixups.add(jf);
  return result;
}

void CodeGeneration::fixupJumps(const JumpList &list, bool b) {
  const CompactUintArray &jumps = list.getJumps(b);
  const size_t            n     = jumps.size();
  if(n) {
    const UINT jmpTo = size();
    for(size_t i = 0; i < n; i++) {
      fixupJump(jumps[i],jmpTo);
    }
    listLabel(b?list.m_trueLabel:list.m_falseLabel);
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
  listFixupTable();
  listCallTable();
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
    insertZeroes(pos, bytesAdded);
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

String JumpFixup::toString() const {
  return format(_T("%-*d:%-*s %-4s  addr:%-5d (%-5s size:%d)%s")
               ,LISTFILE_POSLEN, m_instructionPos
               ,LISTFILE_OPCLEN, m_op->getMnemonic().cstr()
               ,labelToString(m_jmpLabel).cstr()
               ,m_jmpTo
               ,m_isShortJump?_T("short"):_T("near")
               ,m_instructionSize
               ,m_fixed?_T(""):_T("<----- Need fixup")
               );
}

String FunctionCallInfo::toString() const {
  return format(_T("%-*d:%-40s  (size:%d)")
                ,LISTFILE_POSLEN,m_instructionPos
                ,__super::toString().cstr()
                ,m_instructionSize
                );
}

void CodeGeneration::finalize() {
  finalJumpFixup();

#ifdef IS64BIT
  if(hasCalls()) {
    emitAddStack(LOCALSTACKSPACE + RESERVESTACKSPACE);
    emit(POP,RBX);
  }
#endif // IS64BIT
  emit(RET);
  linkFunctionCalls();
  m_code.finalize(m_esi);
}

#ifdef IS32BIT
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
    m_code.appendZeroes(fillers); // to have some fillers

    m_functionTableStart = size();

    m_code.appendZeroes(m_uniqueFunctionCall.size() * FUNCENTRYSIZE); // for functionTable
// At entry in x64-mode, RCX contains address of the first instruction
    UINT pos = 0;
    pos += insert(   pos,PUSH,RBX);
    pos += insertLEA(pos,RBX ,QWORDPtr(RCX + m_functionTableStart));
    pos += insert(   pos,SUB ,STACK_REG,(LOCALSTACKSPACE + RESERVESTACKSPACE));

    assert(pos <= fillers);

    for(UINT i = 0; i < m_uniqueFunctionCall.size(); i++) {
      const UINT          RBXoffset = i*FUNCENTRYSIZE;
      const FunctionCall &fc        = m_uniqueFunctionCall[i];
      const UINT          codeIndex = m_functionTableStart + RBXoffset;
      m_code.setBytes(codeIndex, (BYTE*)&fc.m_fp, FUNCENTRYSIZE);
      if(hasListFile()) {
        list(_T("%*s%-*d:[%-#0*x] %s (%s)\n")
            ,LISTFILE_MARGIN,_T("")
            ,LISTFILE_POSLEN, codeIndex
            ,LISTFILE_POSLEN, RBXoffset
            ,formatHexValue((UINT64)fc.m_fp).cstr()
            ,fc.m_signature.cstr());
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
  String tmpComment;
  if(hasListFile()) {
    tmpComment    = fc.toString();
    m_listComment = tmpComment.cstr();
  }

#ifdef IS32BIT
  // Call immediate addr
  const UINT pos = emit(CALL,(intptr_t)fc.m_fp);
#else // iIS64BIT
  // Call using reference-table added after code, and during exeution
  // has RBX pointing at element 0
  const UINT index = getFunctionRefIndex(fc);
  const UINT pos   = emit(CALL,QWORDPtr(RBX + (index*sizeof(BuiltInFunction))));
#endif // IS64BIT

  m_callTable.add(FunctionCallInfo(fc, pos, (BYTE)(size()-pos)));
  return pos;
}

#ifdef IS64BIT
#ifndef LONGDOUBLE
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

#else // LONGDOUBLE

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

#endif // LONGDOUBLE
#endif // IS64BIT
