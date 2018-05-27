#pragma once

#include <MyUtil.h>
#include <CompactHashMap.h>
#include <Scanner.h>
#include "PragmaLib.h"
#include "SNode.h"

namespace Expr {

class ExpressionVariable {
private:
  const String m_name;
  // Index into parserTree.m_valueTable
  int          m_valueIndex;
  UINT         m_constant : 1;
  UINT         m_defined  : 1;
  UINT         m_loopVar  : 1;
  UINT         m_marked   : 1;

public:
  ExpressionVariable(const String &name, bool isConstant, bool isDefined, bool isLoopVar);
  inline const String &getName()       const    { return m_name;                    }
  inline       int     getValueIndex() const    { return m_valueIndex;              }
  inline void          setValueIndex(int index) { m_valueIndex = index;             }
  inline void          setDefined()             { m_defined = 1;                    }
  inline bool          isConstant()    const    { return m_constant ? true : false; }
  inline bool          isDefined()     const    { return m_defined  ? true : false; }
  inline bool          isLoopVar()     const    { return m_loopVar  ? true : false; }
  inline bool          isInput()       const    { return (m_constant | m_defined | m_loopVar) == 0; }
  inline bool          isMarked()      const    { return m_marked   ? true : false; }
  inline void          unMark()                 { m_marked = 0; }
  inline void          mark()                   { m_marked = 1; }
  String toString() const;
};

class ExpressionVariableWithValue : public ExpressionVariable {
private:
  const Real m_value;
public:
  inline ExpressionVariableWithValue(const ExpressionVariable &var, Real value)
    : ExpressionVariable(var)
    , m_value(value)
  {
  }
  inline ExpressionVariableWithValue(const String &name, bool isConstant, bool isDefined, bool isLoopVar, Real value)
    : ExpressionVariable(name, isConstant, isDefined, isLoopVar)
    , m_value(value)
  {
  }

  inline Real getValue() const {
    return m_value;
  }
};

typedef Array<ExpressionVariableWithValue> ExpressionVariableArray;

class ExpressionNode;
class SumElement;
class ExpressionFactor;
class ExpressionNodeSelector;
class ParserTree;
class Expression;

class AddentArray : public CompactArray<SumElement*> {
public:
  AddentArray() {
  }
  explicit AddentArray(size_t capacity) : CompactArray<SumElement*>(capacity) {
  }
  void add(SNode n, bool positive);
  inline void add(SumElement *e) {
    __super::add(e);
  }
  inline AddentArray &operator+=(SNode n) {
    add(n, true);
    return *this;
  }
  inline AddentArray &operator-=(SNode n) {
    add(n, false);
    return *this;
  }
  bool equal(     const AddentArray &a) const;
  bool equalMinus(const AddentArray &a) const;
  AddentArray &sort();        // return *this
  AddentArray &sortStdForm(); // return *this

  String toString() const;
};

class FactorArray : public CompactArray<ExpressionFactor*> {
public:
  FactorArray() {
  }
  explicit FactorArray(size_t capacity) : CompactArray<ExpressionFactor*>(capacity) {
  }

  FactorArray selectConstantPositiveExponentFactors() const;
  FactorArray selectConstantNegativeExponentFactors() const;
  FactorArray selectNonConstantExponentFactors() const;
  int findFactorWithChangeableSign() const;

  void add(ExpressionFactor *f);
  void add(SNode base);
  void add(SNode base, SNode exponent);
  void add(SNode base, const Rational &exponent);
  bool isSameNodes(const FactorArray &a) const; // compare if ExpressionNode* equals
  bool equal(      const FactorArray &a) const;
  bool equalMinus( const FactorArray &a) const;
  FactorArray &sort(); // return *this

