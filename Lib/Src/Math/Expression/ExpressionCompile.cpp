#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/MathFunctions.h>

DEFINECLASSNAME(MachineCode);

#define ISBYTE(i)  ((i) >= -128 && (i) < 128)

MachineCode::MachineCode() {
}

MachineCode::~MachineCode() {
  clear();
}

void MachineCode::clear() {
  ExecutableByteArray::clear();
  m_jumpFixups.clear();
  m_refenceArray.clear();
}

MachineCode::MachineCode(const MachineCode &src) : ExecutableByteArray(src) {
  m_refenceArray = src.m_refenceArray;
  linkReferences();
}

MachineCode &MachineCode::operator=(const MachineCode &src) {
  ExecutableByteArray::operator=(src);
  m_refenceArray = src.m_refenceArray;
  linkReferences();
  return *this;
}

int MachineCode::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  append((BYTE*)bytes,count);
  return ret;
}

void MachineCode::setBytes(int addr, const void *bytes, int count) {
  for(BYTE *p = (BYTE*)bytes; count--;) {
    (*this)[addr++] = *(p++);
  }
}

int MachineCode::emit(const IntelInstruction &ins) {
  const int pos = (int)size();
  append((BYTE*)&ins.m_bytes, ins.m_size);
  return pos;
}

void MachineCode::emitTableOp(const IntelOpcode &op, int index) {
  const int offset = index * sizeof(Real);
  if(ISBYTE(offset)) {
    emit(MEM_ADDR_PTR1(op, ESI, offset)); // ie RSI for x64, but its the same value
  } else {
    emit(MEM_ADDR_PTR4(op, ESI, offset));
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
    if(n->isOne()) {
      emit(FLD1);
    } else if (n->isPi()) {
      emit(FLDPI);
    } else if(n->isZero()) {
      emit(FLDZ);
    } else {
      emitFLoad(n);
    }
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM     :
    emitTableOp(MOVSD_XMM_MMWORD(dst.getXMMReg()), n);
    return true;
#endif
  }
  return returnValue;
}
#endif

