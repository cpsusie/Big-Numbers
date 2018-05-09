#pragma once

#include "CodeGeneration.h"
#include <Math/Expression/ParserTree.h>

class CodeGenerator {
private:
  ParserTree             &m_tree;
  MachineCode            *m_codeArray;
  CodeGeneration         *m_code;
  LabelGenerator          m_labelGen;

  inline TrigonometricMode getTrigonometricMode() const {
    return m_tree.getTrigonometricMode();
  }

  inline MemoryRef getTableRef(const ExpressionNode *n) {
    return m_code->getTableRef(n->getValueIndex());
  }

  // Code generation (compile to machinecode)
  void genMachineCode();
  void genStatementList(       const ExpressionNode *n);
  void genReturnBoolExpression(const ExpressionNode *n);
  void genExpression(          const ExpressionNode *n DCL_DSTPARAM);
  void genFLD(                 const ExpressionNode *n);
  void genReciprocal();
  void genCall1Arg(            const ExpressionNode *arg                             , BuiltInFunction1    f, const String &name DCL_DSTPARAM);
  void genCall1Arg(            const ExpressionNode *arg                             , BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM);
  void genCall2Arg(            const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunction2    f, const String &name DCL_DSTPARAM);
  void genCall2Arg(            const ExpressionNode *arg1, const ExpressionNode *arg2, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM);

#ifdef IS64BIT
  bool genFLoad(               const ExpressionNode *n   DCL_DSTPARAM);
#ifdef LONGDOUBLE
#define ALLARGS_BYREF
#endif
#endif

  void genCall(const ExpressionNode *n, BuiltInFunctionRef1 f, const String &name DCL_DSTPARAM) {
    genCall1Arg(n->left(), f, name DST_PARAM);
  }
  void genCall(const ExpressionNode *n, BuiltInFunctionRef2 f, const String &name DCL_DSTPARAM) {
    genCall2Arg(n->left(), n->right(), f, name DST_PARAM);
  }
#ifndef ALLARGS_BYREF
  void genCall(const ExpressionNode *n, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
    genCall1Arg(n->left(), f, name DST_PARAM);
  }
  void genCall(const ExpressionNode *n, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
    genCall2Arg(n->left(), n->right(), f, name DST_PARAM);
  }
#else // ALLARGS_BYREF
  void genCall(const ExpressionNode *n, BuiltInFunction1 f, const String &name DCL_DSTPARAM) {
    genCall(n, (BuiltInFunctionRef1)f, name DST_PARAM);
  }
  void genCall(const ExpressionNode *n, BuiltInFunction2 f, const String &name DCL_DSTPARAM) {
    genCall(n, (BuiltInFunctionRef2)f, name DST_PARAM);
  }
#endif // ALLARGS_BYREF

  void     genPolynomial(       const ExpressionNode *n DCL_DSTPARAM);
  void     genIf(               const ExpressionNode *n DCL_DSTPARAM);
  void     genPowMultSequence(  UINT y);
  void     genCall(             const FunctionCall  &fc DCL_DSTPARAM);
  inline void genFPUOpVal(const OpcodeBase &op, const ExpressionNode *n) {
    m_code->emit(op, RealPtr(getTableRef(n)));
  }

#ifdef IS32BIT
  int      genPush(             const ExpressionNode *n);
  int      genPushRef(          const ExpressionNode *n, int index);
  int      genPushReal(         const Real           &x);
  int      genPushValue(        const ExpressionNode *n); // assume n is name/number
  int      genPushReturnAddr();
  int      genPushInt(int n);
  int      genPush(             const void           *p, UINT size); // return size
  int      genPushRef(          const void           *p);
#else // IS64BIT
  void     genSetParameter(     const ExpressionNode *n, int index);
  void     genSetRefParameter(  const ExpressionNode *n, int index);
  BYTE     genSetRefParameter(  const ExpressionNode *n, int index, bool &savedOnStack);
#endif // IS64BIT

  void     genAssignment(       const ExpressionNode *n);
  void     genIndexedExpression(const ExpressionNode *n);
  void     genBoolExpression(   const ExpressionNode *n, JumpList &jumpList, bool trueAtEnd);
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
