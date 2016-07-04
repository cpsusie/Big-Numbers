#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/MathFunctions.h>

DEFINECLASSNAME(MachineCode);

MachineCode::MachineCode() {
}

MachineCode::~MachineCode() {
  clear();
}

void MachineCode::clear() {
  ExecutableByteArray::clear();
#ifdef IS64BIT
  clearCoefCache();
#endif // IS64BIT
}

MachineCode::MachineCode(const MachineCode &src) : ExecutableByteArray(src) {
  m_refenceArray = src.m_refenceArray;
#ifdef IS64BIT
  copyCoefCache(src);
#endif
  linkReferences();
}

MachineCode &MachineCode::operator=(const MachineCode &src) {
  ExecutableByteArray::operator=(src);
  m_refenceArray = src.m_refenceArray;
#ifdef IS64BIT
  copyCoefCache(src);
#endif
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

const Real *MachineCode::getValueAddr(const ExpressionNode *n) const {
  DEFINEMETHODNAME;
  switch(n->getSymbol()) {
  case NAME  :
    return &n->getVariable().getValue();
  case NUMBER:
    return ((ExpressionNodeNumber*)n)->getRealAddress();
  default:
    Expression::throwUnknownSymbolException(s_className, method, n);
    return NULL;
  }
}

void MachineCode::addImmediateAddr(const void *addr) {
#ifdef IS32BIT
  addBytes(&addr,sizeof(addr));
#else
  const DWORD v = (DWORD)((BYTE*)addr - (getData() + (size() + sizeof(DWORD))));
  addBytes(&v,sizeof(v));
#endif // IS32BIT
}

void MachineCode::emitImmOp(const IntelInstruction &ins, const void *p) {
  emit(ins);
  addImmediateAddr(p);
}

#define ISBYTE(i)  ((i) >= -128 && (i) < 128)

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
    throwMethodInvalidArgumentException(s_className, method, _T("n must be in range [0..255]"));
  }
}

