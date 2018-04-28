#include "pch.h"
#include "ExpressionCompile.h"

// --------------------------------- CodeGenerator -----------------------------------

CodeGenerator::CodeGenerator(ParserTree *tree, TrigonometricMode trigonometricMode, FILE *listFile)
  : m_tree(*tree)
  , m_trigonometricMode(trigonometricMode)
{
  if(tree->getTreeForm() != TREEFORM_STANDARD) {
    throwException(_T("Treeform must be STANDARD to generate machinecode. Form=%s"), m_tree.getTreeFormName().cstr());
  }
  m_codeArray = new MachineCode;                                   TRACE_NEW(m_codeArray);
  m_code     = new CodeGeneration(*m_codeArray, *tree, listFile); TRACE_NEW(m_code)
  try {
    genMachineCode();
    SAFEDELETE(m_code);
  } catch (...) {
    SAFEDELETE(m_code);
    SAFEDELETE(m_codeArray)
    throw;
  }
}

void CodeGenerator::genMachineCode() {
  genProlog();
  genStatementList(m_tree.getRoot());
  m_code->finalize();
#ifdef IS64BIT
  if(m_code->isCallsGenerated()) {
    assert(m_hasCalls);
    m_code->fixupLoadRBX();
  }
#endif
}

#ifdef DEBUG_JUMPLIST
#define LISTJUMPLIST(jl)                  \
{ if(m_code->hasListFile()) {             \
     m_code->list(_T("%-20s(%d):%s\n")    \
                 ,__TFUNCTION__,__LINE__  \
                 ,jl.toString().cstr());  \
  }                                       \
}
#else DEBUG_JUMPLIST
#define LISTJUMPLIST(jl)
#endif // DEBUG_JUMPLIST

// At entry in x64-mode, RCX contains address of the first instruction
void CodeGenerator::genProlog() {
#ifdef IS64BIT
#define LOCALSTACKSPACE   72
#define RESERVESTACKSPACE 40
  m_hasCalls = m_tree.getRoot()->containsFunctionCall();
  m_code->resetStack(RESERVESTACKSPACE);
  if(m_hasCalls) {
    m_code->emit(PUSH,RBX);
    // generate a dummy-instruction, which will be adjusted
    // with the right offset (4-bytes) when code is generated.
    // After the instructions, there will be an array of QWORD's
    // with absolute addresses of the called functions
    m_code->emitLoadRBX();
    m_code->emitSubStack(LOCALSTACKSPACE + RESERVESTACKSPACE); // to get 16-byte aligned RSP
  }
#endif
}

void CodeGenerator::genEpilog() {
#ifdef IS64BIT
  if(m_hasCalls) {
    m_code->emitAddStack(LOCALSTACKSPACE + RESERVESTACKSPACE);
    m_code->emit(POP,RBX);
  }
#endif
  m_code->emit(RET);
}

void CodeGenerator::genStatementList(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case SEMI      :
    genStatementList(n->left());
    genStatementList(n->right());
    break;
  case ASSIGN    :
    genAssignment(n);
    break;
  case RETURNREAL:
    genExpression(n->left() DST_ADDRRDI);
    genEpilog();
    break;
  case RETURNBOOL:
    genReturnBoolExpression(n);
    break;
  default    :
    ParserTree::throwUnknownSymbolException(__TFUNCTION__, n);
    break;
  }
}

#ifdef IS32BIT
void CodeGenerator::genAssignment(const ExpressionNode *n) {
  genExpression(n->right() DST_FPU);
  m_code->emitFSTP(getTableRef(n->left()));
}
#else // IS64BIT
void CodeGenerator::genAssignment(const ExpressionNode *n) {
  genExpression(n->right() DST_INVALUETABLE(n->left()->getValueIndex()));
}
#endif IS32BIT

void CodeGenerator::genReturnBoolExpression(const ExpressionNode *n) {
  JumpList jumps(m_labelGen.nextLabelPair());
  genBoolExpression(n->left(),jumps, true);
  m_code->fixupJumps(jumps,true );
  m_code->emit(MOV,EAX,1  );
  genEpilog();
  m_code->fixupJumps(jumps,false);
  m_code->emit(XOR,EAX,EAX);
  genEpilog();
}

