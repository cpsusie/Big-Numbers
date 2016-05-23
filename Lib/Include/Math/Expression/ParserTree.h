#pragma once

#include <MyUtil.h>
#include <Stack.h>
#include <HashMap.h>
#include <TreeMap.h>
#include <PropertyChangeListener.h>
#include <Scanner.h>
#include "ExpressionSymbol.h"
#include "ExpressionNode.h"

#ifdef _DEBUG
#define TRACE_REDUCTION_CALLSTACK
#endif

typedef enum {
  EXPR_STATE                 // ExpressionState
 ,EXPR_REDUCEITERATION       // unsigned int
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
  ParserTreeComplexity(const ParserTree &tree);
  friend int parserTreeComplexityCmp(const ParserTreeComplexity &rs1, const ParserTreeComplexity &rs2);
  inline bool operator==(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) == 0;
  }
  inline bool operator!=(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) != 0;
  }
  inline bool operator<(const ParserTreeComplexity  &rs) const {
    return parserTreeComplexityCmp(*this, rs) < 0;
  }
  inline bool operator<=(const ParserTreeComplexity &rs) const {
    return parserTreeComplexityCmp(*this, rs) <= 0;
  }
  inline bool operator>(const ParserTreeComplexity  &rs) const {
    return parserTreeComplexityCmp(*this, rs) > 0;
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
  const TCHAR                *m_method;
  String                      m_str;
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
#define ENTERMETHOD()       pushReductionMethod(method, format(_T("n:<%s>"), n.getDebugString().cstr()),n)
#define ENTERMETHOD1(v)     pushReductionMethod(method, format(_T("%s:<%s>"), #v, (v).getDebugString().cstr()))
#define ENTERMETHOD2(v1,v2) pushReductionMethod(method, format(_T("%s:<%s>, %s:<%s>"), #v1, (v1).getDebugString().cstr(), #v2, (v2).getDebugString().cstr()))
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
  pushReductionMethod(method, format(_T("Reduced:<%s>"), _n.getDebugString().cstr()),_n);  \
  m_reductionStack.pop();                                                                  \
  RETURN(_n);                                                                              \
}

