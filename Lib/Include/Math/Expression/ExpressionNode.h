#pragma once

#include <MyUtil.h>
#include <CompactHashMap.h>
#include <Scanner.h>
#include <Math/Number.h>
#include "ExpressionSymbol.h"

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

class ExpressionNodeArray : public CompactArray<ExpressionNode*> {
public:
  ExpressionNodeArray() {
  }
  explicit ExpressionNodeArray(size_t capacity) : CompactArray<ExpressionNode*>(capacity) {
  }
  ExpressionNode *toTree(ExpressionInputSymbol delimiter) const;
  bool isConstant() const;
  BitSet getNonConstantNodes() const;
  String toString() const;
};

class AddentArray : public CompactArray<SumElement*> {
public:
  AddentArray() {
  }
  explicit AddentArray(size_t capacity) : CompactArray<SumElement*>(capacity) {
  }
  void add(ExpressionNode *n, bool positive);
  inline void add(SumElement *e) {
    __super::add(e);
  }
  inline AddentArray &operator+=(ExpressionNode *n) {
    add(n, true);
    return *this;
  }
  inline AddentArray &operator-=(ExpressionNode *n) {
    add(n, false);
    return *this;
  }
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
  void add(ExpressionNode *base, ExpressionNode *exponent = NULL);
  String toString() const;
};

class ExpressionSymbolSet : public BitSet {
public:
  ExpressionSymbolSet();
};

class ExpressionNodeHandler {
public:
  virtual bool handleNode(ExpressionNode *n, int level) = 0;
};

typedef CompactKeyType<ExpressionInputSymbol> ExpressionSymbolKey;

template<class E> class CompactSymbolHashMap : public CompactHashMap<ExpressionSymbolKey, E> {
};

class SymbolOrderMap : private CompactSymbolHashMap<int> {
private:
  bool m_initDone;
  void init();
public:
  SymbolOrderMap() {
    m_initDone = false;
  };
  int compare(ExpressionInputSymbol s1, ExpressionInputSymbol s2);
};

class InverseFunctionMap : private CompactSymbolHashMap<ExpressionInputSymbol> {
private:
  bool m_initDone;
  void init();
public:
  InverseFunctionMap() {
    m_initDone = false;
  };
  ExpressionInputSymbol getInverse(ExpressionInputSymbol symbol);
};

typedef enum {
  EXPRESSIONNODENUMBER
 ,EXPRESSIONNODEBOOLEAN
 ,EXPRESSIONNODEVARIABLE
 ,EXPRESSIONNODEPOLYNOMIAL
 ,EXPRESSIONNODETREE
 ,EXPRESSIONNODESUM
 ,EXPRESSIONNODEPRODUCT
 ,EXPRESSIONNODEFACTOR
} ExpressionNodeType;

class PackedSyntaxNodeInfo {
public:
  const ExpressionInputSymbol m_symbol         : 15;
  // used for garbage-collection
          UINT          m_marked               : 1;
  // used for DebugThread
          UINT          m_breakPoint           : 1;
  // used by polynomials
  mutable UINT          m_coefficientsConstant : 1;
  // used by polynomials
  mutable UINT          m_coefChecked          : 1;
  PackedSyntaxNodeInfo(ExpressionInputSymbol symbol)
    : m_symbol(symbol)
    , m_marked(0)
    , m_breakPoint(0)
    , m_coefficientsConstant(0)
    , m_coefChecked(0)
  {
  }
};

class ExpressionNode {
private:
  static SymbolOrderMap      s_orderMap;
  static InverseFunctionMap  s_inverseFunctionMap;
         ParserTree         &m_tree;
  PackedSyntaxNodeInfo       m_info;