// Generate multiplication-sequence to calculate st(0) = st(0)^y
// using st(0), and if neccessary st(1), by logarithmic powering
// Assume st(0) = x contains value to raise to the power y
void CodeGenerator::genPowMultSequence(UINT y) {
  UINT p2 = 1;
  for(UINT t = y; t && ((t&1)==0); t >>= 1) {
    p2 <<= 1;
    m_code->emit(FMUL,ST0,ST0);  // ST(0) *= ST(0)
  }
  // ST(0) = x^p2
  if(p2 < y) {
    m_code->emit(FLD,ST0); // ST(0) = x^p2; ST(1) = x^p2
    UINT reg = 0, count = 0;
    // Calculate the sequence of multiplications of st(1), st(0) needed
    // to make st(0) = x^(y-p2) and st(1) = x^p2
    for(y -= p2; y > p2; count++) {
      reg <<= 1;
      if(y&p2) { y-=p2; reg |= 1; } else y >>= 1;
    }
    for(;count--; reg >>= 1) {
      m_code->emit(FMUL,ST0,ST(reg&1));
    }
    m_code->emit(FMUL); // finally st(1) *= st(0), pop st(0)
  }
}

#define GENEXPRESSION(n) genExpression(n          DST_PARAM)
#define GENCALL(n,f)     genCall(      n,f,_T(#f) DST_PARAM); return
#define GENCALLARG(n,f)  genCall1Arg(  n,f,_T(#f) DST_PARAM); return
#define GENPOLY(n)       genPolynomial(n          DST_PARAM); return
#define GENIF(n)         genIf(n                  DST_PARAM); return

void CodeGenerator::throwInvalidTrigonometricMode() {
  throwInvalidArgumentException(_T("genExpression"), _T("Invalid trigonometricMode:%d"), m_trigonometricMode);
}

#define GENTRIGOCALL(n,f)                         \
  switch(getTrigonometricMode()) {                \
  case RADIANS: GENCALL(n, f          );          \
  case DEGREES: GENCALL(n, f##Degrees );          \
  case GRADS  : GENCALL(n, f##Grads   );          \
  default     : throwInvalidTrigonometricMode();  \
  }

void CodeGenerator::genExpression(const ExpressionNode *n DCL_DSTPARAM) {
  switch(n->getSymbol()) {
  case NAME  :
  case NUMBER:
#ifdef IS32BIT
    m_code->emitFLD(n);
#else // IS64BIT
    if(m_code->emitFLoad(n DST_PARAM)) {
      return;
    }
#endif // IS64BIT
    break;
  case SEMI:
    genStatementList(n);
    break;

  case PLUS  :
#ifdef LONGDOUBLE
    genExpression(n->left()  DST_FPU);
    genExpression(n->right() DST_FPU);
    m_code->emit(FADD);
    break;
#else // !LONGDOUBLE
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right() DST_FPU);
      m_code->emitFPUOpVal(FADD,n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left() DST_FPU);
      m_code->emitFPUOpVal(FADD,n->right());
    } else {
      genExpression(n->left()  DST_FPU);
      genExpression(n->right() DST_FPU);
      m_code->emit(FADD);
    }
    break;
#endif // LONGDOUBLE

  case MINUS :
    if(n->isUnaryMinus()) {
      genExpression(n->left() DST_FPU);
      m_code->emit(FCHS);
      break;
    }
#ifdef LONGDOUBLE
    genExpression(n->left()  DST_FPU);
    genExpression(n->right() DST_FPU);
    m_code->emit(FSUB);
    break;
#else // !LONGDOUBLE
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left() DST_FPU);
      m_code->emitFPUOpVal(FSUB,n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right() DST_FPU);
      m_code->emitFPUOpVal(FSUBR,n->left());
    } else {
      genExpression(n->left()  DST_FPU);
      genExpression(n->right() DST_FPU);
      m_code->emit(FSUB);
    }
    break;
#endif // LONGDOUBLE

  case PROD  :
#ifdef LONGDOUBLE
    genExpression(n->left()  DST_FPU);
    genExpression(n->right() DST_FPU);
    m_code->emit(FMUL);
    break;