#ifdef IS32BIT
void MachineCode::emitAddESP(int n) {
  if(ISBYTE(n)) {
    emit(ADD_R32_IMM_BYTE(ESP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    emit(ADD_R32_IMM_DWORD(ESP));
    addBytes(&n,4);
  }
}

void MachineCode::emitSubESP(int n) {
  if(ISBYTE(n)) {
    emit(SUB_R32_IMM_BYTE(ESP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    emit(SUB_R32_IMM_DWORD(ESP));
    addBytes(&n,4);
  }
}

#else // IS64BIT

void MachineCode::emitAddRSP(int n) {
  DEFINEMETHODNAME;
  if(ISBYTE(n)) {
    emit(ADD_R64_IMM_BYTE(RSP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    throwInvalidArgumentException(method, _T("n must be in range [0..255]"));
  }
}

void MachineCode::emitSubRSP(int n) {
  DEFINEMETHODNAME;
  if(ISBYTE(n)) {
    emit(SUB_R64_IMM_BYTE(RSP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    throwInvalidArgumentException(method, _T("n must be in range [0..255]"));
  }
}

void MachineCode::emitAddR64(int r64, int  value) {
  if(value == 0) return;
  if(ISBYTE(value)) {
    emit(ADD_R64_IMM_BYTE(r64));
    const char byte = (char)value;
    addBytes(&byte,1);
  } else {
    emit(ADD_R64_IMM_DWORD(r64));
    addBytes(&value,4);
  }
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

#endif // IS32BIT

int MachineCode::emitShortJmp(const IntelInstruction &ins) {
  emit(ins);
  const BYTE addr = 0;
  return addBytes(&addr,1);
}

void MachineCode::fixupShortJumps(const CompactIntArray &jumps, int jmpAddr) {
  for(size_t i = 0; i < jumps.size(); i++) {
    fixupShortJump(jumps[i],jmpAddr);
  }
}

void MachineCode::fixupJumps() {
  bool stable;
  do {
    stable = true;
    for(size_t i = 0; i < m_jumpFixups.size(); i++) {
      JumpFixup &jf = m_jumpFixups[i];
      if(jf.m_isShortJump) {
        const int v = jf.m_jmpAddr - jf.m_addr - 1;
        if (!ISBYTE(v)) {
          changeShortJumpToNearJump(jf.m_addr);
          jf.m_isShortJump = false;
          stable           = false;
        }
      }
    }
  } while(!stable);

  for(size_t i = 0; i < m_jumpFixups.size(); i++) {
    JumpFixup &jf = m_jumpFixups[i];
    if(jf.m_isShortJump) {
      const int v = jf.m_jmpAddr - jf.m_addr - 1;
      assert(ISBYTE(v));
      const BYTE pcRelativAddr = (BYTE)v;
      setBytes(jf.m_addr,&pcRelativAddr,1);
    } else {
      const int pcRelativAddr = jf.m_jmpAddr - jf.m_addr - 4;
      setBytes(jf.m_addr,&pcRelativAddr,4);
    }
  }
}

void MachineCode::changeShortJumpToNearJump(int addr) {
  const int  opcodeAddr = addr-1;
  const BYTE opcode     = (*this)[opcodeAddr];
  int        bytesAdded;
  switch (opcode) {
  case 0xEB: // JMPSHORT
    { const BYTE JmpNear = 0xE9;
      setBytes(opcodeAddr, &JmpNear, 1);
      bytesAdded = 3;
      insertZeroes(addr, bytesAdded);
      for(size_t i = 0; i < m_jumpFixups.size(); i++) {
        JumpFixup &jf = m_jumpFixups[i];
        if(jf.m_addr    > addr) jf.m_addr    += bytesAdded;
        if(jf.m_jmpAddr > addr) jf.m_jmpAddr += bytesAdded;
      }
    }
    break;
  default:
    { assert((opcode >= 0x70) && (opcode <= 0x7F));
      const IntelInstruction newOpcode = B2INS(0x0F80 | (opcode&0xf));
      setBytes(opcodeAddr, &newOpcode, 2);
      bytesAdded = 4;
      insertZeroes(addr+1,bytesAdded);
      for(size_t i = 0; i < m_jumpFixups.size(); i++) {
        JumpFixup &jf = m_jumpFixups[i];
        if(     jf.m_addr == addr) jf.m_addr++; // opcode of instruction being changed, goes from 1 to 2 bytes.
        else if(jf.m_addr >  addr) jf.m_addr    += bytesAdded;
        if(jf.m_jmpAddr > addr)    jf.m_jmpAddr += bytesAdded;
      }
    }
    break;
  }
  adjustReferenceArray(addr, bytesAdded);
}

void MachineCode::adjustReferenceArray(int addr, int n) {
  for (size_t i = 0; i < m_refenceArray.size(); i++) {
    MemoryReference &mr = m_refenceArray[i];
    if(mr.m_byteIndex > addr) {
      mr.m_byteIndex += n;
    }
  }
}

void MachineCode::fixupMemoryReference(const MemoryReference &ref) {
  const BYTE    *instructionAddr  = getData() + ref.m_byteIndex;
  const intptr_t PCrelativeOffset = ref.m_memAddr - instructionAddr - sizeof(instructionAddr);
  setBytes(ref.m_byteIndex,&PCrelativeOffset,sizeof(PCrelativeOffset));
}


#ifdef IS32BIT

void MachineCode::emitCall(BuiltInFunction p, const ExpressionDestination &dummy) {
  emit(CALL);
  BuiltInFunction ref = NULL;
  const int addr = addBytes(&ref, 4);
  m_refenceArray.add(MemoryReference(addr, (BYTE*)p));
#ifdef LONGDOUBLE
  emit(MEM_ADDR_ESP(MOV_R32_DWORD(EAX)));
  emit(MEM_ADDR_PTR(FLD_REAL, EAX));
#endif
}

#else // 64 Bit

#ifndef LONGDOUBLE

void MachineCode::emitCall(BuiltInFunction p, const ExpressionDestination &dst) {
  emit(MOV_R64_IMM_QWORD(RAX));
  addBytes(&p,sizeof(p));
  emit(REG_SRC(CALLABSOLUTE, RAX));
  switch (dst.getType()) {
  case RESULT_IN_FPU       :
    emit(MEM_ADDR_ESP(MOVSD_MMWORD_XMM(XMM0)));                        // XMM0 -> FPU-top
    emit(MEM_ADDR_ESP(FLD_REAL));
    break;
  case RESULT_IN_XMM       :
    if (dst.getXMMReg() == XMM1) {
      emit(MOVEAPS(XMM1, XMM0));                                       // XMM0 -> XMM1
    } // else do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    emit(MEM_ADDR_PTR(MOVSD_MMWORD_XMM(XMM0), RDI));                   // XMM0 -> *RDI
    break;
  case RESULT_ON_STACK:
    emit(MEM_ADDR_ESP1(MOVSD_MMWORD_XMM(XMM0), dst.getStackOffset())); // XMM0 -> RSP[offset1]
    break;
  case RESULT_IN_VALUETABLE:
    emitXMM0ToAddr(dst.getTableIndex());                               // XMM0 -> QWORD PTR[RDI+tableOffset]
    break;
  }
}

#else // LONGDOUBLE

void MachineCode::emitCall(BuiltInFunction p, const ExpressionDestination &dst) {
  switch (dst.getType()) {
  case RESULT_IN_FPU       :
    emit(REG_SRC(MOV_R64_QWORD(RCX), RSI));                            // RCX = RSI; wil generate code after call, that pushed *rsi in FPU
    break;
  case RESULT_IN_ADDRRDI   :
    emit(REG_SRC(MOV_R64_QWORD(RCX), RDI));                            // RCX = RDI
    break;
  case RESULT_ON_STACK:
    { emit(REG_SRC(MOV_R64_QWORD(RCX), RSP));                          // RCX = RSP + dst.stackOffset
      emitAddR64(RCX, dst.getStackOffset());
    }
    break;
  case RESULT_IN_VALUETABLE:
    { emit(REG_SRC(MOV_R64_QWORD(RCX), RSI));                          // RCX = RSI + dst.tableIndex * sizeof(Real)
      emitAddR64(RCX, sizeof(Real) * dst.getTableIndex());
      break;
    }
  }

  emit(MOV_R64_IMM_QWORD(RAX));
  addBytes(&p,sizeof(p));
  emit(REG_SRC(CALLABSOLUTE, RAX));

  switch (dst.getType()) {
  case RESULT_IN_FPU       :
    emit(MEM_ADDR_PTR(FLD_REAL, RAX)); // push *rax intop FPU
    break;
  }
}

#endif // LONGDOUBLE

#endif // IS32BIT

void MachineCode::linkReferences() {
  for(size_t i = 0; i < m_refenceArray.size(); i++) {
    fixupMemoryReference(m_refenceArray[i]);
  }
}

#pragma warning(disable:4717)
void Expression::compile(const String &expr, bool machineCode) {
  parse(expr);
  if(!isOk()) {
    return;
  }

  setReturnType(findReturnType());

  if(machineCode) {
    genCode();
  }
}

#ifdef IS64BIT
extern "C" {
  void callRealResultExpression(ExpressionEntryPoint ep, Real *valueTable, Real &result);
  int  callIntResultExpression( ExpressionEntryPoint ep, Real *valueTable);
};
#endif // IS64BIT

Real Expression::fastEvaluateReal() {
  Real result;
#ifdef IS32BIT
  ExpressionEntryPoint  ep = m_entryPoint;
  Real                 *dp = &getValueRef(0);
  __asm {
    push esi
    mov  esi, dp
    call ep
    pop  esi
    fstp result;
  }
#else // !IS32BIT ie 64 BIT
  callRealResultExpression(m_entryPoint, &getValueRef(0), result);
#endif // IS32BIT

  return result;
}

bool Expression::fastEvaluateBool() {
#ifdef IS32BIT
  ExpressionEntryPoint  ep = m_entryPoint;
  Real                 *dp = &getValueRef(0);
  int result;
  __asm {
    push esi
    mov  esi, dp
    call ep
    pop  esi
    mov result, eax
  }
  return result ? true : false;
#else
  return callIntResultExpression(m_entryPoint, &getValueRef(0)) ? true : false;
#endif // IS32BIT
}

ExpressionReturnType Expression::findReturnType() const {
  DEFINEMETHODNAME;
  ExpressionNodeArray stmtList = getStatementList((ExpressionNode*)getRoot());
  switch(stmtList.last()->getSymbol()) {
  case RETURNREAL : return EXPR_RETURN_REAL;
  case RETURNBOOL : return EXPR_RETURN_BOOL;
  default         : throwUnknownSymbolException(method, stmtList.last());
                    return EXPR_RETURN_REAL;
  }
}

void Expression::genCode() {
  if(getTreeForm() != TREEFORM_STANDARD) {
    throwException(_T("Treeform must be STANDARD to generate machinecode. Form=%s"), getTreeFormName().cstr());
  }
  m_code.clear();
#ifdef TEST_MACHINECODE
  m_code.genTestSequence();
  m_machineCode = true;
#else
  genProlog();
  genStatementList(getRoot());
  m_code.fixupJumps();
  m_code.linkReferences();
  m_machineCode = true;
#endif

  m_entryPoint = m_code.getEntryPoint();
  m_code.flushInstructionCache();
}

void Expression::genProlog() {
#ifdef IS64BIT
#define LOCALSTACKSPACE   80
#define RESERVESTACKSPACE 40
  m_hasCalls = getRoot()->containsFunctionCall();
  m_code.resetStack(RESERVESTACKSPACE);
  if (m_hasCalls) {
    m_code.emitSubRSP(LOCALSTACKSPACE + RESERVESTACKSPACE); // to get 16-byte aligned RSP
  }
#endif
}

void Expression::genEpilog() {
#ifdef IS64BIT
  if (m_hasCalls) {
    m_code.emitAddRSP(LOCALSTACKSPACE + RESERVESTACKSPACE);
  }
#endif
  m_code.emit(RET);
}

void Expression::genStatementList(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case SEMI      :
    genStatementList(n->left());
    genStatementList(n->right());
    break;
  case ASSIGN    :
    genAssignment(n);
    break;
  case RETURNREAL:
    genExpression(n->left(), DST_ADDRRDI);
    genEpilog();
    break;
  case RETURNBOOL:
    genReturnBoolExpression(n);
    break;
  default    :
    throwUnknownSymbolException(__TFUNCTION__, n);
    break;
  }
}

#ifdef IS32BIT
void Expression::genAssignment(const ExpressionNode *n) {
  genExpression(n->right(), DST_FPU);
  m_code.emitFStorePop(n->left());
}
#else // IS64BIT
void Expression::genAssignment(const ExpressionNode *n) {
  genExpression(n->right(), DST_INVALUETABLE(n->left()->getValueIndex()));
}
#endif IS32BIT

void Expression::genReturnBoolExpression(const ExpressionNode *n) {
  JumpList jumps = genBoolExpression(n->left());
  int trueValue = 1;
  const int trueLabel  = m_code.emit(MOV_R32_IMM_DWORD(EAX));
  m_code.addBytes(&trueValue,4);
  genEpilog();

  const int falseLabel = m_code.emit(REG_SRC(XOR_R32_DWORD(EAX),EAX));
  genEpilog();

  m_code.fixupShortJumps(jumps.trueJumps ,trueLabel );
  m_code.fixupShortJumps(jumps.falseJumps,falseLabel);
}

#define GENEXPRESSION(n) genExpression(n, dst)
#define GENCALL(n,f)     genCall(n,f,dst);      return
#define GENPOLY(n)       genPolynomial(n, dst); return
#define GENIF(n)         genIf(n,dst);          return

void Expression::throwInvalidTrigonometricMode() {
  throwInvalidArgumentException(_T("genExpression"), _T("Invalid trigonometricMode:%d"), m_trigonometricMode);
}


#define GENTRIGOCALL(n,f)                         \
  switch(getTrigonometricMode()) {                \
  case RADIANS: GENCALL(n, f          );          \
  case DEGREES: GENCALL(n, f##Degrees );          \
  case GRADS  : GENCALL(n, f##Grads   );          \
  default     : throwInvalidTrigonometricMode();  \
  }

void Expression::genExpression(const ExpressionNode *n, const ExpressionDestination &dst) {
  switch(n->getSymbol()) {
  case NAME  :
  case NUMBER:
#ifdef IS32BIT
    m_code.emitFLoad(n);
#else // IS64BIT
    if(m_code.emitFLoad(n, dst)) {
      return;
    }
#endif // IS32BIT
    break;
  case SEMI:
    genStatementList(n);
    break;

  case PLUS  :
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code.emit(FADD);
    break;
#else // !LONGDOUBLE
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code.emitTableOp(FADD_QWORD,n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code.emitTableOp(FADD_QWORD,n->right());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code.emit(FADD);
    }
    break;
#endif // LONGDOUBLE

  case MINUS :
    if(n->isUnaryMinus()) {
      genExpression(n->left(), DST_FPU);
      m_code.emit(FCHS);
      break;
    }
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code.emit(FSUB);
    break;
#else // !LONGDOUBLE
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code.emitTableOp(FSUB_QWORD,n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code.emitTableOp(FSUBR_QWORD,n->left());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code.emit(FSUB);
    }
    break;
#endif // LONGDOUBLE

  case PROD  :
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code.emit(FMUL);
    break;
#else // !LONGDOUBLE
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code.emitTableOp(FMUL_QWORD,n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code.emitTableOp(FMUL_QWORD,n->right());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code.emit(FMUL);
    }
    break;
#endif // LONGDOUBLE

  case QUOT :
#ifdef LONGDOUBLE
    genExpression(n->left() , DST_FPU);
    genExpression(n->right(), DST_FPU);
    m_code.emit(FDIV);
    break;
#else // !LONGDOUBLE
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left(), DST_FPU);
      m_code.emitTableOp(FDIV_QWORD,n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right(), DST_FPU);
      m_code.emitTableOp(FDIVR_QWORD,n->left());
    } else {
      genExpression(n->left() , DST_FPU);
      genExpression(n->right(), DST_FPU);
      m_code.emit(FDIV);
    }
    break;
#endif // LONGDOUBLE

  case MOD           :    GENCALL(     n, fmod                  );
  case POW           :    GENCALL(     n, mypow                 );
  case ROOT          :    GENCALL(     n, root                  );
  case SIN           :    GENTRIGOCALL(n, sin                   );
  case COS           :    GENTRIGOCALL(n, cos                   );
  case TAN           :    GENTRIGOCALL(n, tan                   );
  case COT           :    GENTRIGOCALL(n, cot                   );
  case CSC           :    GENTRIGOCALL(n, csc                   );
  case SEC           :    GENTRIGOCALL(n, sec                   );
  case ASIN          :    GENTRIGOCALL(n, asin                  );
  case ACOS          :    GENTRIGOCALL(n, acos                  );
  case ATAN          :    GENTRIGOCALL(n, atan                  );
  case ATAN2         :    GENTRIGOCALL(n, atan2                 );
  case ACOT          :    GENTRIGOCALL(n, acot                  );
  case ACSC          :    GENTRIGOCALL(n, acsc                  );
  case ASEC          :    GENTRIGOCALL(n, asec                  );
  case COSH          :    GENCALL(     n, cosh                  );
  case SINH          :    GENCALL(     n, sinh                  );
  case TANH          :    GENCALL(     n, tanh                  );
  case ACOSH         :    GENCALL(     n, acosh                 );
  case ASINH         :    GENCALL(     n, asinh                 );
  case ATANH         :    GENCALL(     n, atanh                 );
  case LN            :    GENCALL(     n, log                   );
  case LOG10         :    GENCALL(     n, log10                 );
  case EXP           :    GENCALL(     n, exp                   );
  case SQR           :    GENCALL(     n, sqr                   );
  case SQRT          :    GENCALL(     n, sqrt                  );
  case ABS           :    GENCALL(     n, fabs                  );
  case FLOOR         :    GENCALL(     n, floor                 );
  case CEIL          :    GENCALL(     n, ceil                  );
  case LINCGAMMA     :    GENCALL(     n, lowerIncGamma         );
  case CHI2DENS      :    GENCALL(     n, chiSquaredDensity     );
  case CHI2DIST      :    GENCALL(     n, chiSquaredDistribution);
  case BINOMIAL      :    GENCALL(     n, binomial              );
  case GAMMA         :    GENCALL(     n, gamma                 );
  case GAUSS         :    GENCALL(     n, gauss                 );
  case FAC           :    GENCALL(     n, fac                   );
  case NORM          :    GENCALL(     n, norm                  );
  case PROBIT        :    GENCALL(     n, probitFunction        );
  case ERF           :    GENCALL(     n, errorFunction         );
  case INVERF        :    GENCALL(     n, inverseErrorFunction  );
  case SIGN          :    GENCALL(     n, dsign                 );
  case MAX           :    GENCALL(     n, dmax                  );
  case MIN           :    GENCALL(     n, dmin                  );
  case RAND          :    GENCALL(     n, random                );
  case NORMRAND      :    GENCALL(     n, randomGaussian        );
  case POLY          :    GENPOLY(     n);
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    genIndexedExpression(n);
    break;
  case IIF           :    GENIF(n);
  default            :
    throwUnknownSymbolException(__TFUNCTION__, n);
    break;
  }
#ifdef IS64BIT
// at this point, the result is at the top in FPU-stack. Move result to dst
  switch (dst.getType()) {
  case RESULT_IN_FPU       : // do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    m_code.emit(MEM_ADDR_PTR(FSTP_REAL, RDI));                          // FPU -> *RDI
    break;
  case RESULT_ON_STACK     :
    m_code.emit(MEM_ADDR_ESP1(FSTP_REAL, dst.getStackOffset()));        // FPU -> RSP[offset]
    break;
  case RESULT_IN_VALUETABLE:
    m_code.emitFStorePop(dst.getTableIndex());                          // FPU -> m_valuetable[tableIndex]
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM       :
    m_code.emit(MEM_ADDR_ESP(FSTP_REAL));                               // FPU  -> *RSP
    m_code.emit(MEM_ADDR_ESP(MOVSD_XMM_MMWORD(dst.getXMMReg())));       // *RSP -> XMM0 or XMM1
    break;
#endif // LONGDOUBLE
  }
#endif // IS64BIT
}

void Expression::genIndexedExpression(const ExpressionNode *n) {
  const bool            summation       = n->getSymbol() == INDEXEDSUM;
  const ExpressionNode *startAssignment = n->child(0);
  const ExpressionNode *loopVar         = startAssignment->left();
  const ExpressionNode *endExpr         = n->child(1);
  const ExpressionNode *expr            = n->child(2);

  genExpression(endExpr, DST_FPU);                     // Evaluate end value for loopVar. and keep it in FPU-register
  m_code.emit(summation ? FLDZ : FLD1);                // Initialize accumulator
  genExpression(startAssignment->right(), DST_FPU);    // Evaluate start value for loopVar
  const int loopStart = (int)m_code.size();
  m_code.emit(FCOMI(2));                               // Invariant:loopVar in st(0), endExpr in st(2)
  const int jmpEnd   = m_code.emitShortJmp(JASHORT);   // Jump loopEnd if st(0) > st(2)
  m_code.emitFStorePop(loopVar);                       // Pop st(0) to loopVar
  genExpression(expr, DST_FPU);                        // Accumulator in st(0) (starting at 0 for INDEXEDSUM, 1 for INDEXEDPRODUCT)
  m_code.emit(summation ? FADD : FMUL);                // Update accumulator with st(0)
  m_code.emitFLoad(loopVar);
  m_code.emit(FLD1);
  m_code.emit(FADD);                                   // Increment loopVar
  const int jmpStart = m_code.emitShortJmp(JMPSHORT);  // Jump loopStart
  const int loopEnd  = (int)m_code.size();
  m_code.emit(FSTP(0));                                // Pop loopVar
  m_code.emit(FXCH(1));                                // Result in st(0), end value in st(1). swap these and pop st(0)
  m_code.emit(FSTP(0));                                // Pop end value
  m_code.fixupShortJump(jmpStart,loopStart);
  m_code.fixupShortJump(jmpEnd  ,loopEnd  );
}

void Expression::genIf(const ExpressionNode *n, const ExpressionDestination &dst) {
  JumpList jumps = genBoolExpression(n->child(0));
  m_code.fixupShortJumps(jumps.trueJumps,(int)m_code.size());
  GENEXPRESSION(n->child(1)); // true-expression
  const int trueResultJump  = m_code.emitShortJmp(JMPSHORT);
  m_code.fixupShortJumps(jumps.falseJumps,(int)m_code.size());
  GENEXPRESSION(n->child(2)); // false-expression
  m_code.fixupShortJump(trueResultJump,(int)m_code.size());
}

static ExpressionInputSymbol reverseComparator(ExpressionInputSymbol symbol) {
  switch(symbol) {
  case EQ :
  case NE : return symbol;
  case LE : return GE;
  case LT : return GT;
  case GE : return LE;
  case GT : return LT;
  }
  throwInvalidArgumentException(__TFUNCTION__,_T("symbol=%d"), symbol);
  return EQ;
}

JumpList Expression::genBoolExpression(const ExpressionNode *n) {
//  dumpSyntaxTree(n);
  JumpList result;
  switch(n->getSymbol()) {
  case NOT  :
    { JumpList jumps = genBoolExpression(n->child(0));
      int trueJump   = m_code.emitShortJmp(JMPSHORT);
      result.falseJumps.addAll(jumps.trueJumps);
      result.falseJumps.add(trueJump);
      result.trueJumps.addAll(jumps.falseJumps);
    }
    break;
  case AND  :
    { JumpList jump1 = genBoolExpression(n->left());
      JumpList jump2 = genBoolExpression(n->right());
      m_code.fixupShortJumps(jump1.trueJumps,(int)m_code.size());
      result.falseJumps.addAll(jump1.falseJumps);
      result.falseJumps.addAll(jump2.falseJumps);
      result.trueJumps.addAll(jump2.trueJumps);
    }
    break;
  case OR   :
    { JumpList jump1 = genBoolExpression(n->left());
      int trueJump   = m_code.emitShortJmp(JMPSHORT);
      m_code.fixupShortJumps(jump1.falseJumps,(int)m_code.size());
      JumpList jump2 = genBoolExpression(n->right());
      result.falseJumps.addAll(jump2.falseJumps);
      result.trueJumps.addAll(jump1.trueJumps);
      result.trueJumps.addAll(jump2.trueJumps);
      result.trueJumps.add(trueJump);
    }
    break;
  case EQ   :
  case NE   :
  case LE   :
  case LT   :
  case GE   :
  case GT   :
    { ExpressionInputSymbol symbol = n->getSymbol();
#ifdef LONGDOUBLE
      genExpression(n->right(), DST_FPU);
      genExpression(n->left() , DST_FPU);
      m_code.emit(FCOMPP);
#else // !LONGDOUBLE
      if(n->left()->isNameOrNumber()) {
        genExpression(n->right(), DST_FPU);
        m_code.emitFComparePop(n->left());
        symbol = reverseComparator(symbol);
      } else if(n->right()->isNameOrNumber()) {
        genExpression(n->left(), DST_FPU);
        m_code.emitFComparePop(n->right());
      } else {
        genExpression(n->right(), DST_FPU);
        genExpression(n->left(), DST_FPU);
        m_code.emit(FCOMPP);
      }
#endif // LONGDOUBLE
      m_code.emit(FNSTSW_AX);
      m_code.emit(SAHF);

      switch(symbol) {
      case EQ:
        result.falseJumps.add(m_code.emitShortJmp(JNESHORT));
        break;
      case NE:
        result.falseJumps.add(m_code.emitShortJmp(JESHORT));
        break;
      case LE:
        result.falseJumps.add(m_code.emitShortJmp(JASHORT));
        break;
      case LT:
        result.falseJumps.add(m_code.emitShortJmp(JAESHORT));
        break;
      case GE:
        result.falseJumps.add(m_code.emitShortJmp(JBSHORT));
        break;
      case GT:
        result.falseJumps.add(m_code.emitShortJmp(JBESHORT));
        break;
      }
    }
    break;

  default:
    throwUnknownSymbolException(__TFUNCTION__, n);
  }
  return result;
}


// n is number of coeffients which is degree - 1.
// coef[0] if coefficient for x^(n-1), coef[n-1] is constant term of polynomial
static Real evaluatePolynomial(Real x, int n, const Real *coef) {
  const Real *last   = coef + n;
  Real        result = *coef;
  while(++coef < last) {
    result = result * x + *coef;
  }
  return result;
}


#ifdef IS32BIT

void Expression::genCall(const ExpressionNode *n, BuiltInFunction1 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPush(n->left());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f, dummy);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunction2 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPush(n->right());
  bytesPushed += genPush(n->left());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f, dummy);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef1 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(n->left(),0);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f, dummy);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef2 f, const ExpressionDestination &dummy) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(n->right(),0);
  bytesPushed += genPushRef(n->left() ,1);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f, dummy);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genPolynomial(const ExpressionNode *n, const ExpressionDestination &dummy) {
  const ExpressionNodeArray &coefArray       = n->getCoefficientArray();
  const int                  firstCoefIndex  = n->getFirstCoefIndex();
  for(int i = 0; i < (int)coefArray.size(); i++) {
    const ExpressionNode *coef = coefArray[i];
    if(coef->isConstant()) {
      setValueByIndex(firstCoefIndex + i, evaluateRealExpr(coef));
    } else {
      genExpression(coef, dummy);
      m_code.emitFStorePop(firstCoefIndex + i);
    }
  }

  int bytesPushed = 0;
  bytesPushed += genPushRef(&getValueRef(firstCoefIndex));
  bytesPushed += genPushInt((int)coefArray.size());
  bytesPushed += genPush(n->getArgument());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)::evaluatePolynomial, dummy);
  m_code.emitAddESP(bytesPushed);
}

static int getAlignedSize(int size) {
  const int rest = size % 4;
  return rest ? (size + (4-rest)) : size;
}

int Expression::genPush(const ExpressionNode *n) {
  if(n->isNameOrNumber()) {
    return genPushReal(n->getValueRef());
  } else {
    genExpression(n, DST_FPU);
    int bytesPushed = getAlignedSize(sizeof(Real));
    m_code.emitSubESP(bytesPushed);
    m_code.emit(FSTP_REAL_PTR_ESP);
    return bytesPushed;
  }
}

int Expression::genPushRef(const ExpressionNode *n, int index) {
  if(n->isNameOrNumber()) {
    return genPushRef(&n->getValueRef());
  } else {
    genExpression(n, DST_FPU);
    m_code.emitFStorePop(index);
    return genPushRef(&getValueRef(index));
  }
}

int Expression::genPushReal(const Real &x) {
  return genPush(&x,sizeof(Real));
}

int Expression::genPush(const void *p, UINT size) { // return size rounded up to nearest multiply of 4
  switch(size) {
  case 2:
    m_code.emit(MOV_TO_AX_IMM_ADDR_WORD);
    m_code.addBytes(&p,4);
    m_code.emit(PUSH_R32(EAX));
    return 4;
  case 4:
    m_code.emit(MOV_TO_EAX_IMM_ADDR_DWORD);
    m_code.addBytes(&p,4);
    m_code.emit(PUSH_R32(EAX));
    return 4;
  case 6:
    genPush(((BYTE*)p)+4,4);
    genPush(p,4);
    return 8;
  case 8:
    genPush(((BYTE*)p)+4,4);
    genPush(p,4);
    return 8;
  case 10:
    genPush(((BYTE*)p)+8,4);
    genPush(p,8);
    return 12;
  default:
    size = getAlignedSize(size);
    UINT count = size / 4;
    m_code.emitSubESP(size);
    m_code.emit(MOV_R32_IMM_DWORD(ECX));
    m_code.addBytes(&count,4);
    m_code.emit(MOV_R32_IMM_DWORD(ESI));
    m_code.addBytes(&p,4);
    m_code.emit(REG_SRC(MOV_R32_DWORD(EDI),ESP));
    m_code.emit(REP_MOVS_DWORD);
    return size;
  }
}

int Expression::genPushRef(const void *p) {
  m_code.emit(PUSH_DWORD);
  m_code.addBytes(&p,sizeof(p));
  return sizeof(void*);
}

int Expression::genPushInt(int n) {
  if(ISBYTE(n)) {
    m_code.emit(PUSH_BYTE);
    char byte = (char)n;
    m_code.addBytes(&byte,1);
  } else {
    m_code.emit(PUSH_DWORD);
    m_code.addBytes(&n,4);
  }
  return sizeof(int);
}

int Expression::genPushReturnAddr() {
#ifdef LONGDOUBLE
  return genPushRef(&getValueRef(0));
#else
  return 0;
#endif
}

#else // IS64BIT

static const BYTE int64ParamRegister[4] = {
#ifndef LONGDOUBLE
  RCX ,RDX ,R8 ,R9
#else
  RDX, R8, R9
#endif
};

#ifndef LONGDOUBLE

void Expression::genCall(const ExpressionNode *n, BuiltInFunction1 f, const ExpressionDestination &dst) {
  genSetParameter(n->left(), 0, false);
  m_code.emitCall((BuiltInFunction)f, dst);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunction2 f, const ExpressionDestination &dst) {
  const bool leftHasCalls  = n->left()->containsFunctionCall();
  const bool rightHasCalls = n->right()->containsFunctionCall();
  if (!rightHasCalls) {
    genSetParameter(n->left() , 0, false);
    genSetParameter(n->right(), 1, false);
  } else if (!leftHasCalls) {
    genSetParameter(n->right(), 1, false);
    genSetParameter(n->left() , 0, false);
  } else { // both left and right parmeter are expression using function calls
    const BYTE offset = genSetParameter(n->left(), 0, true);
    genSetParameter(n->right(), 1, false);
    m_code.emit(MEM_ADDR_ESP1(MOVSD_XMM_MMWORD(XMM0), offset));
    m_code.popTmp();
  }
  m_code.emitCall((BuiltInFunction)f, dst);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef1 f, const ExpressionDestination &dst) {
  genSetRefParameter(n->left(), 0);
  m_code.emitCall((BuiltInFunction)f, dst);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef2 f, const ExpressionDestination &dst) {
  const bool rightHasCalls = n->right()->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if (!rightHasCalls) {
    offset1 = genSetRefParameter(n->left() , 0, stacked1);
    offset2 = genSetRefParameter(n->right(), 1, stacked2);
  } else {
    offset2 = genSetRefParameter(n->right(), 1, stacked2);
    offset1 = genSetRefParameter(n->left() , 0, stacked1);
  }
  if (stacked1) {
    m_code.emit(REG_SRC(MOV_R64_QWORD(RCX), RSP));
    m_code.emit(ADD_R64_IMM_BYTE(RCX)); m_code.addBytes(&offset1, 1);
    m_code.popTmp();
  }
  if (stacked2) {
    m_code.emit(REG_SRC(MOV_R64_QWORD(RDX), RSP));
    m_code.emit(ADD_R64_IMM_BYTE(RDX)); m_code.addBytes(&offset2, 1);
    m_code.popTmp();
  }
  m_code.emitCall((BuiltInFunction)f, dst);
}

#else // LONGDOUBLE

void Expression::genCall(const ExpressionNode *n, BuiltInFunction1 f, const ExpressionDestination &dst) {
  genCall(n, (BuiltInFunctionRef1)f, dst);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunction2 f, const ExpressionDestination &dst) {
  genCall(n, (BuiltInFunctionRef2)f, dst);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef1 f, const ExpressionDestination &dst) {
  genSetRefParameter(n->left(), 0);
  m_code.emitCall((BuiltInFunction)f, dst);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef2 f, const ExpressionDestination &dst) {
  const bool rightHasCalls = n->right()->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if (!rightHasCalls) {
    offset1 = genSetRefParameter(n->left() , 0, stacked1);
    offset2 = genSetRefParameter(n->right(), 1, stacked2);
  } else {
    offset2 = genSetRefParameter(n->right(), 1, stacked2);
    offset1 = genSetRefParameter(n->left() , 0, stacked1);
  }
  if (stacked1) {
    m_code.emit(REG_SRC(MOV_R64_QWORD(RDX), RSP));
    if(offset1) {
      m_code.emit(ADD_R64_IMM_BYTE(RDX)); m_code.addBytes(&offset1, 1);
      m_code.popTmp();
    }
  }
  if (stacked2) {
    m_code.emit(REG_SRC(MOV_R64_QWORD(R8), RSP));
    if(offset2) {
      m_code.emit(ADD_R64_IMM_BYTE(R8)); m_code.addBytes(&offset2, 1);
      m_code.popTmp();
    }
  }
  m_code.emitCall((BuiltInFunction)f, dst);
}

#endif // LONGDOUBLE

void Expression::genPolynomial(const ExpressionNode *n, const ExpressionDestination &dst) {
  const ExpressionNodeArray &coefArray       = n->getCoefficientArray();
  const int                  firstCoefIndex  = n->getFirstCoefIndex();
  for(int i = 0; i < (int)coefArray.size(); i++) {
    const ExpressionNode *coef = coefArray[i];
    if(coef->isConstant()) {
      setValueByIndex(firstCoefIndex + i, evaluateRealExpr(coef));
    } else {
      genExpression(coef, DST_INVALUETABLE(firstCoefIndex + i));
    }
  }
#ifndef LONGDOUBLE
  genSetParameter(n->getArgument(), 0, false);
#else
  genSetRefParameter(n->getArgument(), 0);
#endif

  const BYTE    param2    = int64ParamRegister[1];
  const __int64 coefCount = coefArray.size();
  m_code.emit(MOV_R64_IMM_QWORD(param2)); m_code.addBytes(&coefCount, 8);

  const BYTE    param3    = int64ParamRegister[2];
  const Real   *coef0     = &getValueRef(firstCoefIndex);
  m_code.emit(MOV_R64_IMM_QWORD(param3)); m_code.addBytes(&coef0, sizeof(coef0));

  m_code.emitCall((BuiltInFunction)::evaluatePolynomial, dst);
}

void Expression::genSetRefParameter(const ExpressionNode *n, int index) {
  bool stacked;
  const BYTE offset = genSetRefParameter(n, index, stacked);
  if (stacked) {
    const int r64 = int64ParamRegister[index];
    m_code.emit(REG_SRC(MOV_R64_QWORD(r64), RSP));
    m_code.emitAddR64(r64, offset);
    m_code.popTmp();
  }
}

BYTE Expression::genSetRefParameter(const ExpressionNode *n, int index, bool &savedOnStack) {
  const int dstRegister = int64ParamRegister[index];
  if(n->isNameOrNumber()) {
    m_code.emit(REG_SRC(MOV_R64_QWORD(dstRegister), RSI));
    m_code.emitAddR64(dstRegister, n->getValueIndex() * sizeof(Real));
    savedOnStack = false;
    return 0;
  } else {
    const BYTE offset = m_code.pushTmp();
    genExpression(n, DST_ONSTACK(offset));
    savedOnStack = true;
    return offset;
  }
}

#ifndef LONGDOUBLE
BYTE Expression::genSetParameter(const ExpressionNode *n, int index, bool saveOnStack) {
  const int dstRegister = (index == 0) ? XMM0 : XMM1;
  if(n->isNameOrNumber()) {
    assert(saveOnStack == false);
    const int index = n->getValueIndex();
    m_code.emitTableOp(MOVSD_XMM_MMWORD(dstRegister),index);
  } else if (saveOnStack) {
    const BYTE offset = m_code.pushTmp();
    genExpression(n, DST_ONSTACK(offset));
    return offset;
  } else {
    genExpression(n, DST_XMM(dstRegister));
  }
  return 0;
}

#endif // LONGDOUBLE

#endif // IS32BIT
