#pragma once

#include <MyUtil.h>
#include <Stack.h>
#include <HashMap.h>
#include <PropertyContainer.h>
#include <Scanner.h>
#include "ExpressionSymbol.h"
#include "ParserTreeSymbolTable.h"
#include "SNode.h"

#ifdef _DEBUG
//#define TRACE_REDUCTION_CALLSTACK
#endif

typedef enum {
  PP_STATE                 // ParserTreeState
 ,PP_REDUCEITERATION       // UINT
 ,PP_ROOT                  // ExpressionNode
 ,PP_OK                    // bool
 ,PP_TREEFORM              // ParserTreeForm
 ,EP_TRIGONOMETRICMODE     // TrigonometricMode    (expressionProperty)
 ,EP_RETURNTYPE            // ExpressionReturnType (expressionProperty)
 ,EP_MACHINECODE           // bool                 (expressionProperty)
 ,REDUCTION_STACKHIGHT
} ParserTreeProperties;

typedef enum {
  TREEFORM_STANDARD
 ,TREEFORM_CANONICAL
 ,TREEFORM_NUMERIC
} ParserTreeForm;

typedef enum {
  PS_EMPTY
 ,PS_COMPILED
 ,PS_DERIVED
 ,PS_CANONICALFORM
 ,PS_MAINREDUCTION1
 ,PS_MAINREDUCTION2
 ,PS_RATIONALPOWERSREDUCTION
 ,PS_STANDARDFORM
 ,PS_REDUCTIONDONE
} ParserTreeState;

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

// Only an interface....do not save any state in this, or any derived classes
class NodeOperators {
protected:
  static ExpressionNode  *getZero(           ExpressionNode *n);
  static ExpressionNode  *getOne(            ExpressionNode *n);
  static ExpressionNode  *getMinusOne(       ExpressionNode *n);
  static ExpressionNode  *getTwo(            ExpressionNode *n);
  static ExpressionNode  *getTen(            ExpressionNode *n);
  static ExpressionNode  *numberExpression(  ExpressionNode *n, const Number &v);
  static ExpressionNode  *numberExpression(  ExpressionNode *n, INT64         v);