void MachineCode::emitSubRSP(int n) {
  DEFINEMETHODNAME;
  if(ISBYTE(n)) {
    emit(SUB_R64_IMM_BYTE(RSP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    throwMethodInvalidArgumentException(s_className, method, _T("n must be in range [0..255]"));
  }
}

BYTE MachineCode::pushTmp() {
  const BYTE offset = m_stackTop;
  m_stackTop += 8;
  return offset;
}

BYTE MachineCode::popTmp()  {
  m_stackTop -= 8;
  return m_stackTop;
}

void MachineCode::clearCoefCache() {
  for (size_t i = 0; i < m_coefCache.size(); i++) {
    delete m_coefCache[i];
  }
  m_coefCache.clear();
}

void MachineCode::copyCoefCache(const MachineCode &src) {
  clearCoefCache();
  for (size_t i = 0; i < src.m_coefCache.size(); i++) {
    m_coefCache.add(new CompactDoubleArray(*src.m_coefCache[i]));
  }
}

void MachineCode::addPolyCoefficients(CompactDoubleArray *coefArray) {
  m_coefCache.add(coefArray);
}
#endif // IS32BIT

void MachineCode::emitTestAH(BYTE n) {
  emit(REG_SRC(TEST_IMM_BYTE,AH));
  addBytes(&n,1);
}

void MachineCode::emitTestEAX(unsigned long n) {
  emit(TEST_EAX_IMM_DWORD);
  addBytes(&n,4);
}

int MachineCode::emitShortJmp(const IntelInstruction &ins) {
  emit(ins);
  const BYTE addr = 0;
  return addBytes(&addr,1);
}

void MachineCode::fixupShortJump(int addr, int jmpAddr) {
  const int v = jmpAddr - addr - 1;
  if (!ISBYTE(v)) {
    throwMethodInvalidArgumentException(s_className, _T(__FUNCTION__)
      ,_T("shortJump from %d to %d too long. offset=%d (must be in range [-128,127]")
      ,addr, jmpAddr, v);
  }
  const BYTE pcRelativAddr = (BYTE)v;
  setBytes(addr,&pcRelativAddr,1);
}

void MachineCode::fixupShortJumps(const CompactIntArray &jumps, int jmpAddr) {
  for(size_t i = 0; i < jumps.size(); i++) {
    fixupShortJump(jumps[i],jmpAddr);
  }
}

void MachineCode::fixupMemoryReference(const MemoryReference &ref) {
  const BYTE    *instructionAddr  = getData() + ref.m_byteIndex;
  const intptr_t PCrelativeOffset = ref.m_memAddr - instructionAddr - sizeof(instructionAddr);
  setBytes(ref.m_byteIndex,&PCrelativeOffset,sizeof(PCrelativeOffset));
}

void MachineCode::emitCall(BuiltInFunction p) {
#ifdef IS32BIT
  emit(CALL);
  BuiltInFunction ref = NULL;
  const int addr = addBytes(&ref,4);
  m_refenceArray.add(MemoryReference(addr,(BYTE*)p));
#ifdef LONGDOUBLE
  emit(MEM_ADDR_ESP(MOV_R32_DWORD(EAX)));
  emit(MEM_ADDR_PTR(FLD_TBYTE,EAX));
#endif
#else // 64 Bit
  emit(MOV_R64_IMM_QWORD(RAX));
  addBytes(&p,sizeof(p));
  emit(REG_SRC(CALLABSOLUTE, RAX));
  emit(MEM_ADDR_ESP(MOVSD_MMWORD_XMM(XMM0)));
  emit(MEM_ADDR_ESP(FLD_QWORD));
#endif // IS32BIT
}

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
  void callDoubleResultExpression(ExpressionEntryPoint e, double &result);
  int  callIntResultExpression(ExpressionEntryPoint e);
};
#endif // IS64BIT

Real Expression::fastEvaluate() {

#ifdef IS32BIT
  ExpressionEntryPoint p = m_entryPoint; 
#ifndef LONGDOUBLE

  double result;
  __asm {
    call p
    fstp result;
  }
  return result;

#else // LONGDOUBLE

  #ifndef _DEBUG

    Real result;
    __asm {
      call p
      fstp result;
    }
    return result;

  #else // !_DEBUG

    TenByte result;
    __asm {
      call p
      fstp result;
    }
    return result;

  #endif // _DEBUG

#endif // IONGDOUBLE

#else // !IS32BIT ie 64BIT

#ifndef LONGDOUBLE
  double result;
  callDoubleResultExpression(m_entryPoint, result);
  return result;
#else // LONGDOUBLE
#endif // LONGDOUBLE

#endif // IS32BIT
}

bool Expression::fastEvaluateBool() {
#ifdef IS32BIT
  ExpressionEntryPoint p = m_entryPoint;
  int result;
  __asm {
    call p
    mov result, eax
  }
  return result ? true : false;
#else
  return callIntResultExpression(m_entryPoint) ? true : false;
#endif // IS32BIT
}

ExpressionReturnType Expression::findReturnType() const {
  DEFINEMETHODNAME;
  ExpressionNodeArray stmtList = getStatementList(getRoot());
  switch(stmtList.last()->getSymbol()) {
  case RETURNREAL : return EXPR_RETURN_REAL;
  case RETURNBOOL : return EXPR_RETURN_BOOL;
  default         : throwUnknownSymbolException(method, stmtList.last());
                    return EXPR_RETURN_REAL;
  }
}

void Expression::genCode() {
  if(getTreeForm() != TREEFORM_STANDARD) {
    throwException(_T("Treeform must be STANDARD to generate macinecode. Form=%s"), getTreeFormName().cstr());
  }

  m_code.clear();

#ifdef TEST_MACHINECODE
  m_code.genTestSequence();
  m_machineCode = true;
#else
  genProlog();
  genStatementList(getRoot());
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
    genExpression(n->left());
    genEpilog();
    break;

  case RETURNBOOL:
    genReturnBoolExpression(n);
    break;

  default    :
    throwUnknownSymbolException(_T("genStatementList"), n);
    break;
  }
}

void Expression::genAssignment(const ExpressionNode *n) {
  genExpression(n->right());
  m_code.emitFStorePop(n->left());
}

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

void Expression::genExpression(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NAME  :
  case NUMBER:
    m_code.emitFLoad(n);
    break;

  case SEMI:
    genStatementList(n);
    break;

  case PLUS  :
#ifdef LONGDOUBLE
    genExpression(n->left());
    genExpression(n->right());
    m_code.emit(FADD);
    break;
#else
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right());
      m_code.emitImmOp(MEM_ADDR_DS(FADD_QWORD),n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left());
      m_code.emitImmOp(MEM_ADDR_DS(FADD_QWORD),n->right());
    } else {
      genExpression(n->left());
      genExpression(n->right());
      m_code.emit(FADD);
    }
    break;
