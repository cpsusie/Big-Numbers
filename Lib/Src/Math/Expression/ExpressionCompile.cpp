#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/MathFunctions.h>

DEFINECLASSNAME(MachineCode);

MachineCode::MachineCode() {
}

MachineCode::~MachineCode() {
}

MachineCode::MachineCode(const MachineCode &src) : ExecutableByteArray(src) {
  m_externals = src.m_externals;
  linkExternals();
}

MachineCode &MachineCode::operator=(const MachineCode &src) {
  ByteArray::operator=(src);
  m_externals = src.m_externals;
  linkExternals();
  return *this;
}

int MachineCode::addBytes(const void *bytes, int count) {
  const int ret = size();
  append((BYTE*)bytes,count);
  return ret;
}

void MachineCode::setBytes(int addr, const void *bytes, int count) {
  for(BYTE *p = (BYTE*)bytes; count--;) {
    (*this)[addr++] = *(p++);
  }
}

int MachineCode::emit(const IntelInstruction &ins) {
  const int pos = size();
  append((BYTE*)&ins.m_bytes, ins.m_size);
  return pos;
}

const Real *MachineCode::getValueAddr(const ExpressionNode *n) const {
  DEFINEMETHODNAME(getValueAddr);
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

void MachineCode::emitBinaryOp(const IntelInstruction &ins, const void *p) {
  emit(ins);
  addBytes(&p,sizeof(p));
}

#define ISBYTE(i)  ((i) >= -128 && (i) < 128)

void MachineCode::emitAddEsp(int n) {
  if(ISBYTE(n)) {
    emit(ADD_R32_IMM_BYTE(ESP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    emit(ADD_R32_IMM_DWORD(ESP));
    addBytes(&n,4);
  }
}

void MachineCode::emitSubEsp(int n) {
  if(ISBYTE(n)) {
    emit(SUB_R32_IMM_BYTE(ESP));
    const char byte = n;
    addBytes(&byte,1);
  } else {
    emit(SUB_R32_IMM_DWORD(ESP));
    addBytes(&n,4);
  }
}

void MachineCode::emitTestAH(BYTE n) {
  emit(REG_SRC(TEST_IMM_BYTE,AH));
  addBytes(&n,1);
}

void MachineCode::emitTestEAX(unsigned long n) {
  emit(TEST_EAX_IMM_DWORD);
  addBytes(&n,4);
}
/*
void MachineCode::emitTestSP(unsigned short n) {
  emit3(TEST_SP);
  addBytes(&n,2);
}
*/
int MachineCode::emitShortJmp(const IntelInstruction &ins) {
  emit(ins);
  const BYTE addr = 0;
  return addBytes(&addr,1);
}

void MachineCode::fixupShortJump(int addr, BYTE jmpAddr) {
  const BYTE pcRelativAddr = jmpAddr - addr - 1;
  setBytes(addr,&pcRelativAddr,1);
}

void MachineCode::fixupShortJumps(const CompactIntArray &jumps, int jmpAddr) {
  for(int i = 0; i < jumps.size(); i++) {
    fixupShortJump(jumps[i],jmpAddr);
  }
}

void MachineCode::fixupCall(const ExternalReference &ref) {
  const BYTE *instructionAddr = getData() + ref.m_addr;
  const int PCrelativOffset = (const BYTE*)ref.m_p - instructionAddr - 4;
  setBytes(ref.m_addr,&PCrelativOffset,4);
}

void MachineCode::emitCall(function p) {
  emit(CALL);
  function ref = NULL;
  const int addr = addBytes(&ref,4);
  m_externals.add(ExternalReference(addr,p));
#ifdef LONGDOUBLE
  emit(MEM_ADDR_ESP(MOV_R32_DWORD(EAX)));
  emit(MEM_ADDR_PTR(FLD_TBYTE,EAX));
#endif
}

void MachineCode::linkExternals() {
  for(int i = 0; i < m_externals.size(); i++) {
    fixupCall(m_externals[i]);
  }
}

#pragma warning(disable:4717)
void Expression::compile(const String &expr, bool machineCode) {
  compile(expr.cstr(),machineCode);
}

void Expression::compile(const char *expr, bool machineCode) {
  parse(expr);
  if(!isOk()) {
    return;
  }
  
  setReturnType(findReturnType());

  if(machineCode) {
    genCode();
  }
}

Real Expression::fastEvaluate() {
  function p = m_code.getEntryPoint();

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
}

bool Expression::fastEvaluateBool() {
  function p = m_code.getEntryPoint();
  int result;
  __asm {
    call p
    mov result, eax
  }
  return result ? true : false;
}

ExpressionReturnType Expression::findReturnType() const {
  DEFINEMETHODNAME(findReturnType);
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
  genStatementList(getRoot());
  m_code.linkExternals();
  m_machineCode = true;
#endif

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
    m_code.emit(RET);
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
      m_code.emitBinaryOp(MEM_ADDR_DS(FADD_QWORD),n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left());
      m_code.emitBinaryOp(MEM_ADDR_DS(FADD_QWORD),n->right());
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
      m_code.emitBinaryOp(MEM_ADDR_DS(FSUB_QWORD),n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right());
      m_code.emitBinaryOp(MEM_ADDR_DS(FSUBR_QWORD),n->left());
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
      m_code.emitBinaryOp(MEM_ADDR_DS(FMUL_QWORD),n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left());
      m_code.emitBinaryOp(MEM_ADDR_DS(FMUL_QWORD),n->right());
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
      m_code.emitBinaryOp(MEM_ADDR_DS(FDIV_QWORD),n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right());
      m_code.emitBinaryOp(MEM_ADDR_DS(FDIVR_QWORD),n->left());
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
  for(int i = clist.size() - 1; i >= 0; i--) { // see parameterlist to evaluatePolynomial
    bytesPushed += genPush(clist[i]);
  }
  bytesPushed += genPushInt(clist.size()-1);
  bytesPushed += genPush(x);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((function)::evaluatePolynomial);
  m_code.emitAddEsp(bytesPushed);
}

void Expression::genIndexedExpression(const ExpressionNode *n) {
  const bool            summation       = n->getSymbol() == INDEXEDSUM;
  const ExpressionNode *startAssignment = n->child(0);
  const ExpressionNode *loopVar         = startAssignment->left();
  const ExpressionNode *endExpr         = n->child(1);
  const ExpressionNode *expr            = n->child(2);
  
  genExpression(endExpr);                              // Evaluate end value for loopVar. and keep it in FPU-register
  m_code.emit(summation ? FLDZ : FLD1);               // Initialize accumulator
  genExpression(startAssignment->right());             // Evaluate start value for loopVar
  const int loopStart = m_code.size();
  m_code.emit(FCOMI(2));                              // Invariant:loopVar in st(0), endExpr in st(2)
  const int jmpEnd   = m_code.emitShortJmp(JASHORT);   // Jump loopEnd if st(0) > st(2)
  m_code.emitFStorePop(loopVar);                       // Pop st(0) to loopVar
  genExpression(expr);                                 // Accumulator in st(0) (starting at 0 for INDEXEDSUM, 1 for INDEXEDPRODUCT)
  m_code.emit(summation ? FADD : FMUL);               // Update accumulator with st(0)
  m_code.emitFLoad(loopVar);
  m_code.emit(FLD1);
  m_code.emit(FADD);                                  // Increment loopVar
  const int jmpStart = m_code.emitShortJmp(JMPSHORT);  // Jump loopStart
  const int loopEnd  = m_code.size();
  m_code.emit(FSTP(0));                               // Pop loopVar
  m_code.emit(FXCH(1));                               // Result in st(0), end value in st(1). swap these and pop st(0)
  m_code.emit(FSTP(0));                               // Pop end value
  m_code.fixupShortJump(jmpStart,loopStart);
  m_code.fixupShortJump(jmpEnd  ,loopEnd  );
}

void Expression::genCall(const ExpressionNode *n, function1 f) {
  int bytesPushed = 0;
  bytesPushed += genPush(n->left());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((function)f);
  m_code.emitAddEsp(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, function2 f) {
  int bytesPushed = 0;
  bytesPushed += genPush(n->right());
  bytesPushed += genPush(n->left());
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((function)f);
  m_code.emitAddEsp(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, functionRef1 f) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(n->left(),0);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((function)f);
  m_code.emitAddEsp(bytesPushed);
}

void Expression::genCall(const ExpressionNode *n, functionRef2 f) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(n->right(),0);
  bytesPushed += genPushRef(n->left() ,1);
  bytesPushed += genPushReturnAddr();
  m_code.emitCall((function)f);
  m_code.emitAddEsp(bytesPushed);
}

void Expression::genIf(const ExpressionNode *n) {
  JumpList jumps = genBoolExpression(n->child(0));
  m_code.fixupShortJumps(jumps.trueJumps,m_code.size());
  genExpression(n->child(1)); // true-expression
  const int trueResultJump  = m_code.emitShortJmp(JMPSHORT);
  m_code.fixupShortJumps(jumps.falseJumps,m_code.size());
  genExpression(n->child(2)); // false-expression
  m_code.fixupShortJump(trueResultJump,m_code.size());
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

void Expression::genReturnBoolExpression(const ExpressionNode *n) {
  JumpList jumps = genBoolExpression(n->left());
  int trueValue = 1;
  const int trueLabel  = m_code.emit(MOV_R32_IMM_DWORD(EAX));
  m_code.addBytes(&trueValue,4);
  m_code.emit(RET);

  const int falseLabel = m_code.emit(REG_SRC(XOR_R32_DWORD(EAX),EAX));
  m_code.emit(RET);

  m_code.fixupShortJumps(jumps.trueJumps ,trueLabel );
  m_code.fixupShortJumps(jumps.falseJumps,falseLabel);
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
      m_code.fixupShortJumps(jump1.trueJumps,m_code.size());
      result.falseJumps.addAll(jump1.falseJumps);
      result.falseJumps.addAll(jump2.falseJumps);
      result.trueJumps.addAll(jump2.trueJumps);
    }
    break;
  case OR   :
    { JumpList jump1 = genBoolExpression(n->left());
      int trueJump   = m_code.emitShortJmp(JMPSHORT);
      m_code.fixupShortJumps(jump1.falseJumps,m_code.size());
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
    m_code.emitSubEsp(bytesPushed);
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
    m_code.emitSubEsp(size);
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
  m_code.addBytes(&p,4);
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