#else // !LONGDOUBLE
    if(n->left()->isNameOrNumber()) {
      genExpression(n->right() DST_FPU);
      m_code->emitFPUOpVal(FMUL,n->left());
    } else if(n->right()->isNameOrNumber()) {
      genExpression(n->left() DST_FPU);
      m_code->emitFPUOpVal(FMUL,n->right());
    } else {
      genExpression(n->left()  DST_FPU);
      genExpression(n->right() DST_FPU);
      m_code->emit(FMUL);
    }
    break;
#endif // LONGDOUBLE

  case QUOT :
#ifdef LONGDOUBLE
    genExpression(n->left()  DST_FPU);
    genExpression(n->right() DST_FPU);
    m_code->emit(FDIV);
    break;
#else // !LONGDOUBLE
    if(n->right()->isNameOrNumber()) {
      genExpression(n->left() DST_FPU);
      m_code->emitFPUOpVal(FDIV,n->right());
    } else if(n->left()->isNameOrNumber()) {
      genExpression(n->right() DST_FPU);
      m_code->emitFPUOpVal(FDIVR,n->left());
    } else {
      genExpression(n->left()  DST_FPU);
      genExpression(n->right() DST_FPU);
      m_code->emit(FDIV);
    }
    break;
#endif // LONGDOUBLE

  case MOD           :    GENCALL(     n, fmod                  );
  case POW           :
    if(n->left()->isEulersConstant()) {
      GENCALLARG(  n->right(), exp                   );
    } else if(n->left()->isTwo()) {
      GENCALLARG(  n->right(), exp2                  );
    } else if (n->left()->isTen()) {
      GENCALLARG(  n->right(), exp10                 );
    }
    if(n->right()->isConstant()) {
      const Real p = m_tree.evaluateRealExpr(n->right());
      if((fabs(p) <= 64) && (p - floor(p) == 0)) {
        const int y = getInt(p);
        if(y == 0) {
          genExpression(m_tree.getOne() DST_PARAM);
          return;
        } else if(y == 1) {
          genExpression(n->left() DST_PARAM);
          return;
        } else {
          genExpression(n->left() DST_FPU);
          genPowMultSequence(abs(y));
          if(y < 0) {                    // make st0 = 1/st0
            m_code->emit(FLD1);          // st0=1 , st1=x^|y|
            m_code->emit(FDIVRP,ST1);    // st1=st0/st1; pop st0 => st0=x^y
          }
        }
        break;
      }
    }
    GENCALL(     n, mypow                 );
  case SQR           :
    genExpression(n->left() DST_FPU);
    m_code->emit(FMUL,ST0,ST0);            // st0=x^2
    break;
  case SQRT          :
    genExpression(n->left() DST_FPU);
    m_code->emit(FSQRT);                   // st0=sqrt(st0)
    break;
  case ABS           :
    genExpression(n->left() DST_FPU);
    m_code->emit(FABS);                    // st0=|st0|
    break;
  case ATAN          :                     // atan(left)
    if(getTrigonometricMode() == RADIANS) {
      genExpression(n->left() DST_FPU);
      m_code->emit(FLD1);                  // st0=1, st1=left
      m_code->emit(FPATAN);                // st1=atan(st1/st0); pop st0
      break;
    }
    GENTRIGOCALL(n, atan                  );
  case ATAN2         :                     // atan2(left,right)
    if(getTrigonometricMode() == RADIANS) {
      genExpression(n->left()  DST_FPU);   // st0=left
      genExpression(n->right() DST_FPU);   // st0=right, st1=left
      m_code->emit(FPATAN);                // st1=atan(st1/st0); pop st0
      break;
    }
    GENTRIGOCALL(n, atan2                 );

  case ROOT          :    GENCALL(     n, root                  );
  case SIN           :    GENTRIGOCALL(n, sin                   );
  case COS           :    GENTRIGOCALL(n, cos                   );
  case TAN           :    GENTRIGOCALL(n, tan                   );
  case COT           :    GENTRIGOCALL(n, cot                   );
  case CSC           :    GENTRIGOCALL(n, csc                   );
  case SEC           :    GENTRIGOCALL(n, sec                   );
  case ASIN          :    GENTRIGOCALL(n, asin                  );
  case ACOS          :    GENTRIGOCALL(n, acos                  );
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
  case LOG2          :    GENCALL(     n, log2                  );
  case EXP           :    GENCALL(     n, exp                   );
  case EXP10         :    GENCALL(     n, exp10                 );
  case EXP2          :    GENCALL(     n, exp2                  );
  case FLOOR         :    GENCALL(     n, floor                 );
  case CEIL          :    GENCALL(     n, ceil                  );
  case HYPOT         :    GENCALL(     n, hypot                 );
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
  case RAND          :    GENCALL(     n, randReal              );
  case NORMRAND      :    GENCALL(     n, randomGaussian        );
  case POLY          :    GENPOLY(     n);
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    genIndexedExpression(n);
    break;
  case IIF           :    GENIF(n);
  default            :
    ParserTree::throwUnknownSymbolException(__TFUNCTION__, n);
    break;
  }