  String toString() const;
};

class ExpressionSymbolSet : public BitSet {
public:
  ExpressionSymbolSet();
  ExpressionSymbolSet(ExpressionInputSymbol s1,...); // terminate list with EOI
};

class ExpressionNodeHandler {
public:
  virtual bool handleNode(ExpressionNode *n, int level) = 0;
};

typedef CompactKeyType<ExpressionInputSymbol> ExpressionSymbolKey;

template<class E> class CompactSymbolHashMap : public CompactHashMap<ExpressionSymbolKey, E> {
};

class PackedSyntaxNodeInfo {
public:
  const ExpressionInputSymbol m_symbol         : 15;
          UINT          m_marked               : 1;  // used for garbage-collection
          UINT          m_breakPoint           : 1;  // used for DebugThread
  mutable UINT          m_coefArrayConstant    : 1;  // used by polynomials
  mutable UINT          m_coefArrayChecked     : 1;  // used by polynomials
  mutable UINT          m_reduced              : 1;
  PackedSyntaxNodeInfo(ExpressionInputSymbol symbol)
    : m_symbol(      symbol)
    , m_marked(           0)
    , m_breakPoint(       0)
    , m_coefArrayConstant(0)
    , m_coefArrayChecked( 0)
    , m_reduced(          0)
  {
  }
};

class ExpressionNode {
  DECLAREDEBUGSTR;
private:
  ParserTree          &m_tree;
protected:
  PackedSyntaxNodeInfo m_info;
private:
  Real evaluatePow()               const;
  Real evaluateRoot()              const;
  ExpressionNode &operator=(const ExpressionNode &src); // not implemented
  ExpressionNode(           const ExpressionNode &src); // not implemented
protected:
  static String &addLeftMargin(String &s, int level);

  Exception createAttributeNotSupportedException(const char *attribute) const;
#define UNSUPPORTEDOP() throw createAttributeNotSupportedException(__FUNCTION__)

public:
  ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol);
  virtual ~ExpressionNode() {}

  inline ExpressionInputSymbol       getSymbol()                    const   { return (ExpressionInputSymbol)m_info.m_symbol;                   }
  inline        ParserTree          &getTree()                      const   { return m_tree;                                                   }
  inline String                      getSymbolName()                const   { return getSymbolName(getSymbol());  }
  static String                      getSymbolName(ExpressionInputSymbol symbol);
  inline  void                       mark()                                 { m_info.m_marked = true;                                          }
  inline  void                       unMark()                               { m_info.m_marked = false;                                         }
  inline  bool                       isMarked()                     const   { return m_info.m_marked;                                          }
  inline  void                       setReduced()                   const   { m_info.m_reduced = true;                                         }
  inline  bool                       isReduced()                    const   { return m_info.m_reduced;                                         }
  inline  void                       setBreakPoint()                        { m_info.m_breakPoint = 1;                                         }
  inline  void                       clearBreakPoint()                      { m_info.m_breakPoint = 0;                                         }
  inline  bool                       isBreakPoint()                 const   { return m_info.m_breakPoint;                                      }
  virtual bool                       hasPos()                       const   { return false;    }
  virtual const SourcePosition      &getPos()                       const   { UNSUPPORTEDOP(); }
  ExpressionNode                    *left()                         const   { return child(0).node(); }
  ExpressionNode                    *right()                        const   { return child(1).node(); }
  virtual       SNode                getArgument()                  const   { UNSUPPORTEDOP(); }
  virtual       SNode                child(UINT i)                  const   { UNSUPPORTEDOP(); }
  virtual int                        getChildCount()                const   { UNSUPPORTEDOP(); }
  virtual const SNodeArray          &getChildArray()                const   { UNSUPPORTEDOP(); }
  virtual       SNodeArray          &getChildArray()                        { UNSUPPORTEDOP(); }
  virtual const FactorArray         &getFactorArray()               const   { UNSUPPORTEDOP(); }
  virtual       FactorArray         &getFactorArray()                       { UNSUPPORTEDOP(); }
  virtual const AddentArray         &getAddentArray()               const   { UNSUPPORTEDOP(); }
  virtual       AddentArray         &getAddentArray()                       { UNSUPPORTEDOP(); }
  virtual const SNodeArray          &getCoefArray()                 const   { UNSUPPORTEDOP(); }
  virtual       SNodeArray          &getCoefArray()                         { UNSUPPORTEDOP(); }
  virtual int                        getFirstCoefIndex()            const   { UNSUPPORTEDOP(); }
  virtual void                       setFirstCoefIndex(int index)           { UNSUPPORTEDOP(); }
  virtual bool                       isCoefArrayConstant()          const   { UNSUPPORTEDOP(); }