#endif

  case MINUS : 
    if(n->isUnaryMinus()) {
      genExpression(n->left());
      m_code.emit(FCHS);
      break;
    }
#ifdef LONGDOUBLE
    genExpression(n->left());
    genExpression(n->right());
    m_code.emit(FSUB);
    break;
#else
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left());
      m_code.emitImmOp(MEM_ADDR_DS(FSUB_QWORD),n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right());
      m_code.emitImmOp(MEM_ADDR_DS(FSUBR_QWORD),n->left());
    } else {
      genExpression(n->left());
      genExpression(n->right());
      m_code.emit(FSUB);
    }
    break;
#endif

  case PROD  :
#ifdef LONGDOUBLE
    genExpression(n->left());
    genExpression(n->right());
    m_code.emit(FMUL);
    break;
#else
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right());
      m_code.emitImmOp(MEM_ADDR_DS(FMUL_QWORD),n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left());
      m_code.emitImmOp(MEM_ADDR_DS(FMUL_QWORD),n->right());
    } else {
      genExpression(n->left());
      genExpression(n->right());
      m_code.emit(FMUL);
    }
    break;
#endif

  case QUOT :
#ifdef LONGDOUBLE
    genExpression(n->left());
    genExpression(n->right());
    m_code.emit(FDIV);
    break;
#else
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left());
      m_code.emitImmOp(MEM_ADDR_DS(FDIV_QWORD),n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right());
      m_code.emitImmOp(MEM_ADDR_DS(FDIVR_QWORD),n->left());
    } else {
      genExpression(n->left());
      genExpression(n->right());
      m_code.emit(FDIV);
    }
    break;
#endif

  case MOD   :
    genCall(n, fmod);
    break;
  case POW   :
    genCall(n, mypow);
    break;
  case ROOT   :
    genCall(n, root);
    break;
  case SIN   :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, sin        ); break;
    case DEGREES: genCall(n, sinDegrees ); break;
    case GRADS  : genCall(n, sinGrads   ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case COS   :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, cos        ); break;
    case DEGREES: genCall(n, cosDegrees ); break;
    case GRADS  : genCall(n, cosGrads   ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case TAN   :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, tan        ); break;
    case DEGREES: genCall(n, tanDegrees ); break;
    case GRADS  : genCall(n, tanGrads   ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case COT   :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, cot        ); break;
    case DEGREES: genCall(n, cotDegrees ); break;
    case GRADS  : genCall(n, cotGrads   ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case CSC   :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, csc        ); break;
    case DEGREES: genCall(n, cscDegrees ); break;
    case GRADS  : genCall(n, cscGrads   ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case SEC   :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, sec        ); break;
    case DEGREES: genCall(n, secDegrees ); break;
    case GRADS  : genCall(n, secGrads   ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case ASIN  :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, asin       ); break;
    case DEGREES: genCall(n, asinDegrees); break;
    case GRADS  : genCall(n, asinGrads  ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case ACOS  :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, acos       ); break;
    case DEGREES: genCall(n, acosDegrees); break;
    case GRADS  : genCall(n, acosGrads  ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case ATAN  :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, atan       ); break;
    case DEGREES: genCall(n, atanDegrees); break;
    case GRADS  : genCall(n, atanGrads  ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case ACOT  :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, acot       ); break;
    case DEGREES: genCall(n, acotDegrees); break;
    case GRADS  : genCall(n, acotGrads  ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case ACSC  :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, acsc       ); break;
    case DEGREES: genCall(n, acscDegrees); break;
    case GRADS  : genCall(n, acscGrads  ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case ASEC  :
    switch(getTrigonometricMode()) {
    case RADIANS: genCall(n, asec       ); break;
    case DEGREES: genCall(n, asecDegrees); break;
    case GRADS  : genCall(n, asecGrads  ); break;
    default     : throwInvalidTrigonometricMode();
    }
    break;
  case COSH  : 
    genCall(n, cosh);
    break;
  case SINH  : 
    genCall(n, sinh);
    break;
  case TANH  : 
    genCall(n, tanh);
    break;
  case ACOSH : 
    genCall(n, acosh);
    break;
  case ASINH : 
    genCall(n, asinh);
    break;
  case ATANH : 
    genCall(n, atanh);
    break;
  case LN    :
    genCall(n, log);
    break;
  case LOG10   :
    genCall(n, log10);
    break;
  case EXP   :
    genCall(n, exp);
    break;
  case SQR   :
    genCall(n, sqr);
    break;
  case SQRT  :
    genCall(n, sqrt);
    break;
  case ABS   :
    genCall(n, fabs);
    break;
  case FLOOR :
    genCall(n, floor);
    break;
  case CEIL  :
    genCall(n, ceil);
    break;
  case BINOMIAL:
    genCall(n, binomial);
    break;
  case GAMMA :
    genCall(n, gamma);
    break;
  case GAUSS :
    genCall(n, gauss);
    break;
  case FAC   : 
    genCall(n, fac);
    break;
  case NORM  :
    genCall(n, norm);
    break;
  case PROBIT:
    genCall(n, probitFunction);
    break;
  case ERF   :
    genCall(n, errorFunction);
    break;
  case INVERF:
    genCall(n, inverseErrorFunction);
    break;
  case SIGN  : 
    genCall(n, dsign);
    break;
  case MAX   : 
    genCall(n, dmax);
    break;
  case MIN   : 
    genCall(n, dmin);
    break;
  case RAND  : 
    genCall(n, random);
    break;
  case NORMRAND:
    genCall(n, randomGaussian);
    break;
  case POLY  :
    genPolynomial(n);
    break;
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    genIndexedExpression(n);
    break;
  case IIF   :
    genIf(n);
    break;
  default    :
    throwUnknownSymbolException(_T("genExpression"), n);
    break;
  }
}