#ifdef IS64BIT
// At this point, the result is at the top in FPU-stack. Move result to dst
  switch(dst.getType()) {
  case RESULT_IN_FPU       : // do nothing
    break;
  case RESULT_IN_ADDRRDI   :
    m_code->emitFSTP(RDI);                                                   // FPU -> *RDI
    break;
  case RESULT_ON_STACK     :
    m_code->emitFSTP(m_code->getStackRef(dst.getStackOffset()));             // FPU -> RSP[offset]
    break;
  case RESULT_IN_VALUETABLE:
    m_code->emitFSTP(m_code->getTableRef(dst.getTableIndex()));              // FPU -> m_valuetable[tableIndex]
    break;
#ifndef LONGDOUBLE
  case RESULT_IN_XMM       :
    m_code->emitFSTP(m_code->getStackRef(0));                                // FPU  -> *RSP
    m_code->emitMemToXMM(dst.getXMMReg(),m_code->getStackRef(0));            // *RSP -> XMM0 or XMM1
    break;
#endif // LONGDOUBLE
  }
#endif // IS64BIT
}

void CodeGenerator::genIndexedExpression(const ExpressionNode *n) {
  const bool            summation       = n->getSymbol() == INDEXEDSUM;
  const ExpressionNode *startAssignment = n->child(0);
  const ExpressionNode *loopVar         = startAssignment->left();
  const ExpressionNode *endExpr         = n->child(1);
  const ExpressionNode *expr            = n->child(2);

  genExpression(endExpr DST_FPU);                       // Evaluate end value for loopVar. and keep it in FPU-register
  m_code->emit(summation ? FLDZ : FLD1);                // Initialize accumulator
  genExpression(startAssignment->right() DST_FPU);      // Evaluate start value for loopVar
  const int loopStart = (int)m_code->size();
  const CodeLabel startLabel = m_labelGen.nextLabel();
  m_code->listLabel(startLabel);
  m_code->emit(FCOMI,ST2);                              // Invariant:loopVar in st(0), endExpr in st(2)
  const CodeLabel endLabel = m_labelGen.nextLabel();
  const int jmpEnd   = m_code->emitJmp(JA,endLabel);    // Jump loopEnd if st(0) > st(2)
  m_code->emitFSTP(getTableRef(loopVar));               // Pop st(0) to loopVar
  genExpression(expr DST_FPU);                          // Accumulator in st(0) (starting at 0 for INDEXEDSUM, 1 for INDEXEDPRODUCT)
  m_code->emit(summation ? FADD : FMUL);                // Update accumulator with st(0)
  m_code->emitFLD(loopVar);
  m_code->emit(FLD1);
  m_code->emit(FADD);                                   // Increment loopVar
  const int jmpStart = m_code->emitJmp(JMP,startLabel); // Jump loopStart
  const int loopEnd  = (int)m_code->size();
  m_code->listLabel(endLabel);
  m_code->emit(FSTP,ST0);                               // Pop loopVar
  m_code->emit(FXCH,ST1);                               // Result in st(0), end value in st(1). swap these and pop st(0)
  m_code->emit(FSTP,ST0);                               // Pop end value
  m_code->fixupJump(jmpStart,loopStart);
  m_code->fixupJump(jmpEnd  ,loopEnd  );
}