  virtual int                        getDegree()                    const   { UNSUPPORTEDOP(); }
  virtual const String              &getName()                      const   { UNSUPPORTEDOP(); }
  virtual void                       setVariable(ExpressionVariable *var)   { UNSUPPORTEDOP(); }
  virtual       ExpressionVariable  &getVariable()                  const   { UNSUPPORTEDOP(); }
  virtual const Number              &getNumber()                    const   { UNSUPPORTEDOP(); }
  virtual       int                  getValueIndex()                const   { UNSUPPORTEDOP(); }
  virtual       void                 setValueIndex(int index)               { UNSUPPORTEDOP(); }
  virtual       bool                 getBool()                      const   { UNSUPPORTEDOP(); }
  virtual ExpressionNode            *expand()                               { UNSUPPORTEDOP(); }
  virtual bool                       isExpandable()                         { return false;    }

  virtual int                        compare(ExpressionNode *n);

  virtual ExpressionNode            *clone(ParserTree *tree)        const = 0;

  virtual bool                       isConstant()                   const = 0;
  virtual bool                       isBooleanOperator()            const { return false; }
  // return reference to lvalue
  virtual Real                      &doAssignment()                 const { UNSUPPORTEDOP(); }
  virtual Real                       evaluateReal()                 const;
  virtual bool                       evaluateBool()                 const { UNSUPPORTEDOP(); }

  virtual ExpressionNodeType         getNodeType()                  const = 0;
  virtual ExpressionReturnType       getReturnType()                const   { return EXPR_RETURN_REAL; }
  virtual bool                       hasVariable()                  const   { return false;    }
  virtual bool                       traverseExpression(ExpressionNodeHandler &handler, int level) = 0;

  virtual void                       dumpNode(String &s, int level) const = 0;
  virtual String                     toString()                     const = 0;

  inline  bool                       isName()                       const   { return getSymbol() == NAME;                                      }
  inline  bool                       isNumber()                     const   { return getSymbol() == NUMBER;                                    }
  inline  bool                       isBoolean()                    const   { return getSymbol() == TYPEBOOL;                                  }
  inline  bool                       isRational()                   const   { return isNumber() && getNumber().isRational();                   }
  inline  bool                       isInteger()                    const   { return isNumber() && getNumber().isInteger();                    }
  inline  bool                       isNameOrNumber()               const   { return isName() || isNumber();                                   }
  inline  bool                       isEven()                       const   { return isRational() && getRational().isEven();                   }
  inline  bool                       isOdd()                        const   { return isRational() && getRational().isOdd();                    }
  inline  bool                       isMultiplyable()               const   { return !isNameOrNumber();                                        }
  inline  Real                       getReal()                      const   { return getNumber().getRealValue();                               }
  inline  Rational                   getRational()                  const   { return getNumber().getRationalValue();                           }
  inline  bool                       isUnaryMinus()                 const   { return (getSymbol() == MINUS) && (getChildCount() == 1);         }
  inline  bool                       isBinaryMinus()                const   { return (getSymbol() == MINUS) && (getChildCount() == 2);         }
  inline  bool                       isEulersConstant()             const   { return isName()     && getName()  == _T("e");                    }
  inline  bool                       isPi()                         const   { return isName()     && getName()  == _T("pi");                   }
  inline  bool                       isZero()                       const   { return isNumber()   && getReal()  == 0;                          }
  inline  bool                       isOne()                        const   { return isNumber()   && getReal()  == 1;                          }
  inline  bool                       isTwo()                        const   { return isNumber()   && getReal()  == 2;                          }
  inline  bool                       isTen()                        const   { return isNumber()   && getReal()  == 10;                         }
  inline  bool                       isMinusOne()                   const   { return isNumber()   && getReal()  == -1;                         }
  inline  bool                       isMinusTwo()                   const   { return isNumber()   && getReal()  == -2;                         }
  inline  bool                       isNegative()                   const   { return isNumber()   && getReal()  < 0;                           }
  inline  bool                       isPositive()                   const   { return isNumber()   && getReal()  > 0;                           }
  inline  bool                       isTrue()                       const   { return isBoolean()  && getBool();                                }
  inline  bool                       isFalse()                      const   { return isBoolean()  && !getBool();                               }

