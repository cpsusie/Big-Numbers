#pragma once

#include <Math/Expression/ParserTree.h>
#include "CodeGeneration.h"

namespace Expr {

class CodeGenerator {
private:
  ParserTree     &m_tree;
  MachineCode    *m_codeArray;
  CodeGeneration *m_code;
  LabelGenerator  m_labelGen;

  inline TrigonometricMode getTrigonometricMode() const {
    return m_tree.getTrigonometricMode();
  }

  inline ParserTree &getTree() {
    return m_tree;
  }
  inline MemoryRef getTableRef(SNode n) {
    return m_code->getTableRef(n.getValueIndex());
  }

  // Code generation (compile to machinecode)
  void genMachineCode();
  ExpressionReturnType genStatementList(const ExpressionNode *n);
  void genReturnBoolExpression(SNode n);
  void genExpression(          SNode n DCL_DSTPARAM);
  void genFLD(                 SNode n);
  void genReciprocal();
  void genCall1Arg(            SNode arg             , BuiltInFunction1    f, const String &name DCL_DSTPARAM);
  void genCall1Arg(            SNode arg             , BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM);
  void genCall2Arg(            SNode arg1, SNode arg2, BuiltInFunction2    f, const String &name DCL_DSTPARAM);
  void genCall2Arg(            SNode arg1, SNode arg2, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM);

#ifdef IS64BIT
  bool genFLoad(               SNode n   DCL_DSTPARAM);
#ifdef LONGDOUBLE
#define ALLARGS_BYREF
#endif
#endif

  void genCall(SNode n, BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM) {
    genCall1Arg(n.left(), f, name DST_PARAM);
  }
  void genCall(SNode n, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM) {
    genCall2Arg(n.left(), n.right(), f, name DST_PARAM);
  }
#ifndef ALLARGS_BYREF
  void genCall(SNode n, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
    genCall1Arg(n.left(), f, name DST_PARAM);
  }
  void genCall(SNode n, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
    genCall2Arg(n.left(), n.right(), f, name DST_PARAM);
  }
#else // ALLARGS_BYREF
  void genCall(SNode n, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
    genCall(n, (BuiltInFunctionRef1)f, name DST_PARAM);
  }
  void genCall(SNode n, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
    genCall(n, (BuiltInFunctionRef2)f, name DST_PARAM);
  }
#endif // ALLARGS_BYREF

  void     genPolynomial(       SNode n DCL_DSTPARAM);
  void     genIf(               SNode n DCL_DSTPARAM);
  void     genPowMultSequence(  UINT y);
  void     genSqrtSequence(     UINT r);
  void     genCall(             const FunctionCall  &fc DCL_DSTPARAM);
  inline void genFPUOpVal(const OpcodeFPUArithm &op, SNode n) {
    m_code->emitFPUOpMem(op, RealPtr(getTableRef(n)));
  }
  inline void genFPUOpVal(const OpcodeFPUCompare &op, SNode n) {
    m_code->emitFPUOpMem(op, RealPtr(getTableRef(n)));
  }

#ifdef IS32BIT
  int      genPush(             SNode n);
  int      genPushRef(          SNode n, int index);
  int      genPushReal(         const Real           &x);
  int      genPushValue(        SNode n); // assume n is name/number
  int      genPushReturnAddr();
  int      genPushInt(int n);
  int      genPush(             const void           *p, UINT size); // return size
  int      genPushRef(          const void           *p);
#else // IS64BIT
  void     genSetParameter(     SNode n, int index);
  void     genSetRefParameter(  SNode n, int index);
  BYTE     genSetRefParameter(  SNode n, int index, bool &savedOnStack);
#endif // IS64BIT

  void     genAssignment(       SNode n);
  void     genIndexedExpression(SNode n);
  void     genBoolExpression(   SNode n, JumpList &jumpList, bool trueAtEnd);
  void throwInvalidTrigonometricMode();

public:
  // Allocates a pointer to MachineCode with new, and generate instructions from parserTree to
  // be executed. The allocated pointer can be retrieved by getCode() and should be released with
  // delete after usage. No exceptions should be thrown, as syntax-checks has been done, when
  // building the parsertree/symbol-table, but in case of an internal error, m_code will
  // automaticly be deleted.
  CodeGenerator(ParserTree *tree, FILE *listFile = NULL);
  const MachineCode *getCode() const {
    return m_codeArray;
  }
};

}; // namespace Expr