void CodeGenerator::genIf(const ExpressionNode *n DCL_DSTPARAM) {
  JumpList jumps(m_labelGen.nextLabelPair());
  genBoolExpression(n->child(0),jumps,true);
  const CodeLabel endLabel = m_labelGen.nextLabel();
  m_code->fixupJumps(jumps,true);
  GENEXPRESSION(n->child(1));           // "true"-expr
  const int firstResultJump  = m_code->emitJmp(JMP,endLabel);
  m_code->fixupJumps(jumps,false);
  GENEXPRESSION(n->child(2));           // "false"-expr
  m_code->fixupJump(firstResultJump);
  m_code->listLabel(endLabel);
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

// Generates code and updates jl, so that the generated code will end with a
// conditional jump, which will do a jump only if evaluteBool(n) != trueAtEnd
// It's up to the caller of the function to fixup the jump's generated.
// This applies recursively through and/or and not operators
void CodeGenerator::genBoolExpression(const ExpressionNode *n, JumpList &jl, bool trueAtEnd) {
//  dumpSyntaxTree(n);
  switch(n->getSymbol()) {
  case SYMNOT:
    { JumpList jumps(!jl);
      genBoolExpression(n->child(0), jumps, !trueAtEnd);
      jl ^= jumps;
    }
    break;
  case SYMAND:
    if(!trueAtEnd) { // De Morgan's law
      JumpList jumps(!jl);
      genBoolExpression(!SNode((ExpressionNode*)n->left()) || !SNode((ExpressionNode*)n->right()), jumps, !trueAtEnd);
      jl ^= jumps;
    } else {
      genBoolExpression(n->left() , jl, trueAtEnd);
      genBoolExpression(n->right(), jl, trueAtEnd);
    }
    break;
  case SYMOR   :
    if(!trueAtEnd) { // De Morgan's law
      JumpList jumps(!jl);
      genBoolExpression(!SNode((ExpressionNode*)n->left()) && !SNode((ExpressionNode*)n->right()), jumps, !trueAtEnd);
      jl ^= jumps;
    } else {
      JumpList jump1(m_labelGen.nextLabel(),jl.m_trueLabel);
      genBoolExpression(n->left(), jump1, !trueAtEnd);
      m_code->fixupJumps(jump1,!trueAtEnd);
      genBoolExpression(n->right(), jl, trueAtEnd);
      jl.getJumps(trueAtEnd).addAll(jump1.getJumps(trueAtEnd));
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
      genExpression(n->right() DST_FPU);
      genExpression(n->left()  DST_FPU);
      m_code->emit(FCOMPP);
#else // !LONGDOUBLE
      if(n->left()->isNameOrNumber()) {
        genExpression(n->right() DST_FPU);
        m_code->emitFPUOpVal(FCOMP,n->left());
        symbol = reverseComparator(symbol);
      } else if(n->right()->isNameOrNumber()) {
        genExpression(n->left() DST_FPU);
        m_code->emitFPUOpVal(FCOMP,n->right());
      } else {
        genExpression(n->right() DST_FPU);
        genExpression(n->left() DST_FPU);
        m_code->emit(FCOMPP);
      }
#endif // LONGDOUBLE
      m_code->emit(FNSTSWAX);
      m_code->emit(SAHF);

      switch(symbol) {
      case EQ:
        if(trueAtEnd) {
          jl.m_falseJumps.add(m_code->emitJmp(JNE,jl.m_falseLabel));
        } else {
          jl.m_trueJumps.add( m_code->emitJmp(JE ,jl.m_trueLabel ));
        }
        break;
      case NE:
        if(trueAtEnd) {
          jl.m_falseJumps.add(m_code->emitJmp(JE ,jl.m_falseLabel));
        } else {
          jl.m_trueJumps.add( m_code->emitJmp(JNE,jl.m_trueLabel ));
        }
        break;
      case LE:
        if(trueAtEnd) {
          jl.m_falseJumps.add(m_code->emitJmp(JA ,jl.m_falseLabel));
        } else {
          jl.m_trueJumps.add( m_code->emitJmp(JBE,jl.m_trueLabel ));
        }
        break;
      case GT:
        if(trueAtEnd) {
          jl.m_falseJumps.add(m_code->emitJmp(JBE,jl.m_falseLabel));
        } else {
          jl.m_trueJumps.add( m_code->emitJmp(JA ,jl.m_trueLabel ));
        }
        break;
      case GE:
        if(trueAtEnd) {
          jl.m_falseJumps.add(m_code->emitJmp(JB ,jl.m_falseLabel));
        } else {
          jl.m_trueJumps.add( m_code->emitJmp(JAE,jl.m_trueLabel ));
        }
        break;
      case LT:
        if(trueAtEnd) {
          jl.m_falseJumps.add(m_code->emitJmp(JAE,jl.m_falseLabel));
        } else {
          jl.m_trueJumps.add( m_code->emitJmp(JB ,jl.m_trueLabel ));
        }
        break;
      }
    }
    break;
  default:
    ParserTree::throwUnknownSymbolException(__TFUNCTION__, n);
  }
}

static const TCHAR *polyName         = _T("poly");
static const TCHAR *polySignatureStr = _T("(Real x, int n, const Real *coef)");

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

void CodeGenerator::genCall(const FunctionCall &fc DCL_DSTPARAM) {
#ifdef IS64BIT
  assert(m_hasCalls);
#endif
  m_code->emitCall(fc DST_PARAM);

#ifdef IS32BIT
#ifdef LONGDOUBLE
  m_code->emitFLD(m_code->getTableRef(0)); // this is the address which was pushed for returnvalue
#endif // LONGDOUBLE
#endif // IS32BIT

}

#ifdef IS32BIT

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunction1 f, const String &name) {
  int bytesPushed = 0;
  bytesPushed += genPush(arg);
  bytesPushed += genPushReturnAddr();
  genCall(FunctionCall(f,name));
  m_code->emitAddStack(bytesPushed);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2 f, const String &name) {
  int bytesPushed = 0;
  bytesPushed += genPush(arg2);
  bytesPushed += genPush(arg1);
  bytesPushed += genPushReturnAddr();
  genCall(FunctionCall(f,name));
  m_code->emitAddStack(bytesPushed);
}

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunctionRef1 f, const String &name) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(arg,0);
  bytesPushed += genPushReturnAddr();
  genCall(FunctionCall(f,name));
  m_code->emitAddStack(bytesPushed);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const String &name) {
  int bytesPushed = 0;
  bytesPushed += genPushRef(arg2,0);
  bytesPushed += genPushRef(arg1,1);
  bytesPushed += genPushReturnAddr();
  genCall(FunctionCall(f,name));
  m_code->emitAddStack(bytesPushed);
}