  static bool                  isBooleanOperator(     ExpressionInputSymbol symbol); // { AND,OR,NOT,EQ,NE,LT,LE,GT,GE }
  static bool                  isCompareOperator(     ExpressionInputSymbol symbol); // { EQ,NE,LT,LE,GT,GE }
  static bool                  isBinaryOperator(      ExpressionInputSymbol symbol); // { POW,ROOT,PROD,QUOT,PLUS,MINUS,MOD }
  static bool                  isTrigonomtricFunction(ExpressionInputSymbol symbol);
  static bool                  isSymmetricFunction(   ExpressionInputSymbol symbol);
  static bool                  isAsymmetricFunction(  ExpressionInputSymbol symbol);
  static ExpressionInputSymbol getInverseFunction(    ExpressionInputSymbol symbol);
  // symbol in { EQ,NE,LE,LT,GE,GT } (EQ,NE are symmetric, the rest are changed. left<->right. ie. GE->LE, etc..
  static ExpressionInputSymbol reverseComparator(ExpressionInputSymbol symbol);
  // symbol in { EQ,NE,LE,LT,GE,GT } (EQ->NE, LE->GT, etc... (this is not the same as reverseComparator)
  static ExpressionInputSymbol negateComparator( ExpressionInputSymbol symbol);
  inline ExpressionInputSymbol getInverseFunction() const { return getInverseFunction(getSymbol()); }
  inline  bool isCompareOperator()       const { return isCompareOperator(     getSymbol()); }
  inline  bool isBinaryOperator()        const { return isBinaryOperator(      getSymbol()); }
  inline  bool isTrigonomtricFunction()  const { return isTrigonomtricFunction(getSymbol()); }
  virtual bool isSymmetricFunction()     const { return isSymmetricFunction(   getSymbol()); }
  virtual bool isAsymmetricFunction()    const { return isAsymmetricFunction(  getSymbol()); }

  bool    dependsOn(const String &name) const;
  // If selector specified, only nodes where selector.select(n) is true will be counted.
  // If not specified, all nodes is counted
  int     getNodeCount(ExpressionNodeSelector *selector = NULL) const;
  bool    containsFunctionCall()        const;
  Real   &getValueRef()                 const;

  bool    isSymmetricExponent()         const;
  bool    isAsymmetricExponent()        const;
  static bool rationalExponentsMultiply(const Rational &r1, const Rational &r2) {
    return ::isAsymmetricExponent(r1) || ::isAsymmetricExponent(r2);
  }

  // if both n1 and n2 are rational constants, they will be reduced as much as possible
  // without loosing symmetry with even exponents. if one or both are not rational
  // these 2 functions will just return as normal (prod/quot)
  ExpressionNode *multiplyExponents(ExpressionNode *n1, ExpressionNode *n2) const;
  ExpressionNode *divideExponents(  ExpressionNode *n1, ExpressionNode *n2) const;
  TrigonometricMode getTrigonometricMode() const;
  int     getPrecedence()               const;
  bool    reducesToRationalConstant(Rational *r) const;
  bool    reducesToRational(        Rational *r) const;

  String  parenthesizedExpressionToString(const ExpressionNode  *parent)  const;
  bool    needParentheses(                const ExpressionNode  *parent)  const;
  int     getMaxTreeDepth()                                               const;
#ifdef CHECK_CONSISTENCY
  void    checkIsConsistent()                                             const;
  bool    isConsistentSymbolAndType()                                     const;
#endif // CHECK_CONSISTENCY

