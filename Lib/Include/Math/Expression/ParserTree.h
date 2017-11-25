#pragma once

#include <MyUtil.h>
#include <Stack.h>
#include <HashMap.h>
#include <TreeMap.h>
#include <PropertyContainer.h>
#include <Scanner.h>
#include "ExpressionSymbol.h"
#include "ExpressionNode.h"

#ifdef _DEBUG
#define TRACE_REDUCTION_CALLSTACK
#endif

typedef enum {
  EXPR_STATE                 // ExpressionState
 ,EXPR_REDUCEITERATION       // UINT
 ,EXPR_TRIGONOMETRICMODE     // TrigonometricMode
 ,EXPR_RETURNTYPE            // ExpressionReturnType
 ,EXPR_ROOT                  // ExpressionNode
 ,EXPR_OK                    // bool
 ,EXPR_TREEFORM
 ,REDUCTION_STACKHIGHT
} ExpressionProperties;

typedef enum {
  TREEFORM_STANDARD
 ,TREEFORM_CANONICAL
 ,TREEFORM_NUMERIC
} ParserTreeForm;

class ParserTreeComplexity {
private:
  int m_nodeCount;
  int m_nameCount;
  int m_treeDepth;
public:
  ParserTreeComplexity(ParserTree &tree);
  friend int parserTreeComplexityCmp(const ParserTreeComplexity &rs1, const ParserTreeComplexity &rs2);
  inline bool operator==(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) == 0;
  }
  inline bool operator!=(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) != 0;
  }
  inline bool operator< (const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) <  0;
  }
  inline bool operator<=(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) <= 0;
  }
  inline bool operator> (const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) >  0;
  }
  inline bool operator>=(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) >= 0;
  }
  String toString() const {
    return format(_T("#nodes:%3d, #names:%2d, treedepth:%d\n"), m_nodeCount, m_nameCount, m_treeDepth);
  }
};

#ifdef TRACE_REDUCTION_CALLSTACK
class ReductionStackElement {
public:
  const TCHAR          *m_method;
  String                m_str;
  const ExpressionNode *m_node;
  ReductionStackElement(const TCHAR *method, const String &str, const ExpressionNode *node = NULL) : m_method(method), m_node(node) {
    m_str = format(_T("%-20s:%s"), m_method, str.cstr());
  }
  inline const String &toString() const {
    return m_str;
  }
};
typedef Stack<ReductionStackElement> ReductionStack;
#endif