  Exception createAttributeNotSupportedException(const char *attribute) const;
  ExpressionNode &operator=(const ExpressionNode &src); // not implemented
  ExpressionNode(           const ExpressionNode &src); // not implemented
protected:
  static String &addLeftMargin(String &s, int level);
public:
  ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol);
  virtual ~ExpressionNode() {}
  inline ExpressionInputSymbol       getSymbol()                    const   { return (ExpressionInputSymbol)m_info.m_symbol;                   }
  inline  const ParserTree          *getTree()                      const   { return &m_tree;                                                  }
  inline        ParserTree          *getTree()                              { return &m_tree;                                                  }
          const Expression          *getExpr()                      const;
                Expression          *getExpr();
  String                             getSymbolName()                const;
  inline  void                       mark()                                 { m_info.m_marked = true;                                          }
  inline  void                       unMark()                               { m_info.m_marked = false;                                         }
  inline  bool                       isMarked()                     const   { return m_info.m_marked;                                          }
  inline  void                       setBreakPoint()                        { m_info.m_breakPoint = 1;                                         }
  inline  void                       clearBreakPoint()                      { m_info.m_breakPoint = 0;                                         }
  inline  bool                       isBreakPoint()                 const   { return m_info.m_breakPoint;                                      }
  virtual bool                       hasPos()                       const   { return false;                                                    }
  virtual const SourcePosition      &getPos()                       const   { throw createAttributeNotSupportedException("Pos");               }
  virtual       ExpressionNode      *left()                                 { return child(0);                                                 }
  virtual const ExpressionNode      *left()                         const   { return child(0);                                                 }
  virtual       ExpressionNode      *right()                                { return child(1);                                                 }
  virtual const ExpressionNode      *right()                        const   { return child(1);                                                 }
  virtual       ExpressionNode      *getArgument()                          { throw createAttributeNotSupportedException("Argument");          }
  virtual const ExpressionNode      *getArgument()                  const   { throw createAttributeNotSupportedException("Argument");          }
  virtual       ExpressionNode      *child(UINT i)                          { throw createAttributeNotSupportedException("Child");             }
  virtual const ExpressionNode      *child(UINT i)                  const   { throw createAttributeNotSupportedException("Child");             }
  virtual int                        getChildCount()                const   { throw createAttributeNotSupportedException("ChildCount");        }
  virtual       ExpressionNodeArray &getChildArray()                        { throw createAttributeNotSupportedException("ChildArray");        }
  virtual const ExpressionNodeArray &getChildArray()                const   { throw createAttributeNotSupportedException("ChildArray");        }
  virtual       FactorArray         &getFactorArray()                       { throw createAttributeNotSupportedException("FactorArray");       }
  virtual const FactorArray         &getFactorArray()               const   { throw createAttributeNotSupportedException("FactorArray");       }
  virtual       AddentArray         &getAddentArray()                       { throw createAttributeNotSupportedException("AddentArray");       }
  virtual const AddentArray         &getAddentArray()               const   { throw createAttributeNotSupportedException("AddentArray");       }
  virtual       ExpressionNodeArray &getCoefficientArray()                  { throw createAttributeNotSupportedException("CoefficientArray");  }
  virtual const ExpressionNodeArray &getCoefficientArray()          const   { throw createAttributeNotSupportedException("CoefficientArray");  }
  virtual int                        getFirstCoefIndex()            const   { throw createAttributeNotSupportedException("FirstCoefIndex");    }
  virtual void                       setFirstCoefIndex(int index)           { throw createAttributeNotSupportedException("FirstCoefIndex");    }
  virtual int                        getDegree()                    const   { throw createAttributeNotSupportedException("Degree");            }
  virtual const String              &getName()                      const   { throw createAttributeNotSupportedException("Name");              }
  virtual void                       setVariable(ExpressionVariable *var)   { throw createAttributeNotSupportedException("Variable");          }
  virtual       ExpressionVariable  &getVariable()                  const   { throw createAttributeNotSupportedException("Variable");          }
  virtual const Number              &getNumber()                    const   { throw createAttributeNotSupportedException("Number");            }
  virtual       int                  getValueIndex()                const   { throw createAttributeNotSupportedException("ValueIndex");        }
  virtual       void                 setValueIndex(int index)               { throw createAttributeNotSupportedException("ValueIndex");        }
  virtual       bool                 getBool()                      const   { throw createAttributeNotSupportedException("Bool");              }
  virtual ExpressionNode            *expand()                               { throw createAttributeNotSupportedException("Expand");            }
  virtual bool                       isExpandable()                         { return false;                                                    }

  virtual int                        compare(ExpressionNode *n);

  virtual       ExpressionNode      *clone(ParserTree *tree)        const = 0;

  virtual bool                       isConstant()                   const = 0;
  virtual ExpressionNodeType         getNodeType()                  const = 0;
  virtual bool                       hasVariable()                  const   { return false;                                                    }
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
  inline  ExpressionInputSymbol      getInverseFunction() const {
    return s_inverseFunctionMap.getInverse(getSymbol());
  }

  bool    isCoefficientArrayConstant()  const;
  bool    dependsOn(const String &name) const;
  // If selector specified, only nodes where selector.select(n) is true will be counted.
  // If not specified, all nodes is counted
  int     getNodeCount(ExpressionNodeSelector *selector = NULL) const;
  bool    containsFunctionCall()        const;
  Real   &getValueRef()                 const;
  bool    isBinaryOperator()            const;
  bool    isBooleanOperator()           const;
  bool    isTrigonomtricFunction()      const;
  bool    isSymmetricFunction()         const;
  bool    isAsymmetricFunction()        const;
  bool    isSymmetricExponent()         const;
  bool    isAsymmetricExponent()        const;
  int     getPrecedence()               const;

  String  parenthesizedExpressionToString(const ExpressionNode  *parent)  const;
  bool    needParentheses(                const ExpressionNode  *parent)  const;
  int     getMaxTreeDepth()                                               const;
  String  statementListToString()                                         const;
};