#ifdef IS32BIT
static Real evaluatePolynomial(const Real x, int degree, ...) {
  va_list argptr;
  va_start(argptr,degree);
  
  Real result = va_arg(argptr,Real);
  while(degree--) {
    result = result * x + va_arg(argptr,Real);
  }
  va_end(argptr);
  return result;
}

void Expression::genPolynomial(const ExpressionNode *n) {
  const ExpressionNodeArray &clist = n->getCoefficientArray();
  const ExpressionNode      *x     = n->getArgument();

  int bytesPushed = 0;
  for(int i = (int)clist.size() - 1; i >= 0; i--) { // see parameterlist to evaluatePolynomial
    bytesPushed += genPush(clist[i]);
  }
  bytesPushed += genPushInt((int)clist.size()-1);
  bytesPushed += genPush(x);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)::evaluatePolynomial);
  m_code.emitAddESP(bytesPushed);
}

#else // IS64BIT
static double evaluatePolynomial(double x, const CompactDoubleArray &coef) {
  const double *ak     = &coef[0];
  const double *last   = &coef.last();
  double        result = *ak;
  while(ak++ < last) {
    result = result * x + *ak;
  }
  return result;
}

void Expression::genPolynomial(const ExpressionNode *n) {
  const ExpressionNodeArray &coefNodes = n->getCoefficientArray();
  CompactDoubleArray *coefP = new CompactDoubleArray(coefNodes.size());
  m_code.addPolyCoefficients(coefP);
  CompactDoubleArray &coefValues = *coefP;

  BitSet variableCoefSet(coefNodes.size());
  for (int i = 0; i < coefNodes.size(); i++) {
    const ExpressionNode *cn = coefNodes[i];
    if (cn->isConstant()) {
      coefValues.add(evaluateRealExpr(cn));
    }
    else {
      coefValues.add(0);
      variableCoefSet.add(i);
    }
  }
  for (Iterator<size_t> it = variableCoefSet.getIterator(); it.hasNext();) {
    const size_t i = it.next();
    const ExpressionNode *cn = coefNodes[i];
    genExpression(cn);
    m_code.emitFStorePop(&coefValues[i]);
  }
  genSetParameter(n->getArgument(), 0, false);
  m_code.emit(MOV_R64_IMM_QWORD(RDX)); m_code.addBytes(&coefP,sizeof(coefP));
  m_code.emitCall((BuiltInFunction)::evaluatePolynomial);
}

#endif