  void throwInvalidSymbolForTreeMode(const TCHAR *method) const;
  void throwUnknownSymbolException(  const TCHAR *method) const;
  void throwUnknownNodeTypeException(const TCHAR *method) const;
};

// Should only be called in Canonical treeform
bool equal(     const ExpressionNode *n1, const ExpressionNode *n2);
bool equalMinus(const ExpressionNode *n1, const ExpressionNode *n2);

typedef CompactKeyType<const ExpressionNode*>  ExpressionNodeKey;

template<class E> class CompactNodeHashMap : public CompactHashMap<ExpressionNodeKey, E> {
};


class ExpressionNodeNumber : public ExpressionNode {
private:
  Number m_number;
  int    m_valueIndex;
public:
  inline ExpressionNodeNumber(ParserTree *tree, const Real &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
    SETDEBUGSTR();
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Rational &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
    SETDEBUGSTR();
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Number &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
    SETDEBUGSTR();
  }

  int getValueIndex() const {
    return m_valueIndex;
  }

  void setValueIndex(int index) {
    m_valueIndex = index;
  }

  const Number &getNumber() const {
    return m_number;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }
  Real evaluateReal() const {
    return m_number.getRealValue();
  }

  ExpressionNodeType getNodeType() const {
    return NT_NUMBER;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return m_number.toString();
  }
};

class ExpressionNodeBoolConst : public ExpressionNode {
private:
  const bool m_value;
public:
  inline ExpressionNodeBoolConst(ParserTree *tree, bool b) : ExpressionNode(tree, TYPEBOOL), m_value(b) {
    SETDEBUGSTR();
  }
  bool getBool() const {
    return m_value;
  }
  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }
  ExpressionReturnType getReturnType() const   {
    return EXPR_RETURN_BOOL;
  }
  Real evaluateReal() const { UNSUPPORTEDOP(); }
  bool evaluateBool() const { return m_value;  }

  ExpressionNodeType getNodeType() const {
    return NT_BOOLCONST;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return boolToStr(m_value);
  }
};

class ExpressionNodeVariable : public ExpressionNode {
private:
  String              m_name;
  ExpressionVariable *m_var;
public:
  ExpressionNodeVariable(ParserTree *tree, const String &name);
  inline ExpressionNodeVariable(ParserTree *tree, const String &name, ExpressionVariable &var)
    : ExpressionNode(tree, NAME)
    , m_name(name)
  {
    m_var  = &var;
    SETDEBUGSTR();
  }

  const String &getName() const {
    return m_name;
  }

  void setVariable(ExpressionVariable *var) {
    m_var = var;
  }

  ExpressionVariable &getVariable() const {
    return *m_var;
  }

  bool hasVariable() const {
    return m_var != NULL;
  }

  int getValueIndex() const {
    return m_var->getValueIndex();
  }

  void setValueIndex(int index) {
    m_var->setValueIndex(index);
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return m_var->isConstant();
  }
  Real evaluateReal() const {
    return getValueRef();
  }

  ExpressionNodeType getNodeType() const {
    return NT_VARIABLE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return getName();
  }
};

class ExpressionNodeTree : public ExpressionNode {
private:
  SNodeArray m_childArray;
  void initChildArray(va_list argptr);
protected:
  // Terminate arguemnt list with NULL
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, ...);