typedef CompactKeyType<const ExpressionNode*>  ExpressionNodeKey;

template<class E> class CompactNodeHashMap : public CompactHashMap<ExpressionNodeKey, E> {
};

ExpressionNodeArray getExpressionList(ExpressionNode *n);
ExpressionNodeArray getStatementList( ExpressionNode *n);

class ExpressionNodeNumber : public ExpressionNode {
private:
  Number m_number;
  int    m_valueIndex;
public:
  inline ExpressionNodeNumber(ParserTree *tree, const Real &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Rational &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Number &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
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

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODENUMBER;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return m_number.toString();
  }
};

class ExpressionNodeBoolean : public ExpressionNode {
private:
  const bool m_value;
public:
  ExpressionNodeBoolean(ParserTree *tree, bool b) : ExpressionNode(tree, TYPEBOOL), m_value(b) {
  }
  bool getBool() const {
    return m_value;
  }

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEBOOLEAN;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return getBool() ? _T("true") : _T("false");
  }
};

class ExpressionNodeVariable : public ExpressionNode {
private:
  String              m_name;
  ExpressionVariable *m_var;
public:

  ExpressionNodeVariable(ParserTree *tree, const String &name);

  ExpressionNodeVariable(ParserTree *tree, const String &name, ExpressionVariable &var)
    : ExpressionNode(tree, NAME)
    , m_name(name)
  {
    m_var  = &var;
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

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEVARIABLE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const {
    return getName();
  }
};

class ExpressionNodeTree : public ExpressionNode {
private:
  ExpressionNodeArray m_childArray;
  void initChildArray(va_list argptr);
protected:
  // Terminate arguemnt list with NULL
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, ...);

public:
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr);
  ExpressionNodeTree(ParserTree *tree, ExpressionInputSymbol symbol, const ExpressionNodeArray &childArray);
  ExpressionNodeTree(ParserTree *tree, const ExpressionNodeTree *src);

  ExpressionNode *child(UINT i) {
    return m_childArray[i];
  }
  const ExpressionNode *child(UINT i) const {
    return m_childArray[i];
  }

  int getChildCount() const {
    return (int)m_childArray.size();
  }

  ExpressionNodeArray &getChildArray() {
    return m_childArray;
  }
  const ExpressionNodeArray &getChildArray() const {
    return m_childArray;
  }

  ExpressionNode            *expand();
  bool                       isExpandable();

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODETREE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodePoly : public ExpressionNode {
private:
  ExpressionNodeArray  m_coefficientArray;
  ExpressionNode      *m_argument;
  int                  m_firstCoefIndex;

public:
  ExpressionNodePoly(ParserTree *tree, va_list argptr);
  ExpressionNodePoly(ParserTree *tree, const ExpressionNodeArray &coefficientArray, ExpressionNode *argument);
  ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src);

  int getDegree() const {
    return (int)m_coefficientArray.size() - 1;
  }

  ExpressionNodeArray &getCoefficientArray() {
    return m_coefficientArray;
  }
  const ExpressionNodeArray &getCoefficientArray() const {
    return m_coefficientArray;
  }

  ExpressionNode *getArgument() {
    return m_argument;
  }

  const ExpressionNode *getArgument() const {
    return m_argument;
  }

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

  bool traverseExpression(ExpressionNodeHandler &handler, int level);
  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEPOLYNOMIAL;
  }

  void dumpNode(String &s, int level) const;

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
    : ExpressionNodeTree(tree, symbol, argptr), SourcePositionAttribute(pos) {
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
  ExpressionNodePolyWithPos(ParserTree *tree, const SourcePosition &pos, va_list argptr)
    : ExpressionNodePoly(tree, argptr), SourcePositionAttribute(pos) {
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

  static ExpressionNodeSum *multiply(ExpressionNodeSum *n1, ExpressionNodeSum *n2);

  int compare(ExpressionNode *n);

  ExpressionNode *clone(ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODESUM;
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

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEPRODUCT;
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

// Wrapper class til ExpressionNode
class SNode {
private:
  ExpressionNode *m_node;
public:
  inline SNode() : m_node(NULL) {
  }
  inline SNode(ExpressionNode *node) : m_node(node) {
  }
  SNode(ParserTree *tree, int             v);
  SNode(ParserTree *tree, __int64         v);
  SNode(ParserTree *tree, const Rational &v);
  SNode(ParserTree *tree, const Real     &v);
  inline bool isEmpty() const {
    return m_node == NULL;
  }
  inline ExpressionNode *node() {
    return m_node;
  }
  inline const ExpressionNode *node() const {
    return m_node;
  }
  inline operator ExpressionNode*() {
    return m_node;
  }
  inline operator const ExpressionNode*() const {
    return m_node;
  }
  inline ExpressionInputSymbol getSymbol() const   {
    return m_node->getSymbol();
  }
  inline String getSymbolName() const {
    return m_node->getSymbolName();
  }

  inline ExpressionVariable &variable() const {
    return m_node->getVariable();
  }
  inline const String       &name()  const { return m_node->getName(); }
  inline SNode               left()  const { return m_node->left();    }
  inline SNode               right() const { return m_node->right();   }
  inline SNode               child(UINT index) const {
    return m_node->child(index);
  }
  inline ExpressionNodeArray &getChildArray() const {
    return m_node->getChildArray();
  }
  inline FactorArray &getFactorArray() const {
    return m_node->getFactorArray();
  }
  inline AddentArray &getAddentArray() const {
    return m_node->getAddentArray();
  }
  inline ExpressionNodeArray &getCoefficientArray() const {
    return m_node->getCoefficientArray();
  }
  inline SNode getArgument() const {
    return m_node->getArgument();
  }
  inline int getDegree() const {
    return m_node->getDegree();
  }
  inline bool isUnaryMinus() const {
    return m_node->isUnaryMinus();
  }
  inline bool isNumber() const {
    return m_node->isNumber();
  }
  inline bool isInteger() const {
    return m_node->isInteger();
  }
  inline bool isRational() const {
    return m_node->isRational();
  }
  inline const Number &getNumber() const {
    return m_node->getNumber();
  }
  inline Real getReal() const {
    return m_node->getReal();
  }
  inline Rational getRational() const {
    return m_node->getRational();
  }
  inline bool isZero() const {
    return m_node->isZero();
  }
  inline bool isOne() const {
    return m_node->isOne();
  }
  inline bool isTen() const {
    return m_node->isTen();
  }
  inline bool isEulersConstant() const {
    return m_node->isEulersConstant();
  }
  inline bool isNegative() const {
    return m_node->isNegative();
  }
  inline bool isOdd() const {
    return m_node->isOdd();
  }
  inline bool isConstant() const {
    return m_node->isConstant();
  }
  inline bool isSymmetricFunction() const {
    return m_node->isSymmetricFunction();
  }
  inline bool isAsymmetricFunction() const {
    return m_node->isAsymmetricFunction();
  }
  inline bool isTrue() const {
    return m_node->isTrue();
  }
  inline bool isFalse() const {
    return m_node->isFalse();
  }
  inline ExpressionInputSymbol getInverseFunction() const {
    return m_node->getInverseFunction();
  }
  SNode operator+(  const SNode &n) const;
  // binary -
  SNode operator-(  const SNode &n) const;
  // unary  -
  SNode operator-() const;
  SNode operator*(  const SNode &n) const;
  SNode operator/(  const SNode &n) const;
  SNode operator%(  const SNode &n) const;
  SNode &operator+=(const SNode &n);
  SNode &operator-=(const SNode &n);
  SNode &operator*=(const SNode &n);
  SNode &operator/=(const SNode &n);
  SNode operator&&( const SNode &n) const;
  SNode operator||( const SNode &n) const;
  SNode operator!() const;
  bool operator==( const SNode &n) const;
  inline bool operator!=(const SNode &n) const {
    return !(*this == n);
  }

  inline bool isSameNode(const SNode n) const {
    return m_node == n.m_node;
  }
  inline String toString() const {
    return isEmpty() ? _T("") : m_node->toString();
  }

  friend SNode reciprocal(const SNode &x);
  friend SNode sqrt(      const SNode &x);
  friend SNode sqr(       const SNode &x);
  friend SNode pow(       const SNode &x, const SNode &y);
  friend SNode root(      const SNode &x, const SNode &y);
  friend SNode exp(       const SNode &x);
  friend SNode exp10(     const SNode &x);
  friend SNode exp2(      const SNode &x);
  friend SNode ln(        const SNode &x);
  friend SNode log10(     const SNode &x);
  friend SNode log2(      const SNode &x);
  friend SNode sin(       const SNode &x);
  friend SNode cos(       const SNode &x);
  friend SNode tan(       const SNode &x);
  friend SNode cot(       const SNode &x);
  friend SNode asin(      const SNode &x);
  friend SNode acos(      const SNode &x);
  friend SNode atan(      const SNode &x);
  friend SNode atan2(     const SNode &y, const SNode &x);
  friend SNode acot(      const SNode &x);
  friend SNode csc(       const SNode &x);
  friend SNode sec(       const SNode &x);

  friend SNode sinh(      const SNode &x);
  friend SNode cosh(      const SNode &x);
  friend SNode tanh(      const SNode &x);
  friend SNode gauss(     const SNode &x);
};

class SNodeArray : public CompactArray<SNode> {
protected:
  SNode toTree(ExpressionInputSymbol delimiter);
};

class SStmtList : public SNodeArray {
public:
  SStmtList() {
  };
  SStmtList(SNode n);
  SStmtList &removeUnusedAssignments();

  operator SNode() {
    return toTree(SEMI);
  }
};

class SExprList : public SNodeArray {
public:
  SExprList() {
  }
  SExprList(ExpressionNodeArray &a);
  operator ExpressionNodeArray();
};

SNode unaryExp(  ExpressionInputSymbol symbol, SNode n);
SNode binExp(    ExpressionInputSymbol symbol, SNode n1, SNode n2);
SNode condExp(   SNode condition , SNode nTrue  , SNode nFalse);
SNode polyExp(   SExprList &coefficientArray, SNode argument);
SNode indexSum(  SNode assignStmt, SNode endExpr, SNode expr  );
SNode indexProd( SNode assignStmt, SNode endExpr, SNode expr  );
SNode assignStmt(SNode leftSide  , SNode expr);