class ParserTree : public PropertyContainer {

#ifdef TRACE_REDUCTION_CALLSTACK
protected:
  mutable ReductionStack m_reductionStack;
  void pushReductionMethod(const TCHAR *method, const String &s, const ExpressionNode *n = NULL) const;
  void popReductionMethod(const TCHAR *method) const;
  void resetReductionStack();
#define STARTREDUCTION()    resetReductionStack()
#define ENTERMETHOD()       pushReductionMethod(method, format(_T("n:<%s>"), n.toString().cstr()),n)
#define ENTERMETHOD1(v)     pushReductionMethod(method, format(_T("%s:<%s>"), _T(#v), (v).toString().cstr()))
#define ENTERMETHOD2(v1,v2) pushReductionMethod(method, format(_T("%s:<%s>, %s:<%s>"), _T(#v1), (v1).toString().cstr(), _T(#v2), (v2).toString().cstr()))
#define LEAVEMETHOD()       popReductionMethod(method)

#define RETURN(x) { LEAVEMETHOD(); return x; }

#define RETURNNULL                                                                         \
{ pushReductionMethod(method, _T("Return NULL"));                                          \
  m_reductionStack.pop();                                                                  \
  RETURN(NULL);                                                                            \
}

#define RETURNBOOL(b)                                                                      \
{ const bool _b = b;                                                                       \
  pushReductionMethod(method, format(method, _T("Return %s"), boolToStr(_b)));             \
  m_reductionStack.pop();                                                                  \
  RETURN(_b);                                                                              \
}

#define RETURNNODE(n)                                                                      \
{ const SNode &_n = n;                                                                     \
  pushReductionMethod(method, format(_T("Reduced:<%s>"), _n.toString().cstr()),_n);        \
  m_reductionStack.pop();                                                                  \
  RETURN(_n);                                                                              \
}

#define RETURNSHOWSTR(v)                                                                   \
{ const String _s = (v).toString();                                                        \
  pushReductionMethod(method, format(_T("%s:<%s>"), _T(#v), _s.cstr()));                   \
  m_reductionStack.pop();                                                                  \
  RETURN(v);                                                                               \
}

#define RETURNPSHOWSTR(p)                                                                  \
{ const String _s = (p)->toString();                                                       \
  pushReductionMethod(method, format(_T("%s:<%s>"), _T(#p), _s.cstr()));                   \
  m_reductionStack.pop();                                                                  \
  RETURN(p);                                                                               \
}

#else

#define STARTREDUCTION()
#define ENTERMETHOD()
#define ENTERMETHOD1(v)
#define ENTERMETHOD2(v1,v2)
#define LEAVEMETHOD()

#define RETURN(x)         return x
#define RETURNNULL        return NULL
#define RETURNBOOL(b)     return b
#define RETURNNODE(n)     return n
#define RETURNSHOWSTR(v)  return v
#define RETURNPSHOWSTR(p) return p

#endif

private:
  DECLARECLASSNAME;
  ExpressionNode               *m_root;
  int                           m_indexNameCounter;
  // map name -> index in m_variableTable
  StringTreeMap<int>            m_nameTable;
  Array<ExpressionVariable>     m_variableTable;
  mutable CompactRealArray      m_valueTable;
  CompactArray<ExpressionNode*> m_nodeTable;
  CompactArray<SumElement*>     m_addentTable;
  StringArray                   m_errors;
  bool                          m_ok;
  ParserTreeForm                m_treeForm;
  ExpressionNodeNumber         *m_minusOne, *m_zero, *m_one, *m_two, *m_ten, *m_half;

  ExpressionVariable   *allocateSymbol(     const String &name, const Real &value, bool isConstant, bool isLeftSide, bool isLoopVar);
  ExpressionVariable   *allocateSymbol(     ExpressionNode *n                    , bool isConstant, bool isLeftSide, bool isLoopVar);
  ExpressionVariable   *allocateConstant(   ExpressionNode *n, const String &name, const Real &value);
  void                  allocateNumber(     ExpressionNode *n, bool reuseIfExist);
  // Insert value into m_valueTable, return index of position
  int                   insertValue(Real value);
  // Find i, so m_valueTable[i] == value, and m_valueTable[i] is not used by a vaiable. return -1, if not found
  int                   findNumberIndexByValue(const Real &value) const;
  // Return set with indices in m_valueTable, for all elements usedby variables
  BitSet                getVariablesIndexSet() const;
  void buildSymbolTable(                    ExpressionNode *n);
  void buildSymbolTableIndexedExpression(   ExpressionNode *n);
  void buildSymbolTableAssign(              ExpressionNode *n, bool loopAssignment);
  void checkDependentOnLoopVariablesOnly(   ExpressionNode *n);
  String getNewTempName();
  String getNewLoopName(const String &oldName) const;

  inline void resetSimpleConstants() {;
    m_minusOne = m_zero = m_one = m_two = m_ten = m_half = NULL;
  }
  void markSimpleConstants();
  friend class SNode;
  friend class ExpressionNode;
  friend class ExpressionNodeVariable;
  friend class SumElement;
  friend class FactorArray;
  friend class AllocateNumbers;
  friend class MarkedNodeTransformer;
  friend class ExpressionPainter;
protected:
  ParserTree();
  ParserTree(           const ParserTree &src);
  ParserTree &operator=(const ParserTree &rhs);

  void setOk(bool ok);
  void setTreeForm(ParserTreeForm form);
  void releaseAll();
  void pruneUnusedNodes();
  void markPow1Nodes();
  void deleteUnmarked();
  void buildSymbolTable();
  void clearSymbolTable();
  void copyValues(ParserTree &src);
  ExpressionVariable *getVariableByName(const String &name);
  inline void setValueByIndex(UINT valueIndex, Real value) const {
    getValueRef(valueIndex) = value;
  }
  inline Real getValueByIndex(UINT valueIndex) const {
    return getValueRef(valueIndex);
  }
  ExpressionNode *traverseSubstituteNodes(ExpressionNode *n, CompactNodeHashMap<ExpressionNode*> &nodeMap);

  ExpressionNodeVariable *fetchVariableNode( const String               &name    );
  ExpressionNode         *constExpression(   const String               &name    );
  // if exponent not specified, it is set to 1
  ExpressionFactor       *fetchFactorNode(   ExpressionNode *base, ExpressionNode *exponent = NULL);
  // terminate argumentlist with NULL
  ExpressionNodeTree     *fetchTreeNode(     ExpressionInputSymbol     symbol, ...                );

  inline ExpressionNodeNumber *numberExpression(const Real     &v) {
    return new ExpressionNodeNumber(this, v);
  }
  inline ExpressionNodeNumber *numberExpression(const Rational &v) {
    return new ExpressionNodeNumber(this, v);
  }
  inline ExpressionNodeNumber *numberExpression(int             v) {
    return new ExpressionNodeNumber(this, Rational(v));
  }
  inline ExpressionNodeNumber *numberExpression(UINT            v) {
    return new ExpressionNodeNumber(this, Rational(v));
  }
  inline ExpressionNodeNumber *numberExpression(__int64         v) {
    return new ExpressionNodeNumber(this, Rational(v));
  }
  inline ExpressionNodeNumber *numberExpression(const Number   &v) {
    return new ExpressionNodeNumber(this, v);
  }
  inline ExpressionNodeBoolean *booleanExpression(bool b)          {
    return new ExpressionNodeBoolean(this, b);
  }
  ExpressionNode *allocateLoopVarNode( const String &prefix);

  // Return nodes in std. form
  ExpressionNode *minusS(                    ExpressionNode *n);
  ExpressionNode *reciprocalS(               ExpressionNode *n);
  ExpressionNode *sumS(                      ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *differenceS(               ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *productS(                  ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *quotientS(                 ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *modulusS(                  ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *powerS(                    ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *rootS(                     ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *sqrS(                      ExpressionNode *n);
  ExpressionNode *sqrtS(                     ExpressionNode *n);
  ExpressionNode *expS(                      ExpressionNode *n);
  ExpressionNode *exp10S(                    ExpressionNode *n);
  ExpressionNode *exp2S(                     ExpressionNode *n);
  ExpressionNode *cotS(                      ExpressionNode *n);
  ExpressionNode *cscS(                      ExpressionNode *n);
  ExpressionNode *secS(                      ExpressionNode *n);

  // Return nodes in canonical. form
  ExpressionNode *minusC(                    ExpressionNode *n);
  ExpressionNode *reciprocalC(               ExpressionNode *n);
  ExpressionNode *sumC(                      ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *differenceC(               ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *productC(                  ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *quotientC(                 ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *modulusC(                  ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *powerC(                    ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *rootC(                     ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *sqrC(                      ExpressionNode *n);
  ExpressionNode *sqrtC(                     ExpressionNode *n);
  ExpressionNode *expC(                      ExpressionNode *n);
  ExpressionNode *exp10C(                    ExpressionNode *n);
  ExpressionNode *exp2C(                     ExpressionNode *n);
  ExpressionNode *cotC(                      ExpressionNode *n);
  ExpressionNode *cscC(                      ExpressionNode *n);
  ExpressionNode *secC(                      ExpressionNode *n);

  void initDynamicOperations(ParserTreeForm treeForm);
public:
  virtual ~ParserTree();
  virtual Real evaluateRealExpr(const ExpressionNode *n) const = 0;

  inline const ExpressionNode *getRoot() const {
    return m_root;
  }
  inline ExpressionNode *getRoot() {
    return m_root;
  }

  void setRoot(ExpressionNode *n);

  void unmarkAll();
  inline bool isOk() const {
    return m_ok;
  }

  inline ParserTreeForm getTreeForm() const {
    return m_treeForm;
  }
  inline String getTreeFormName() const {
    return getTreeFormName(getTreeForm());
  }

  static String getTreeFormName(ParserTreeForm treeForm);

  inline ExpressionNode *getMinusOne() {
    if(!m_minusOne) m_minusOne = numberExpression(-1);
    return m_minusOne;
  }
  inline ExpressionNode *getZero() {
    if(!m_zero    ) m_zero     = numberExpression(0);
    return m_zero;
  }
  inline ExpressionNode *getOne() {
    if(!m_one     ) m_one      = numberExpression(1);
    return m_one;
  }
  inline ExpressionNode *getTwo() {
    if(!m_two     ) m_two      = numberExpression(2);
    return m_two;
  }
  inline ExpressionNode *getTen() {
    if(!m_ten     ) m_ten      = numberExpression(10);
    return m_ten;
  }
  inline ExpressionNode *getHalf() {
    if(!m_half    ) m_half     = numberExpression(Rational(1,2));
    return m_half;
  }

  inline SNode _0()    { return SNode(getZero()); }
  inline SNode _1()    { return SNode(getOne());  }
  inline SNode _2()    { return SNode(getTwo());  }
  inline SNode _10()   { return SNode(getTen());  }
  inline SNode _half() { return SNode(getHalf()); }

  inline Real &getValueRef(const ExpressionVariable &var) const {
    return getValueRef(var.getValueIndex());
  }
  inline Real &getValueRef(UINT valueIndex) const {
    return m_valueTable[valueIndex];
  }
  inline size_t getValueCount() const {
    return m_valueTable.size();
  }
  ExpressionNodePoly     *fetchPolyNode(     const ExpressionNodeArray  &coefficientArray, ExpressionNode *argument);
  ExpressionFactor       *getFactor(         ExpressionNode *base, ExpressionNode *exponent = NULL);
  ExpressionNode         *getTree(           ExpressionInputSymbol       symbol, ExpressionNodeArray &a);
  ExpressionNode         *getSum(            AddentArray          &addentArray);
  ExpressionNode         *getProduct(        FactorArray          &factorArray);
  ExpressionNode         *getPoly(           ExpressionNodeArray &coefficientArray, ExpressionNode *argument);

  ExpressionFactor       *getFactor(         ExpressionFactor     *oldFactor,  ExpressionNode *newBase, ExpressionNode *newExpo);
  ExpressionNode         *getTree(           ExpressionNode *oldTree         , ExpressionNodeArray  &newChildArray  );
  ExpressionNode         *getSum(            ExpressionNode *oldSum          , AddentArray          &newAddentArray );
  ExpressionNode         *getProduct(        ExpressionNode *oldProduct      , FactorArray          &newFactorArray );
  ExpressionNode         *getPoly(           ExpressionNode *oldPoly         , ExpressionNodeArray  &newCoefficientArray, ExpressionNode *newArgument);

  ExpressionNode         *expandPower(       ExpressionNode *base, const Rational &exponent);

  // used by parser
  ExpressionNode         *vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr);

typedef ExpressionNode *(ParserTree::*UnaryOperator)( ExpressionNode *n);
typedef ExpressionNode *(ParserTree::*BinaryOperator)(ExpressionNode *n1, ExpressionNode *n2);

  UnaryOperator  pminus, preciprocal, psqr ,  psqrt, pexp,  pexp10, pexp2, pcot,   pcsc,   psec;
  BinaryOperator psum  , pdiff      , pprod,  pquot, pmod,  ppower, proot;

  ExpressionNode *conditionalExpression(     ExpressionNode *condition
                                            ,ExpressionNode *exprTrue
                                            ,ExpressionNode *exprFalse);

  ExpressionNode *and(                       ExpressionNode *n1, ExpressionNode *n2);
  ExpressionNode *or(                        ExpressionNode *n1, ExpressionNode *n2);

  ExpressionNode *assignStatement(           ExpressionNode *leftSide
                                            ,ExpressionNode *expr );
  ExpressionNode *functionExpression(        ExpressionInputSymbol symbol
                                            ,ExpressionNode *child);
  ExpressionNode *unaryMinus(                ExpressionNode *child);
  ExpressionNode *unaryExpression(           ExpressionInputSymbol symbol
                                            ,ExpressionNode *child);
  ExpressionNode *binaryExpression(          ExpressionInputSymbol symbol
                                            ,ExpressionNode *left
                                            ,ExpressionNode *right);
  ExpressionNode *ternaryExpression(         ExpressionInputSymbol symbol
                                            ,ExpressionNode *child0
                                            ,ExpressionNode *child1
                                            ,ExpressionNode *child2);

  ExpressionNode *indexedSum(                ExpressionNode *assign
                                            ,ExpressionNode *endExpr
                                            ,ExpressionNode *expr);

  ExpressionNode *indexedProduct(            ExpressionNode *assign
                                            ,ExpressionNode *endExpr
                                            ,ExpressionNode *expr);

  void addError( ExpressionNode *n              , const TCHAR *format,...);
  void addError( const SourcePosition       &pos, const TCHAR *format,...);
  void addError(                                  const TCHAR *format,...);
  void vAddError(const SourcePosition       *pos, const TCHAR *format, va_list argptr);

  const StringArray &getErrors() const {
    return m_errors;
  }

  // Error should be an element from StringArray returned by getErrors().
  // Will return sourcePosition specified in error as "(line,col):errorText"
  // and modify error to be text after "(line,col):"
  // If no leading "(line,col):" an Exception is thrown
  static SourcePosition decodeErrorString(String &error);

  // Error should be an element from StringArray returned by getErrors().
  // Will return textposition in expr, remove the textposition "(line,column)" from error
  // If no leading "(line,col):" an Exception is thrown
  static inline int decodeErrorString(const String &expr, String &error) {
    return decodeErrorString(error).findCharIndex(expr);
  }

  void listErrors(FILE *f = stdout) const;
  void listErrors(tostream &out) const;
  void listErrors(const TCHAR *fname) const;
  void setValue(const String &name, const Real &value);
  const ExpressionVariable *getVariable(const String &name) const;

  int getNodeCount(ExpressionNodeSelector *selector = NULL);
  // Terminate symbolset with 0. Only specified symbols will be counted
  int getNodeCount(bool ignoreMarked, ExpressionInputSymbol s1,...);
  int getTreeDepth() const;
  ParserTreeComplexity getComplexity();
#ifdef TRACE_REDUCTION_CALLSTACK
  const ReductionStack &getReductionStack() const {
    return m_reductionStack;
  }
  void clearAllBreakPoints();
#endif
  ExpressionVariableArray getAllVariables() const;
  int getNodeTableSize() const {
    return (int)m_nodeTable.size();
  }
  int getAddentTableSize() const {
    return (int)m_addentTable.size();
  }
  void substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap);
  void traverseTree(ExpressionNodeHandler &handler);
  String variablesToString() const;
  String treeToString() const;
};
