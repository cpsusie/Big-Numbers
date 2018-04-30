#include "pch.h"
#include "CodeGeneration.h"

#ifdef IS64BIT
#define LOCALSTACKSPACE   72
#define RESERVESTACKSPACE 40
#endif

CodeGeneration::CodeGeneration(MachineCode *code, const CompactRealArray &valueTable, const StringArray &nameCommentArray, FILE *listFile)
  : m_code(*code)
  , m_valueTable(valueTable)
  , m_listFile(listFile, nameCommentArray)
  , m_listEnabled(listFile!=NULL)
{
  setValueCount(m_valueTable.size());
#ifdef IS64BIT
  m_functionTableStart = 0;
  resetStack(RESERVESTACKSPACE);
#endif // IS64BIT

  m_lastCodeSize = 0;
  if(listingEnabled()) {
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
  m_listFile.setEsiOffset(m_esiOffset);
}

UINT CodeGeneration::insertIns(UINT pos, const InstructionBase &ins) {
  assert(pos <= size());
  const UINT added = ins.size();
  if(pos < size()) {
    insertZeroes(pos,added);
    m_code.setBytes(pos, ins.getBytes(), added);
  } else {
    m_code.append(ins.getBytes(), added);
  }
  return added;
}

UINT CodeGeneration::insert(UINT pos, const Opcode0Arg &opcode) {
  const UINT added = insertIns(pos, opcode);
  if(listingEnabled()) m_listFile.add(ListLine(pos,opcode));
  return added;
}

UINT CodeGeneration::insert(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg) {
  const UINT added = insertIns(pos,opcode(arg));
  if(listingEnabled()) m_listFile.add(ListLine(pos,opcode,arg));
  return added;
}

UINT CodeGeneration::insert(UINT pos, const OpcodeBase &opcode, const InstructionOperand &arg1, const InstructionOperand &arg2) {
  const UINT added = insertIns(pos, opcode(arg1,arg2));
  if(listingEnabled()) m_listFile.add(ListLine(pos, opcode,arg1, arg2));
  return added;
}

UINT CodeGeneration::insert(UINT pos, const StringPrefix &prefix, const StringInstruction &strins) {
  const UINT added = insertIns(pos, prefix(strins));
  if(listingEnabled()) m_listFile.add(ListLine(pos, prefix, strins));
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

UINT CodeGeneration::insertJmp(UINT pos, const OpcodeBase &opcode, CodeLabel label) {
  const InstructionBase ins = opcode(0);
  JumpFixup jf(opcode, pos, label, 0, ins.size());
  insertIns(pos, ins);
  if(listingEnabled()) m_listFile.add(ListLine(pos, opcode, 0, label));
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

    if(m_listFile.hasListFile()) {
      ListLine *ll = m_listFile.findLineByPos(jf.m_instructionPos);
      if(ll) {
        assert(ll->m_type == LLT_JUMP);
        ll->m_iprel = ipRel;
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
  if(listingEnabled()) m_listFile.adjustPositions(pos, count);
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

#ifdef IS64BIT
  if(hasCalls()) {
    emitAddStack(LOCALSTACKSPACE + RESERVESTACKSPACE);
    emit(POP,RBX);
  }
#endif // IS64BIT
  emit(RET);

  linkFunctionCalls();
  m_code.finalize(m_esi);

  m_listFile.flush();
  listFixupTable();
  listCallTable();
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
      const UINT          rbxOffset = i*FUNCENTRYSIZE;
      const FunctionCall &fc        = m_uniqueFunctionCall[i];
      const UINT          pos       = m_functionTableStart + rbxOffset;
      m_code.setBytes(pos, (BYTE*)&fc.m_fp, FUNCENTRYSIZE);
      if(listingEnabled()) {
        m_listFile.add(ListLine(pos, rbxOffset, fc));
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
#ifdef IS32BIT
  const InstructionOperand arg1((intptr_t)fc.m_fp); // Call immediate addr
#else // iIS64BIT
  // Call using reference-table added after code, and during exeution
  // has RBX pointing at element 0
  const UINT index = getFunctionRefIndex(fc);
  const MemoryOperand arg1(QWORDPtr(RBX + (index*sizeof(BuiltInFunction))));
#endif // IS64BIT

  const bool old = enableListing(false);
  const UINT pos = emit(opcodeCALL,arg1);
  enableListing(old);
  if(listingEnabled()) m_listFile.add(ListLine(pos,opcodeCALL, arg1, fc));
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

// --------------------------------- ListFile functions ----------------------------

void CodeGeneration::list(const TCHAR *format, ...) const {
  if(!listingEnabled()) return;
  va_list argptr;
  va_start(argptr,format);
  m_listFile.vprintf(format,argptr);
  va_end(argptr);
}

void ListFile::adjustPositions(UINT pos, UINT bytesAdded) {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    ListLine &l = (*this)[i];
    if(l.m_pos >= pos) l.m_pos += bytesAdded;
  }
}

static int listFileLineCmp(const ListLine &l1, const ListLine &l2) {
  return (int)l1.m_pos - (int)l2.m_pos;
}

void ListFile::flush() {
  if(!hasListFile()) return;
  sort(listFileLineCmp);
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    const ListLine &l = (*this)[i];
    _ftprintf(m_f, _T("%s\n"), l.toString().cstr());
  }
  clear();
}

void ListFile::vprintf(const TCHAR *format, va_list argptr) const {
  _vftprintf(m_f, format, argptr);
}

const TCHAR *ListFile::findListComment(const InstructionOperand &op) const {
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

ListLine *ListFile::findLineByPos(UINT pos) {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    ListLine &line = (*this)[i];
    if(line.m_pos == pos) {
      return &line;
    }
  }
  return NULL;
}

MemoryOperand ListLine::s_dummy = BYTEPtr(0);

#define LF_MARGIN  4
#define LF_POSLEN  4
#define LF_INSLEN 36
#define LF_MNELEN  6
#define LF_OPSLEN 40

String ListLine::toString() const {
  switch(m_type) {
  case LLT_LABEL    :
    return format(_T("%s:"), labelToString(m_label).cstr());
  case LLT_0ARG     :
    return format(_T("%*s%-*d:%-*s %-*s")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_INSLEN, toInsStr(*m_opcode0).cstr()
                 ,LF_OPSLEN, m_opcode0->getMnemonic().cstr()
                 );

  case LLT_1ARG     :
    return format(_T("%*s%-*d:%-*s %-*s")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_INSLEN, toInsStr((*m_opcode)(*m_op1)).cstr()
                 ,LF_OPSLEN, format(_T("%-*s %s")
                                   ,LF_MNELEN, m_opcode->getMnemonic().cstr()
                                   ,m_op1->toString().cstr()).cstr()
                 );
  case LLT_2ARG     :
    return format(_T("%*s%-*d:%-*s %-*s")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_INSLEN, toInsStr((*m_opcode)(*m_op1,*m_op2)).cstr()
                 ,LF_OPSLEN, format(_T("%-*s %s, %s")
                                   ,LF_MNELEN, m_opcode->getMnemonic().cstr()
                                   ,m_op1->toString().cstr()
                                   ,m_op2->toString().cstr()
                                   ).cstr()
                 );
  case LLT_STRINGOP :
    return format(_T("%*s%-*d:%-*s %-*s")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_INSLEN, toInsStr((*m_stringPrefix)(*m_strins)).cstr()
                 ,LF_OPSLEN, format(_T("%-*s %s")
                                   ,LF_MNELEN, m_stringPrefix->getMnemonic().cstr()
                                   ,m_strins->getMnemonic().cstr()
                                   ).cstr()
                 );
  case LLT_JUMP     :
    return format(_T("%*s%-*d:%-*s %-*s; %s")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_INSLEN, toInsStr((*m_opcode)(m_iprel)).cstr()
                 ,LF_OPSLEN, format(_T("%-*s %s")
                                   ,LF_MNELEN, m_opcode->getMnemonic().cstr()
                                   ,formatHexValue(m_iprel,false).cstr()
                                   ).cstr()
                 ,labelToString(m_label).cstr()
                 );
  case LLT_CALL     :
    return format(_T("%*s%-*d:%-*s %-*s; %s")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_INSLEN, toInsStr((*m_opcode)(*m_op1)).cstr()
                 ,LF_OPSLEN, format(_T("%-*s %s")
                                   ,LF_MNELEN, m_opcode->getMnemonic().cstr()
                                   ,m_op1->toString().cstr()
                                   ).cstr()
                 ,m_call.toString().cstr()
                 );
  case LLT_FUNCADDR:
    return format(_T("%*s%-*d:[%-#0*x] %s (%s)")
                 ,LF_MARGIN,_T("")
                 ,LF_POSLEN, m_pos
                 ,LF_POSLEN, m_rbxOffset
                 ,formatHexValue((UINT64)m_call.m_fp).cstr()
                 ,m_call.m_signature.cstr());
  default:
    return format(_T("Unkown linetype:%d"), m_type);
  }
}
/*
void CodeGeneration::listIns(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  const String str = vformat(format,argptr);
  va_end(argptr);
  if(m_listComment) {
    list(_T("%*s%-*d:%-36s %-40s ;%s\n")
        ,LF_MARGIN,_T("")
        ,LF_POSLEN,m_lastCodeSize
        ,m_insStr.cstr(), str.cstr()
        ,m_listComment);
    m_listComment = NULL;
  } else {
    list(_T("%*s%-*d:%-36s %s\n")
        ,LF_MARGIN,_T("")
        ,LF_POSLEN,m_lastCodeSize
        ,m_insStr.cstr(), str.cstr());
  }
  m_lastCodeSize = size();
}
*/
void CodeGeneration::listFixupTable() const {
  if(listingEnabled() && !m_jumpFixups.isEmpty()) {
    list(_T("Jump table:\n"));
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      list(_T("%*s%s\n")
          ,LF_MARGIN,_T("")
          ,m_jumpFixups[i].toString().cstr());
    }
    list(_T("\n"));
  }
}

void CodeGeneration::listCallTable() const {
  if(listingEnabled() && !m_callTable.isEmpty()) {
    list(_T("Call table:\n"));
    for(size_t i = 0; i < m_callTable.size(); i++) {
      list(_T("%*s%s\n")
          ,LF_MARGIN,_T("")
          ,m_callTable[i].toString().cstr());
    }
    list(_T("\n"));
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
               ,m_fixed?_T(""):_T("<----- Need fixup")
               );
}

String FunctionCallInfo::toString() const {
  return format(_T("%-*d:%-40s  (size:%d)")
                ,LF_POSLEN,m_instructionPos
                ,__super::toString().cstr()
                ,m_instructionSize
                );
}