void Expression::genIndexedExpression(const ExpressionNode *n) {
  const bool            summation       = n->getSymbol() == INDEXEDSUM;
  const ExpressionNode *startAssignment = n->child(0);
  const ExpressionNode *loopVar         = startAssignment->left();
  const ExpressionNode *endExpr         = n->child(1);
  const ExpressionNode *expr            = n->child(2);
  
  genExpression(endExpr);                              // Evaluate end value for loopVar. and keep it in FPU-register
  m_code.emit(summation ? FLDZ : FLD1);                // Initialize accumulator
  genExpression(startAssignment->right());             // Evaluate start value for loopVar
  const int loopStart = (int)m_code.size();
  m_code.emit(FCOMI(2));                               // Invariant:loopVar in st(0), endExpr in st(2)
  const int jmpEnd   = m_code.emitShortJmp(JASHORT);   // Jump loopEnd if st(0) > st(2)
  m_code.emitFStorePop(loopVar);                       // Pop st(0) to loopVar
  genExpression(expr);                                 // Accumulator in st(0) (starting at 0 for INDEXEDSUM, 1 for INDEXEDPRODUCT)
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

#ifdef IS32BIT
void Expression::genCall(const ExpressionNode *n, BuiltInFunction1 f) {
  int bytesPushed = 0;
  bytesPushed += genPush(n->left());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunction2 f) {
  int bytesPushed = 0;
  bytesPushed += genPush(n->right());
  bytesPushed += genPush(n->left());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef1 f) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(n->left(),0);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f);
  m_code.emitAddESP(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef2 f) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(n->right(),0);
  bytesPushed += genPushRef(n->left() ,1);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((BuiltInFunction)f);
  m_code.emitAddESP(bytesPushed);
}
#else // IS64BIT

void Expression::genCall(const ExpressionNode *n, BuiltInFunction1 f) {
  genSetParameter(n->left(), 0, false);
  m_code.emitCall((BuiltInFunction)f);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunction2 f) {
  const bool leftHasCalls  = n->left()->containsFunctionCall();
  const bool rightHasCalls = n->right()->containsFunctionCall();
  if (!rightHasCalls) {
    genSetParameter(n->left(), 0, false);
    genSetParameter(n->right(), 1, false);
  } else if (!leftHasCalls) {
    genSetParameter(n->right(), 1, false);
    genSetParameter(n->left(), 0, false);
  } else { // both left and right parmeter are expression using function calls
    const BYTE offset = genSetParameter(n->left(), 0, true);
    genSetParameter(n->right(), 1, false);
    m_code.emit(MEM_ADDR_ESP1(MOVSD_XMM_MMWORD(XMM0), offset));
    m_code.popTmp();
  }
  m_code.emitCall((BuiltInFunction)f);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef1 f) {
  bool stacked;
  const BYTE offset = genSetRefParameter(n->left(), 0, stacked);
  if (stacked) {
    m_code.emit(REG_SRC(MOV_R64_QWORD(RCX), RSP));
    m_code.emit(ADD_R64_IMM_BYTE(RCX)); m_code.addBytes(&offset, 1);
    m_code.popTmp();
  }
  m_code.emitCall((BuiltInFunction)f);
}

void Expression::genCall(const ExpressionNode *n, BuiltInFunctionRef2 f) {
  const bool rightHasCalls = n->right()->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if (!rightHasCalls) {
    offset1 = genSetRefParameter(n->left(), 0, stacked1);
    offset2 = genSetRefParameter(n->right(), 1, stacked2);
  } else {
    offset2 = genSetRefParameter(n->right(), 1, stacked2);
    offset1 = genSetRefParameter(n->left(), 0, stacked1);
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
  m_code.emitCall((BuiltInFunction)f);
}
#endif // IS32BIT

void Expression::genIf(const ExpressionNode *n) {
  JumpList jumps = genBoolExpression(n->child(0));
  m_code.fixupShortJumps(jumps.trueJumps,(int)m_code.size());
  genExpression(n->child(1)); // true-expression
  const int trueResultJump  = m_code.emitShortJmp(JMPSHORT);
  m_code.fixupShortJumps(jumps.falseJumps,(int)m_code.size());
  genExpression(n->child(2)); // false-expression
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
  throwException(_T("reverseComparator:Unknown relational operator:%d"), symbol);
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
      genExpression(n->left());
      genExpression(n->right());
      m_code.emit(FCOMPP);
#else
      if(n->left()->isNameOrNumber()) {
        genExpression(n->right());
        m_code.emitFCompare(n->left());
      } else if(n->right()->isNameOrNumber()) {
        genExpression(n->left());
        m_code.emitFCompare(n->right());
        symbol = reverseComparator(symbol);
      } else {
        genExpression(n->left());
        genExpression(n->right());
        m_code.emit(FCOMPP);
      }
#endif
      m_code.emit(FNSTSW_AX);
      switch(symbol) {
      case EQ:
        m_code.emitTestAH(0x40);
        result.falseJumps.add(m_code.emitShortJmp(JESHORT));
        break;
      case NE: 
        m_code.emitTestAH(0x40);
        result.falseJumps.add(m_code.emitShortJmp(JNESHORT));
        break;
      case LE: 
        m_code.emitTestAH(0x01);
        result.falseJumps.add(m_code.emitShortJmp(JNESHORT));
        break;
      case LT: 
        m_code.emitTestAH(0x41);
        result.falseJumps.add(m_code.emitShortJmp(JNESHORT));
        break;
      case GE: 
        m_code.emitTestAH(0x41);
        result.falseJumps.add(m_code.emitShortJmp(JESHORT));
        break;
      case GT: 
        m_code.emitTestAH(0x01);
        result.falseJumps.add(m_code.emitShortJmp(JESHORT));
        break;
      }
    }
    break;

  default:
    throwUnknownSymbolException(_T("genBoolExpression"), n);
  }
  return result;
}