  static ExpressionNode  *unaryExpression(   ExpressionInputSymbol symbol, ExpressionNode *n);
  static ExpressionNode  *binaryExpression(  ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2);
  static ExpressionNode  *functionExpression(ExpressionInputSymbol symbol, ExpressionNode *n);
public:
  virtual ExpressionNode *sum(       ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *diff(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *prod(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *quot(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *mod(       ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *power(     ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *root(      ExpressionNode *n1, ExpressionNode *n2) const = NULL;
  virtual ExpressionNode *minus(     ExpressionNode *n) const = NULL;
  virtual ExpressionNode *reciprocal(ExpressionNode *n) const = NULL;
  virtual ExpressionNode *sqr(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *sqrt(      ExpressionNode *n) const = NULL;
  virtual ExpressionNode *exp(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *exp10(     ExpressionNode *n) const = NULL;
  virtual ExpressionNode *exp2(      ExpressionNode *n) const = NULL;
  virtual ExpressionNode *cot(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *csc(       ExpressionNode *n) const = NULL;
  virtual ExpressionNode *sec(       ExpressionNode *n) const = NULL;

  virtual ParserTreeForm  getTreeForm() const = NULL;
  static const NodeOperators *s_stdForm, *s_canonForm, *s_stdNumForm, *s_canonNumForm;
};

class ParserTree : public PropertyContainer {
private:
  ExpressionNode               *m_root;
  CompactArray<ExpressionNode*> m_nodeTable;
  CompactArray<SumElement*>     m_addentTable;
  ParserTreeSymbolTable         m_symbolTable;
  StringArray                   m_errors;
  bool                          m_ok;
  const NodeOperators          *m_ops;
  mutable ParserTreeState       m_state;
  mutable UINT                  m_reduceIteration;
  ExpressionNodeNumber         *m_minusOne, *m_zero, *m_one, *m_two, *m_ten, *m_half;
  ExpressionNodeBoolean        *m_false, *m_true;
private:
  inline void resetSimpleConstants() {
    m_minusOne = m_zero = m_one = m_two = m_ten = m_half = NULL;
    m_false = m_true = NULL;
  }
  void init(ParserTreeState      state
           ,UINT                 reduceIteration);
  void markSimpleConstants();
  friend class ExpressionNode;
  friend class ExpressionNodeVariable;
  friend class NodeOperators;
  friend class NodeOperatorsCanonForm;
  friend class SNode;
  friend class ParserTreeSymbolTable;
  friend class SumElement;
  friend class FactorArray;
  friend class MarkedNodeTransformer;
  friend class ExpressionPainter;

protected:
  ParserTree();
  ParserTree(           const ParserTree &src);
  ParserTree &operator=(const ParserTree &src);
  void setOk(             bool            ok      );
  void setTreeForm(       ParserTreeForm  form    );
  void setState(          ParserTreeState newState);
  void setReduceIteration(UINT            it      );
  void releaseAll();
  void pruneUnusedNodes();
  void markPow1Nodes();
  void deleteUnmarked();
  void buildSymbolTable(const ParserTreeSymbolTable *oldValues = NULL) {
    m_symbolTable.create(this, oldValues);
  }
  ExpressionVariable *getVariableByName(const String &name) {
    return m_symbolTable.getVariable(name);
  }
  inline void setValueByIndex(UINT valueIndex, Real value) const {
    getValueRef(valueIndex) = value;
  }
  inline Real getValueByIndex(UINT valueIndex) const {
    return getValueRef(valueIndex);
  }
  ExpressionNode *traverseSubstituteNodes(ExpressionNode *n, CompactNodeHashMap<ExpressionNode*> &nodeMap);

  ExpressionNode *allocateLoopVarNode(const String &prefix) {
    ExpressionNodeVariable *result = fetchVariableNode(m_symbolTable.getNewLoopName(prefix));
    m_symbolTable.allocateSymbol(result, false, true, true);
    return result;
  }

  ExpressionNodeVariable *fetchVariableNode( const String               &name    );
  ExpressionNode         *constExpression(   const String               &name    );
  // if exponent not specified, it is set to 1
  ExpressionFactor       *fetchFactorNode(   ExpressionNode *base, ExpressionNode *exponent = NULL);
  // terminate argumentlist with NULL
  ExpressionNodeTree     *fetchTreeNode(     ExpressionInputSymbol     symbol, ...                );

  inline ExpressionNodeNumber *numberExpression(const Real     &v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeNumber *numberExpression(const Rational &v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeNumber *numberExpression(int             v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, Rational(v)); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeNumber *numberExpression(UINT            v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, Rational(v)); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeNumber *numberExpression(INT64           v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, Rational(v)); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeNumber *numberExpression(const Number   &v) {
    ExpressionNodeNumber *n = new ExpressionNodeNumber(this, v); TRACE_NEW(n);
    return n;
  }
  inline ExpressionNodeBoolean *booleanExpression(bool b)          {
    ExpressionNodeBoolean *n = new ExpressionNodeBoolean(this, b); TRACE_NEW(n);
    return n;
  }

  void expandMarkedNodes();
  void multiplyMarkedNodes();

public:
  virtual ~ParserTree();

  inline const ExpressionNode *getRoot() const {
    return m_root;
  }
  inline ExpressionNode *getRoot() {
    return m_root;
  }

  void setRoot(ExpressionNode *n);

  virtual TrigonometricMode getTrigonometricMode() const = NULL;

  void unmarkAll();
  inline bool isOk() const {
    return m_ok;
  }

  inline ParserTreeForm getTreeForm() const {
    return m_ops->getTreeForm();
  }
  inline String getTreeFormName() const {
    return getTreeFormName(getTreeForm());
  }

  static String getTreeFormName(ParserTreeForm treeForm);

  inline ParserTreeState getState() const {
    return m_state;
  }

  inline UINT getReduceIteration() const {
    return m_reduceIteration;
  }

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
  inline ExpressionNode *getFalse() {
    if(!m_false   ) m_false    = booleanExpression(false);
    return m_false;
  }
  inline ExpressionNode *getTrue() {
    if(!m_true    ) m_true     = booleanExpression(true);
    return m_true;
  }
  inline Real &getValueRef(UINT valueIndex) const {
    return m_symbolTable.getValueRef(valueIndex);
  }
  inline Real &getValueRef(const ExpressionVariable &var) const {
    return getValueRef(var.getValueIndex());
  }

  inline ExpressionNode  *sum(       ExpressionNode *n1, ExpressionNode *n2) { return m_ops->sum(  n1,n2); }
  inline ExpressionNode  *diff(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->diff( n1,n2); }
  inline ExpressionNode  *prod(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->prod( n1,n2); }
  inline ExpressionNode  *quot(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->quot( n1,n2); }
  inline ExpressionNode  *mod(       ExpressionNode *n1, ExpressionNode *n2) { return m_ops->mod(  n1,n2); }
  inline ExpressionNode  *power(     ExpressionNode *n1, ExpressionNode *n2) { return m_ops->power(n1,n2); }
  inline ExpressionNode  *root(      ExpressionNode *n1, ExpressionNode *n2) { return m_ops->root( n1,n2); }
  inline ExpressionNode  *minus(     ExpressionNode *n) { return m_ops->minus(n);      }
  inline ExpressionNode  *reciprocal(ExpressionNode *n) { return m_ops->reciprocal(n); }
  inline ExpressionNode  *sqr(       ExpressionNode *n) { return m_ops->sqr(n);        }
  inline ExpressionNode  *sqrt(      ExpressionNode *n) { return m_ops->sqrt(n);       }
  inline ExpressionNode  *exp(       ExpressionNode *n) { return m_ops->exp(n);        }
  inline ExpressionNode  *exp10(     ExpressionNode *n) { return m_ops->exp10(n);      }
  inline ExpressionNode  *exp2(      ExpressionNode *n) { return m_ops->exp2(n);       }
  inline ExpressionNode  *cot(       ExpressionNode *n) { return m_ops->cot(n);        }
  inline ExpressionNode  *csc(       ExpressionNode *n) { return m_ops->csc(n);        }
  inline ExpressionNode  *sec(       ExpressionNode *n) { return m_ops->sec(n);        }

  ExpressionNodePoly     *fetchPolyNode(     const ExpressionNodeArray  &coefficientArray, ExpressionNode *argument);
  ExpressionFactor       *getFactor(         ExpressionNode *base, ExpressionNode *exponent = NULL);
  ExpressionNode         *getTree(           ExpressionInputSymbol       symbol, ExpressionNodeArray &a);
  ExpressionNode         *getSum(            AddentArray          &addentArray);
  ExpressionNode         *getProduct(        FactorArray          &factorArray);
  ExpressionNode         *getPoly(           ExpressionNodeArray  &coefficientArray, ExpressionNode *argument);

  ExpressionFactor       *getFactor(         ExpressionFactor     *oldFactor,  ExpressionNode *newBase, ExpressionNode *newExpo);
  ExpressionNode         *getTree(           ExpressionNode *oldTree         , ExpressionNodeArray  &newChildArray  );
  ExpressionNode         *getSum(            ExpressionNode *oldSum          , AddentArray          &newAddentArray );
  ExpressionNode         *getProduct(        ExpressionNode *oldProduct      , FactorArray          &newFactorArray );
  ExpressionNode         *getPoly(           ExpressionNode *oldPoly         , ExpressionNodeArray  &newCoefficientArray, ExpressionNode *newArgument);

  ExpressionNode         *expandPower(       ExpressionNode *base, const Rational &exponent);

  // used by parser
  ExpressionNode         *vFetchNode(const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr);

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

  void addError( ExpressionNode *n              , _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void addError( const SourcePosition       &pos, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void addError(                                  _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void vAddError(const SourcePosition       *pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

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
  inline void setValue(const String &name, const Real &value) {
    m_symbolTable.setValue(name,value);
  }
  inline const ExpressionVariable *getVariable(const String &name) const {
    return m_symbolTable.getVariable(name);
  }

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
  int getNodeTableSize() const {
    return (int)m_nodeTable.size();
  }
  int getAddentTableSize() const {
    return (int)m_addentTable.size();
  }
  void substituteNodes(CompactNodeHashMap<ExpressionNode*> &nodeMap);
  void traverseTree(ExpressionNodeHandler &handler);
  const ParserTreeSymbolTable &getSymbolTable() const {
    return m_symbolTable;
  }
  String treeToString() const;
};