public:
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr);
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, const SNodeArray &childArray);
  ExpressionNodeTree(ParserTree *tree, const ExpressionNodeTree *src);

  SNode child(UINT i) const {
    return m_childArray[i];
  }
  int getChildCount() const {
    return (int)m_childArray.size();
  }
  SNodeArray &getChildArray() {
    return m_childArray;
  }
  const SNodeArray &getChildArray() const {
    return m_childArray;
  }

  ExpressionNode   *expand();
  bool              isExpandable();

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return NT_TREE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeBoolExpr : public ExpressionNodeTree {
public:
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, ExpressionNode *bexpr)
    : ExpressionNodeTree(tree, symbol, bexpr, NULL)
  {
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, ExpressionNode *left, ExpressionNode *right)
    : ExpressionNodeTree(tree, symbol, left, right, NULL)
  {
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, const ExpressionNodeBoolExpr *src)
    : ExpressionNodeTree(tree,src)
  {
  }
   inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeTree(tree, symbol, argptr)
  {
  }
  ExpressionNode *clone(ParserTree *tree) const {
    ExpressionNode *n = new ExpressionNodeBoolExpr(tree, this); TRACE_NEW(n);
    return n;
  }

  bool isConstant() const;
  ExpressionReturnType getReturnType() const {
    return EXPR_RETURN_BOOL;
  }
  Real evaluateReal() const { UNSUPPORTEDOP(); }
  bool evaluateBool() const;

  bool isBooleanOperator() const {
    return true;
  }

  ExpressionNodeType getNodeType() const {
    return NT_BOOLEXPR;
  }
  String toString() const;
};