void CodeGenerator::genPolynomial(const ExpressionNode *n) {
  const ExpressionNodeArray &coefArray       = n->getCoefficientArray();
  const int                  firstCoefIndex  = n->getFirstCoefIndex();
  for(int i = 0; i < (int)coefArray.size(); i++) {
    const ExpressionNode *coef = coefArray[i];
    if(coef->isConstant()) {
      m_tree.getValueRef(firstCoefIndex + i) = m_tree.evaluateRealExpr(coef);
    } else {
      genExpression(coef);
      m_code->emitFSTP(m_code->getTableRef(firstCoefIndex + i));
    }
  }

  int bytesPushed = 0;
  bytesPushed += genPushRef(&m_tree.getValueRef(firstCoefIndex));
  bytesPushed += genPushInt((int)coefArray.size());
  bytesPushed += genPush(n->getArgument());
  bytesPushed += genPushReturnAddr();
  genCall(FunctionCall((BuiltInFunction)::evaluatePolynomial, polyName, polySignatureStr));
  m_code->emitAddStack(bytesPushed);
}

static int getAlignedSize(int size) {
  const int rest = size % 4;
  return rest ? (size + (4-rest)) : size;
}

int CodeGenerator::genPush(const ExpressionNode *n) {
  if(n->isNameOrNumber()) {
    return genPushValue(n);
//    return genPushReal(n->getValueRef());
  } else {
    genExpression(n DST_FPU);
    int bytesPushed = getAlignedSize(sizeof(Real));
    m_code->emitSubStack(bytesPushed);
    m_code->emitFSTP(m_code->getStackRef(0));
    return bytesPushed;
  }
}