#define RETURNSHOWSTR(v)                                                                   \
{ const String _s = (v).toString();                                                        \
  pushReductionMethod(method, format(_T("%s:<%s>"), #v, _s.cstr()));                       \
  m_reductionStack.pop();                                                                  \
  RETURN(v);                                                                               \
}

#define RETURNPSHOWSTR(p)                                                                  \
{ const String _s = (p)->toString();                                                       \
  pushReductionMethod(method, format(_T("%s:<%s>"), #p, _s.cstr()));                       \
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
  const ExpressionNode                 *m_root;
  int                                   m_indexNameCounter;
  mutable StringTreeMap<int>            m_symbolTable;
  mutable Array<ExpressionVariable>     m_variables;
  mutable CompactArray<ExpressionNode*> m_nodeTable;
  mutable CompactArray<SumElement*>     m_addentTable;
  mutable StringArray                   m_errors;
  mutable bool                          m_ok;
  ParserTreeForm                        m_treeForm;
  mutable const ExpressionNodeNumber   *m_minusOne, *m_zero, *m_one, *m_two, *m_half;

  ExpressionVariable   *allocateSymbol(     const String &name, const Real &value, bool isConstant, bool isLeftSide, bool isLoopVar) const;
  ExpressionVariable   *allocateSymbol(     const ExpressionNode *n        , bool isConstant, bool isLeftSide, bool isLoopVar) const;
  ExpressionVariable   *allocateConstant(   const ExpressionNode *n, const String &name, const Real &value) const;
  void buildSymbolTable(                    const ExpressionNode *n);
  void buildSymbolTableIndexedExpression(   const ExpressionNode *n);
  void buildSymbolTableAssign(              const ExpressionNode *n, bool loopAssignment);
  void checkDependentOnLoopVariablesOnly(   const ExpressionNode *n);
  String getNewTempName();
  String getNewLoopName(const String &oldName) const;

  inline void resetSimpleConstants() {;
    m_minusOne = m_zero = m_one = m_two = m_half = NULL;
  }
  void markSimpleConstants();
  friend class SNode;
  friend class ExpressionNode;
  friend class ExpressionNodeVariable;
  friend class SumElement;
  friend class FactorArray;
  friend class MarkedNodeTransformer;
protected:
  ParserTree();
  ParserTree(           const ParserTree &src);
  ParserTree &operator=(const ParserTree &rhs);

  void setOk(bool ok);
  void setTreeForm(ParserTreeForm form);
  void releaseAll();
  void pruneUnusedNodes();
  void markPow1Nodes() const;
  void deleteUnmarked();
  void buildSymbolTable();
  void clearSymbolTable();
  void copyValues(ParserTree &src);
  const ExpressionNode *traverseSubstituteNodes(const ExpressionNode *n, const CompactNodeHashMap<const ExpressionNode*> &nodeMap);


  inline const ExpressionNodeNumber *numberExpression(const Real     &v) const {
    return new ExpressionNodeNumber(this, v);
  }
  inline const ExpressionNodeNumber *numberExpression(const Rational &v) const {
    return new ExpressionNodeNumber(this, v);
  }
  inline const ExpressionNodeNumber *numberExpression(int             v) const {
    return new ExpressionNodeNumber(this, Rational(v));
  }
  inline const ExpressionNodeNumber *numberExpression(unsigned int    v) const {
    return new ExpressionNodeNumber(this, Rational(v));
  }
  inline const ExpressionNodeNumber *numberExpression(__int64         v) const {
    return new ExpressionNodeNumber(this, Rational(v));
  }
  inline const ExpressionNodeNumber *numberExpression(const Number   &v) const {
    return new ExpressionNodeNumber(this, v);
  }
  inline const ExpressionNodeBoolean *booleanExpression(bool b)          const {
    return new ExpressionNodeBoolean(this, b);
  }
  const ExpressionNode *allocateLoopVarNode( const String &prefix) const;

  // Return nodes in std. form
  const ExpressionNode *minusS(                    const ExpressionNode *n)  const;
  const ExpressionNode *reciprocalS(               const ExpressionNode *n)  const;
  const ExpressionNode *sumS(                      const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *differenceS(               const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *productS(                  const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *quotientS(                 const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *modulusS(                  const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *powerS(                    const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *rootS(                     const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *sqrS(                      const ExpressionNode *n)  const;
  const ExpressionNode *sqrtS(                     const ExpressionNode *n)  const;
  const ExpressionNode *expS(                      const ExpressionNode *n)  const;
  const ExpressionNode *cotS(                      const ExpressionNode *n)  const;
  const ExpressionNode *cscS(                      const ExpressionNode *n)  const;
  const ExpressionNode *secS(                      const ExpressionNode *n)  const;

  // Return nodes in canonical. form
  const ExpressionNode *minusC(                    const ExpressionNode *n)  const;
  const ExpressionNode *reciprocalC(               const ExpressionNode *n)  const;
  const ExpressionNode *sumC(                      const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *differenceC(               const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *productC(                  const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *quotientC(                 const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *modulusC(                  const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *powerC(                    const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *rootC(                     const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *sqrC(                      const ExpressionNode *n)  const;
  const ExpressionNode *sqrtC(                     const ExpressionNode *n)  const;
  const ExpressionNode *expC(                      const ExpressionNode *n)  const;
  const ExpressionNode *cotC(                      const ExpressionNode *n)  const;
  const ExpressionNode *cscC(                      const ExpressionNode *n)  const;
  const ExpressionNode *secC(                      const ExpressionNode *n)  const;

  void initDynamicOperations(ParserTreeForm treeForm);
public:
  virtual ~ParserTree();
  virtual Real evaluateRealExpr(const ExpressionNode *n) const = 0;

  inline const ExpressionNode *getRoot() const {
    return m_root;
  }
  
  void setRoot(const ExpressionNode *n);
  
  void unmarkAll() const;
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

  inline const ExpressionNode *getMinusOne() const {
    if(!m_minusOne) m_minusOne = numberExpression(-1);
    return m_minusOne;
  }
  inline const ExpressionNode *getZero() const {
    if(!m_zero    ) m_zero     = numberExpression(0);
    return m_zero;
  }
  inline const ExpressionNode *getOne() const {
    if(!m_one     ) m_one      = numberExpression(1);
    return m_one;
  }
  inline const ExpressionNode *getTwo() const {
    if(!m_two     ) m_two      = numberExpression(2);
    return m_two;
  }
  inline const ExpressionNode *getHalf() const {
    if(!m_half    ) m_half     = numberExpression(Rational(1,2));
    return m_half;
  }

  inline const SNode _0()    const { return SNode(getZero()); }
  inline const SNode _1()    const { return SNode(getOne());  }
  inline const SNode _2()    const { return SNode(getTwo());  }
  inline const SNode _half() const { return SNode(getHalf()); }

  const ExpressionNodeVariable *fetchVariableNode( const String               &name    ) const;
  const ExpressionNode         *constExpression(   const String               &name    ) const;
  const ExpressionFactor       *fetchFactorNode(   const ExpressionNode *base            , const ExpressionNode *exponent = NULL) const; // if exponent not specified, it is set to 1
  const ExpressionNodeTree     *fetchTreeNode(     ExpressionInputSymbol       symbol, ...               ) const; // terminate argumentlist with NULL
  const ExpressionNodePoly     *fetchPolyNode(     const ExpressionNodeArray  &coefficientArray, const ExpressionNode *argument       ) const;

  const ExpressionFactor       *getFactor(         const ExpressionNode *base            , const ExpressionNode *exponent = NULL) const;
  const ExpressionNode         *getTree(           ExpressionInputSymbol       symbol          , const ExpressionNodeArray  &a              ) const;
  const ExpressionNode         *getSum(            const AddentArray          &addentArray)  const;
  const ExpressionNode         *getProduct(        const FactorArray          &factorArray)  const;
  const ExpressionNode         *getPoly(           const ExpressionNodeArray &coefficientArray, const ExpressionNode *argument       ) const;

  const ExpressionFactor       *getFactor(         const ExpressionFactor     *oldFactor,        const ExpressionNode *newBase, const ExpressionNode *newExpo) const;
  const ExpressionNode         *getTree(           const ExpressionNode *oldTree         , const ExpressionNodeArray  &newChildArray  ) const;
  const ExpressionNode         *getSum(            const ExpressionNode *oldSum          , const AddentArray          &newAddentArray ) const;
  const ExpressionNode         *getProduct(        const ExpressionNode *oldProduct      , const FactorArray          &newFactorArray ) const;
  const ExpressionNode         *getPoly(           const ExpressionNode *oldPoly         , const ExpressionNodeArray &newCoefficientArray, const ExpressionNode *newArgument) const;

  const ExpressionNode         *expandPower(       const ExpressionNode *base, const Rational &exponent) const;

  ExpressionNode               *vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr); // used by parser

typedef const ExpressionNode *(ParserTree::*UnaryOperator)( const ExpressionNode *n) const;
typedef const ExpressionNode *(ParserTree::*BinaryOperator)(const ExpressionNode *n1, const ExpressionNode *n2) const;

  UnaryOperator  pminus, preciprocal, psqr ,  psqrt, pexp,  pcot,   pcsc,   psec;
  BinaryOperator psum  , pdiff      , pprod,  pquot, pmod,  ppower, proot;

  const ExpressionNode *conditionalExpression(     const ExpressionNode *condition
                                                  ,const ExpressionNode *exprTrue
                                                  ,const ExpressionNode *exprFalse) const;

  const ExpressionNode *and(                       const ExpressionNode *n1, const ExpressionNode *n2) const;
  const ExpressionNode *or(                        const ExpressionNode *n1, const ExpressionNode *n2) const;

  const ExpressionNode *assignStatement(           const ExpressionNode *leftSide
                                                  ,const ExpressionNode *expr ) const;
  const ExpressionNode *functionExpression(        ExpressionInputSymbol symbol
                                                  ,const ExpressionNode *child) const;
  const ExpressionNode *unaryMinus(                const ExpressionNode *child) const;
  const ExpressionNode *unaryExpression(           ExpressionInputSymbol symbol
                                                  ,const ExpressionNode *child) const;
  const ExpressionNode *binaryExpression(          ExpressionInputSymbol symbol
                                                  ,const ExpressionNode *left
                                                  ,const ExpressionNode *right) const;
  const ExpressionNode *ternaryExpression(         ExpressionInputSymbol symbol
                                                  ,const ExpressionNode *child0
                                                  ,const ExpressionNode *child1
                                                  ,const ExpressionNode *child2) const;

  const ExpressionNode *indexedSum(                const ExpressionNode *assign
                                                  ,const ExpressionNode *endExpr
                                                  ,const ExpressionNode *expr) const;

  const ExpressionNode *indexedProduct(            const ExpressionNode *assign
                                                  ,const ExpressionNode *endExpr
                                                  ,const ExpressionNode *expr) const;

  void addError( const ExpressionNode *n  , const TCHAR *format,...) const;
  void addError( const SourcePosition       &pos, const TCHAR *format,...) const;
  void addError(                                  const TCHAR *format,...) const;
  void vAddError(const SourcePosition       *pos, const TCHAR *format, va_list argptr) const;

  const StringArray &getErrors() const {
    return m_errors;
  }

  static int decodeErrorString(const String &expr, String &error); // Error should be an element from StringArray returned by getErrors(). 
                                                                   // Will return textposition in expr, remove the textposition "(line,column)" from error
  void listErrors(FILE *f = stdout) const;
  void listErrors(tostream &out) const;
  void listErrors(const TCHAR *fname) const;
  void setValue(const String &name, const Real &value);
  ExpressionVariable *getVariable(const String &name);
  const ExpressionVariable *getVariable(const String &name) const;

  int getNodeCount(ExpressionNodeSelector *selector = NULL) const;
  int getNodeCount(bool ignoreMarked, ExpressionInputSymbol s1,...) const; // terminate symbolset with 0. Only specified symbols will be counted
  int getTreeDepth() const;
  ParserTreeComplexity getComplexity() const;
#ifdef TRACE_REDUCTION_CALLSTACK
  const ReductionStack &getReductionStack() const {
    return m_reductionStack;
  }
  void enableReductionStack(bool enable);
  void clearAllBreakPoints();
#endif
  Array<ExpressionVariable> &getVariables() {
    return m_variables;
  }
  const Array<ExpressionVariable> &getVariables() const {
    return m_variables;
  }
  int getNodeTableSize() const {
    return m_nodeTable.size();
  }
  int getAddentTableSize() const {
    return m_addentTable.size();
  }
  void substituteNodes(const CompactNodeHashMap<const ExpressionNode*> &nodeMap);
  void traverseTree(ExpressionNodeHandler &handler) const;
  String variablesToString() const;
  String treeToString() const;
  static void enableAllDebugStrings(bool enable);
};