static int getAlignedSize(int size) {
  const int rest = size % 4;
  return rest ? (size + (4-rest)) : size;
}

#ifdef IS32BIT
int Expression::genPush(const ExpressionNode *n) {
  if(n->isNameOrNumber()) {
    return genPushReal(*m_code.getValueAddr(n));
  } else {
    genExpression(n);
#ifdef _DEBUG
    Real &tmpVar = m_code.getTmpVar(0);
    m_code.emitFStorePop(&tmpVar);
    return genPushReal(tmpVar);
#else
    int bytesPushed = getAlignedSize(sizeof(Real));
    m_code.emitSubESP(bytesPushed);
    m_code.emit(FSTP_REAL_PTR_ESP);
    return bytesPushed;
#endif
  }
}

int Expression::genPushRef(const ExpressionNode *n, int index) {
  if(n->isNameOrNumber()) {
    return genPushRef(m_code.getValueAddr(n));
  } else {
    genExpression(n);
    Real &tmpVar = m_code.getTmpVar(index);
    m_code.emitFStorePop(&tmpVar);
    return genPushRef(&tmpVar);
  }
}

int Expression::genPushReal(const Real &x) {
  return genPush(&x,sizeof(Real));
}

int Expression::genPush(const void *p, unsigned int size) { // return size rounded up to nearest multiply of 4
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
    genPush(((char*)p)+4,4);
    genPush(p,4);
    return 8;
  case 8:
    genPush(((char*)p)+4,4);
    genPush(p,4);
    return 8;
  case 10:
    genPush(((char*)p)+8,4);
    genPush(p,8);
    return 12;
  default:
    size = getAlignedSize(size);
    unsigned int count = size / 4;
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
  m_code.emit(MOV_R32_IMM_DWORD(EAX));
  m_code.addBytes(&p,sizeof(p));
  m_code.emit(PUSH_R32(EAX));
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
  return genPushRef(&m_code.getTmpVar(2));
#else
  return 0;
#endif
}

#else // IS64BIT

BYTE Expression::genSetParameter(const ExpressionNode *n, int index, bool saveOnStack) {
  const int dstRegister = (index == 0) ? XMM0 : XMM1;
  if(n->isNameOrNumber()) {
    assert(saveOnStack == false);
    const double *addr = m_code.getValueAddr(n);
    m_code.emit(MEM_ADDR_DS(MOVSD_XMM_MMWORD(dstRegister)));
    m_code.addImmediateAddr(addr);
  } else {
    if (saveOnStack) {
      const BYTE offset = m_code.pushTmp();
      genExpression(n);
      m_code.emit(MEM_ADDR_ESP1(FSTP_QWORD, offset));
      return offset;
    }
    else {
      genExpression(n);
      m_code.emit(MEM_ADDR_ESP(FSTP_QWORD));
      m_code.emit(MEM_ADDR_ESP(MOVSD_XMM_MMWORD(dstRegister)));
    }
  }
  return 0;
}

BYTE Expression::genSetRefParameter(const ExpressionNode *n, int index, bool &savedOnStack) {
  const int dstRegister = (index == 0) ? RCX : RDX;
  if(n->isNameOrNumber()) {
    const double *addr = m_code.getValueAddr(n);
    m_code.emit(MOV_R64_IMM_QWORD(dstRegister));
    m_code.addBytes(&addr,sizeof(addr));
    savedOnStack = false;
    return 0;
  } else {
    const BYTE offset = m_code.pushTmp();
    genExpression(n);
    m_code.emit(MEM_ADDR_ESP1(FSTP_QWORD, offset));
    savedOnStack = true;
    return offset;
  }
}

#endif // IS32BIT