class ExpressionNodePoly : public ExpressionNode {
private:
  SNodeArray m_coefArray;
  SNode      m_arg;
  int        m_firstCoefIndex;

public:
  ExpressionNodePoly(ParserTree *tree, const SNodeArray &coefArray, SNode arg);
  ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src);

  int getDegree() const {
    return (int)m_coefArray.size() - 1;
  }

  const SNodeArray &getCoefArray() const {
    return m_coefArray;
  }
  SNodeArray &getCoefArray() {
    return m_coefArray;
  }

  SNode getArgument() const {
    return m_arg;
  }
  bool isCoefArrayConstant() const;
  // return true, if all coefficients are constant, and non-zero coef[i] for even i
  bool isSymmetricFunction()        const;
  // return true, if all coefficients are constant, and non-zero coef[i] for odd i
  bool isAsymmetricFunction()       const;

  bool equal(     const ExpressionNode *poly) const; // assume poly.symbol == POLY
  bool equalMinus(const ExpressionNode *poly) const; // assume poly.symbol == POLY
  int getFirstCoefIndex() const {
    return m_firstCoefIndex;
  }

  void setFirstCoefIndex(int index) {
    m_firstCoefIndex = index;
  }

  ExpressionNode       *expand();
  bool                  isExpandable() {
    return true;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;
  Real evaluateReal() const;

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  ExpressionNodeType getNodeType() const {
    return NT_POLY;
  }

  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeAssign : public ExpressionNodeTree {
public:
  ExpressionNodeAssign(ParserTree *tree, va_list argptr)
    : ExpressionNodeTree(tree, ASSIGN, argptr)
  {
  }
  ExpressionNodeAssign(ExpressionNode *leftSide, ExpressionNode *expr)
    : ExpressionNodeTree(&(leftSide->getTree()), ASSIGN, leftSide, expr, NULL)
  {
  }
  ExpressionNodeAssign(ParserTree *tree, const ExpressionNodeAssign *src);
  ExpressionNode *clone(ParserTree *tree) const;

  ExpressionReturnType getReturnType() const {
    return EXPR_NORETURNTYPE;
  }
  Real evaluateReal() const {
    UNSUPPORTEDOP();
  }
  Real &doAssignment() const;

  ExpressionNodeType getNodeType() const {
    return NT_ASSIGN;
  }

  String toString() const;
};

class ExpressionNodeStmtList : public ExpressionNodeTree {
private:
  const ExpressionReturnType m_returnType;
  SNode doAssignments() const; // return last expression
public:
  ExpressionNodeStmtList(ParserTree *tree, const SNodeArray &childArray);
  ExpressionNodeStmtList(ParserTree *tree, const ExpressionNodeStmtList *src);

//  ExpressionNode            *expand();
//  bool                       isExpandable();

//  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

//  bool isConstant() const;
  ExpressionReturnType getReturnType() const {
    return m_returnType;
  }
  Real evaluateReal() const;
  bool evaluateBool() const;

  ExpressionNodeType getNodeType() const {
    return NT_STMTLIST;
  }
  String toString() const;
};

// ----------------------------------- Used by parser to save sourceposition in text -------------------------------------
class SourcePositionAttribute {
private:
  SourcePosition m_pos;
protected:
  SourcePositionAttribute(const SourcePosition &pos) : m_pos(pos) {
  }
public:
  inline const SourcePosition &getPos() const   {
    return m_pos;
  }
};


class ExpressionNodeNumberWithPos : public ExpressionNodeNumber, private SourcePositionAttribute {
public:
  inline ExpressionNodeNumberWithPos(ParserTree *tree, const SourcePosition &pos, const Real &value)
    : ExpressionNodeNumber(tree, value), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeVariableWithPos : public ExpressionNodeVariable, private SourcePositionAttribute {
public:
  ExpressionNodeVariableWithPos(ParserTree *tree, const SourcePosition &pos, const String &name)
    : ExpressionNodeVariable(tree, name), SourcePositionAttribute(pos) {
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeTreeWithPos : public ExpressionNodeTree, private SourcePositionAttribute {
public:
  ExpressionNodeTreeWithPos(ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeTree(tree, symbol, argptr), SourcePositionAttribute(pos)
  {
    if(getChildCount() > 0)
      SETDEBUGSTR();
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeBoolExprWithPos : public ExpressionNodeBoolExpr, private SourcePositionAttribute {
public:
  ExpressionNodeBoolExprWithPos(ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeBoolExpr(tree, symbol, argptr), SourcePositionAttribute(pos) {
    SETDEBUGSTR();
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodePolyWithPos : public ExpressionNodePoly, private SourcePositionAttribute {
public:
  ExpressionNodePolyWithPos(ParserTree *tree, const SourcePosition &pos, const SNodeArray &coefArray, SNode arg)
    : ExpressionNodePoly(tree, coefArray, arg), SourcePositionAttribute(pos)
  {
    SETDEBUGSTR();
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

class ExpressionNodeAssignWithPos : public ExpressionNodeAssign, private SourcePositionAttribute {
public:
  ExpressionNodeAssignWithPos(ParserTree *tree, const SourcePosition &pos, va_list argptr)
    : ExpressionNodeAssign(tree, argptr), SourcePositionAttribute(pos)
  {
    SETDEBUGSTR();
  }
  const SourcePosition &getPos() const {
    return SourcePositionAttribute::getPos();
  }
  bool hasPos() const {
    return true;
  }
};

// ------------------------------------------------------------------------------------------------------

class ExpressionNodeSum : public ExpressionNode {
private:
  AddentArray m_elements;
public:
  ExpressionNodeSum(ParserTree *tree, const AddentArray &elements);

  AddentArray &getAddentArray() {
    return m_elements;
  }
  const AddentArray &getAddentArray() const {
    return m_elements;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;
  Real evaluateReal() const;

  ExpressionNodeType getNodeType() const {
    return NT_SUM;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeProduct : public ExpressionNode {
private:
  FactorArray m_factors;
public:
  ExpressionNodeProduct(ParserTree *tree, FactorArray &factors);

  FactorArray &getFactorArray() {
    return m_factors;
  }
  const FactorArray &getFactorArray() const {
    return m_factors;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;
  Real evaluateReal() const;

  ExpressionNodeType getNodeType() const {
    return NT_PRODUCT;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeSelector : public Selector<const ExpressionNode*> {
};

class ExpressionNodeSymbolSelector : public ExpressionNodeSelector {
private:
  const ExpressionSymbolSet *m_symbolSet;
  const bool                 m_ignoreMarked;
public:
  ExpressionNodeSymbolSelector(const ExpressionSymbolSet *symbolSet, bool ignoreMarked=false) : m_symbolSet(symbolSet), m_ignoreMarked(ignoreMarked) {
  }
  bool select(const ExpressionNode* const &n) {
    return (!m_ignoreMarked || !n->isMarked()) && ((m_symbolSet == NULL) || m_symbolSet->contains(n->getSymbol()));
  }
  AbstractSelector *clone() const {
    return new ExpressionNodeSymbolSelector(m_symbolSet, m_ignoreMarked);
  }
};

}; // namespace Expr