int CodeGenerator::genPushRef(const ExpressionNode *n, int index) {
  if(n->isNameOrNumber()) {
    return genPushRef(&n->getValueRef());
  } else {
    genExpression(n DST_FPU);
    m_code->emitFSTP(m_code->getTableRef(index));
    return genPushRef(&m_tree.getValueRef(index));
  }
}

int CodeGenerator::genPushValue(const ExpressionNode *n) {
  int bytesPushed = 0;
  const MemoryRef mem1 = m_code->getTableRef(n->getValueIndex());
  const MemoryRef mem2 = MemoryRef(TABLEREF_REG + (mem1.getOffset()+sizeof(DWORD)));

#ifdef LONGDOUBLE
  const MemoryRef mem3 = MemoryRef(TABLEREF_REG + (mem2.getOffset()+sizeof(DWORD)));
  m_code->emit(PUSH, DWORDPtr(mem3)); // reverse sequence, because stack grows downwards
  bytesPushed += sizeof(DWORD);
#endif

  m_code->emit(PUSH, DWORDPtr(mem2)); // reverse sequence, because stack grows downwards
  bytesPushed += sizeof(DWORD);
  m_code->emit(PUSH, DWORDPtr(mem1));
  bytesPushed += sizeof(DWORD);
  return bytesPushed;
}

int CodeGenerator::genPushReal(const Real &x) {
  return genPush(&x,sizeof(Real));
}

// return size rounded up to nearest multiply of 4
int CodeGenerator::genPush(const void *p, UINT size) {
  switch(size) {
  case 2:
    m_code->emit(PUSH,WORDPtr((intptr_t)p));
    return 4;
  case 4:
    m_code->emit(PUSH,DWORDPtr((intptr_t)p));
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
    const UINT count = size / 4;
    m_code->emitSubStack(size   );
    m_code->emit(MOV,ECX,count);
    m_code->emit(MOV,ESI,(intptr_t)p);
    m_code->emit(MOV,EDI,ESP  );
    m_code->emit(REP,MOVSD    );
    return size;
  }
}

int CodeGenerator::genPushRef(const void *p) {
  m_code->emit(PUSH,(intptr_t)p);
  return sizeof(void*);
}

int CodeGenerator::genPushInt(int n) {
  m_code->emit(PUSH,n);
  return sizeof(int);
}

int CodeGenerator::genPushReturnAddr() {
#ifdef LONGDOUBLE
  return genPushRef(&m_tree.getValueRef(0));
#else
  return 0;
#endif
}

#else // IS64BIT

static const IndexRegister int64ParamRegister[] = {
#ifndef LONGDOUBLE
  RCX ,RDX ,R8 ,R9
#else
  RDX, R8, R9
#endif
};

#ifndef LONGDOUBLE

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
  genSetParameter(arg, 0);
  genCall(FunctionCall(f, name) DST_PARAM);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
  const bool arg1HasCalls = arg1->containsFunctionCall();
  const bool arg2HasCalls = arg2->containsFunctionCall();
  if(!arg2HasCalls) {
    genSetParameter(arg1, 0);
    genSetParameter(arg2, 1);
  } else if (!arg1HasCalls) {
    genSetParameter(arg2, 1);
    genSetParameter(arg1, 0);
  } else { // both parmeters are expressions using function calls
    const BYTE offset = m_code->pushTmp();
    genExpression(arg1 DST_ONSTACK(offset));
    genSetParameter(arg2, 1);
    m_code->emitMemToXMM(XMM0, m_code->getStackRef(offset));
    m_code->popTmp();
  }
  genCall(FunctionCall(f, name) DST_PARAM);
}

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM) {
  genSetRefParameter(arg, 0);
  genCall(FunctionCall(f, name) DST_PARAM);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM) {
  const bool arg2HasCalls = arg2->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if(!arg2HasCalls) {
    offset1 = genSetRefParameter(arg1, 0, stacked1);
    offset2 = genSetRefParameter(arg2, 1, stacked2);
  } else {
    offset2 = genSetRefParameter(arg2, 1, stacked2);
    offset1 = genSetRefParameter(arg1, 0, stacked1);
  }
  if(stacked1) {
    m_code->emitLoadAddr(RCX, m_code->getStackRef(offset1));
    m_code->popTmp();
  }
  if(stacked2) {
    m_code->emitLoadAddr(RDX, m_code->getStackRef(offset2));
    m_code->popTmp();
  }
  genCall(FunctionCall(f, name), dst);
}

#else // LONGDOUBLE

void CodeGenerator::genCall1Arg(const ExpressionNode *arg, BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM) {
  genSetRefParameter(arg, 0);
  genCall(FunctionCall(f, name), dst);
}

void CodeGenerator::genCall2Arg(const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM) {
  const bool arg2HasCalls = arg2->containsFunctionCall();

  bool stacked1, stacked2;
  BYTE offset1, offset2;

  if(!arg2HasCalls) {
    offset1 = genSetRefParameter(arg1, 0, stacked1);
    offset2 = genSetRefParameter(arg2, 1, stacked2);
  } else {
    offset2 = genSetRefParameter(arg2, 1, stacked2);
    offset1 = genSetRefParameter(arg1, 0, stacked1);
  }
  if(stacked1) {
    m_code->emitLoadAddr(RDX,m_code->getStackRef(offset1));
    if(offset1) {
      m_code->popTmp();
    }
  }
  if(stacked2) {
    m_code->emitLoadAddr(R8,m_code->getStackRef(offset2));
    if(offset2) {
      m_code->popTmp();
    }
  }
  genCall(FunctionCall(f, name), dst);
}

#endif // LONGDOUBLE

void CodeGenerator::genPolynomial(const ExpressionNode *n DCL_DSTPARAM) {
  const ExpressionNodeArray &coefArray       = n->getCoefficientArray();
  const int                  firstCoefIndex  = n->getFirstCoefIndex();
  for(int i = 0; i < (int)coefArray.size(); i++) {
    const ExpressionNode *coef = coefArray[i];
    if(coef->isConstant()) {
      m_tree.getValueRef(firstCoefIndex + i) = m_tree.evaluateRealExpr(coef);
    } else {
      genExpression(coef DST_INVALUETABLE(firstCoefIndex + i));
    }
  }
#ifndef LONGDOUBLE
  genSetParameter(n->getArgument(), 0);
#else
  genSetRefParameter(n->getArgument(), 0);
#endif // LONGDOUBLE

  const IndexRegister &param2    = int64ParamRegister[1];
  const size_t         coefCount = coefArray.size();
  m_code->emit(MOV,param2,coefCount);

  const IndexRegister &param3    = int64ParamRegister[2];
  m_code->emitLoadAddr(param3, m_code->getTableRef(firstCoefIndex));
  genCall(FunctionCall((BuiltInFunction)::evaluatePolynomial, polyName, polySignatureStr) DST_PARAM);
}

void CodeGenerator::genSetRefParameter(const ExpressionNode *n, int index) {
  bool stacked;
  const BYTE offset = genSetRefParameter(n, index, stacked);
  if(stacked) {
    m_code->emitLoadAddr(int64ParamRegister[index], m_code->getStackRef(offset));
    m_code->popTmp();
  }
}

BYTE CodeGenerator::genSetRefParameter(const ExpressionNode *n, int index, bool &savedOnStack) {
  if(n->isNameOrNumber()) {
    m_code->emitLoadAddr(int64ParamRegister[index], getTableRef(n));
    savedOnStack = false;
    return 0;
  } else {
    const BYTE offset = m_code->pushTmp();
    genExpression(n DST_ONSTACK(offset));
    savedOnStack = true;
    return offset;
  }
}

#ifndef LONGDOUBLE
void CodeGenerator::genSetParameter(const ExpressionNode *n, int index) {
  const XMMRegister &dstRegister = (index == 0) ? XMM0 : XMM1;
  if(n->isNameOrNumber()) {
    m_code->emitMemToXMM(dstRegister,getTableRef(n));
  } else {
    genExpression(n DST_XMM(dstRegister));
  }
}

#endif // LONGDOUBLE

#endif // IS64BIT
