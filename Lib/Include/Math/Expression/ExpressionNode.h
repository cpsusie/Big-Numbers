#pragma once

#include <MyUtil.h>
#include <CompactHashMap.h>
#include <Scanner.h>
#include "ExpressionSymbol.h"
#include "Number.h"

class ExpressionVariable {
private:
  String       m_name;
  Real         m_value;
  unsigned int m_constant : 1;
  unsigned int m_defined  : 1;
  unsigned int m_loopVar  : 1;
  unsigned int m_marked   : 1;
public:
  ExpressionVariable(const String &name, const Real &value, bool isConstant, bool isDefined, bool isLoopVar);
  inline const String &getName()    const { return m_name;                    }
  inline       Real   &getValue()         { return m_value;                   }
  inline const Real   &getValue()   const { return m_value;                   }
  void                 setValue(const Real &value);
  inline void          setDefined()       { m_defined = 1;                    }
  inline bool          isConstant() const { return m_constant ? true : false; }
  inline bool          isDefined()  const { return m_defined  ? true : false; }
  inline bool          isLoopVar()  const { return m_loopVar  ? true : false; }
  inline bool          isInput()    const { return (m_constant | m_defined | m_loopVar) == 0; }
  inline bool          isMarked()   const { return m_marked   ? true : false; }
  inline void          unMark()           { m_marked = 0; }
  inline void          mark()             { m_marked = 1; }
  String toString() const;
};

class ExpressionNode;
class SumElement;
class ExpressionFactor;
class ExpressionNodeSelector;
class ParserTree;
class Expression;

class ExpressionNodeArray : public CompactArray<const ExpressionNode*> {
#ifdef _DEBUG
private:
  String                m_debugString;
public:
  static bool           s_debugStringEnabled;
  void initDebugString();
  void debugStringAddLast();
  void debugStringAddAll(const ExpressionNodeArray &src);
  inline const String &getDebugString() const { return m_debugString; }

#define INITEXPRESSIONNODEARRAYDEBUGSTRING(a)      { if(ExpressionNodeArray::s_debugStringEnabled) (a)->initDebugString();      }
#define EXPRESSIONNODEARRAYADDLAST(        a)      { if(ExpressionNodeArray::s_debugStringEnabled) (a)->debugStringAddLast();   }
#define EXPRESSIONNODEARRAYADDALL(         a, src) { if(ExpressionNodeArray::s_debugStringEnabled) (a)->debugStringAddAll(src); }
#else
#define INITEXPRESSIONNODEARRAYDEBUGSTRING(a)
#define EXPRESSIONNODEARRAYADDLAST(        a)
#define EXPRESSIONNODEARRAYADDALL(         a, src)
#endif

public:
  ExpressionNodeArray() {
  }
  explicit ExpressionNodeArray(size_t capacity) : CompactArray<const ExpressionNode*>(capacity) {
  }
  const ExpressionNode *toTree(ExpressionInputSymbol delimiter) const;

#ifdef _DEBUG
  inline void clear(intptr_t capacity=0) {
    CompactArray<const ExpressionNode*>::clear(capacity);
    INITEXPRESSIONNODEARRAYDEBUGSTRING(this);
  }
  inline void add(const ExpressionNode *n) {
    CompactArray<const ExpressionNode*>::add(n);
    EXPRESSIONNODEARRAYADDLAST(this);
  }
  inline void remove(size_t index, size_t count = 1) {
    CompactArray<const ExpressionNode*>::remove(index, count);
    INITEXPRESSIONNODEARRAYDEBUGSTRING(this);
  }
  inline void addAll(const ExpressionNodeArray &src) {
    CompactArray<const ExpressionNode*>::addAll(src);
    EXPRESSIONNODEARRAYADDALL(this, src);
  }
#endif

  bool isConstant() const;

  String toString() const;
  inline static bool enableDebugString(bool enabled) {
#ifdef _DEBUG
    const bool ret = s_debugStringEnabled;
    s_debugStringEnabled = enabled;
    return ret;
#else
    return false;
#endif
  }
};

class AddentArray : public CompactArray<const SumElement*> {
#ifdef _DEBUG
private:
  String                m_debugString;
public:
  static bool           s_debugStringEnabled;
  void initDebugString();
  void debugStringAddLast();
  void debugStringAddAll(const AddentArray &src);
  inline const String &getDebugString() const { return m_debugString; }

#define INITADDENTARRAYDEBUGSTRING(a)      { if(AddentArray::s_debugStringEnabled) (a)->initDebugString();      }
#define ADDENTARRAYADDLAST(        a)      { if(AddentArray::s_debugStringEnabled) (a)->debugStringAddLast();   }
#define ADDENTARRAYADDALL(         a, src) { if(AddentArray::s_debugStringEnabled) (a)->debugStringAddAll(src); }
#else
#define INITADDENTARRAYDEBUGSTRING(a)
#define ADDENTARRAYADDLAST(        a)
#define ADDENTARRAYADDALL(         a, src)
#endif

public:
  AddentArray() {
  }
  explicit AddentArray(size_t capacity) : CompactArray<const SumElement*>(capacity) {
  }

  void add(const ExpressionNode *n, bool positive);

  inline AddentArray &operator+=(const ExpressionNode *n) {
    add(n, true);
    return *this;
  }

  inline AddentArray &operator-=(const ExpressionNode *n) {
    add(n, false);
    return *this;
  }

  inline void clear(intptr_t capacity=0) {
    CompactArray<const SumElement*>::clear(capacity);
    INITADDENTARRAYDEBUGSTRING(this);
  }
  inline void add(const SumElement *e) {
    CompactArray<const SumElement*>::add(e);
    ADDENTARRAYADDLAST(this);
  }
  inline void remove(size_t index, size_t count = 1) {
    CompactArray<const SumElement*>::remove(index, count);
    INITADDENTARRAYDEBUGSTRING(this);
  }
  inline void addAll(const AddentArray &src) {
    CompactArray<const SumElement*>::addAll(src);
    ADDENTARRAYADDALL(this, src);
  }

  String toString() const;
  inline static bool enableDebugString(bool enabled) {
#ifdef _DEBUG
    const bool ret = s_debugStringEnabled;
    s_debugStringEnabled = enabled;
    return ret;
#else
    return false;
#endif
  }
};

class FactorArray : public CompactArray<const ExpressionFactor*> {
#ifdef _DEBUG
private:
  String                m_debugString;

public:
  static bool           s_debugStringEnabled;
  void initDebugString();
  void debugStringAddLast();
  void debugStringAddAll(const FactorArray &src);
  inline const String &getDebugString() const { return m_debugString; }

#define INITFACTORARRAYEBUGSTRING(a)      { if(FactorArray::s_debugStringEnabled) (a)->initDebugString();      }
#define FACTORARRAYADDLAST(       a)      { if(FactorArray::s_debugStringEnabled) (a)->debugStringAddLast();   }
#define FACTORARRAYADDALL(        a, src) { if(FactorArray::s_debugStringEnabled) (a)->debugStringAddAll(src); }
#else
#define INITFACTORARRAYEBUGSTRING(a)
#define FACTORARRAYADDLAST(       a)
#define FACTORARRAYADDALL(        a, src)
#endif

public:
  FactorArray() {
  }
  explicit FactorArray(size_t capacity) : CompactArray<const ExpressionFactor*>(capacity) {
  }

  FactorArray selectConstantPositiveExponentFactors() const;
  FactorArray selectConstantNegativeExponentFactors() const;
  FactorArray selectNonConstantExponentFactors() const;
  int findFactorWithChangeableSign() const;

  inline void clear(intptr_t capacity=0) {
    CompactArray<const ExpressionFactor*>::clear(capacity);
    INITFACTORARRAYEBUGSTRING(this);
  }
  void add(const ExpressionFactor *f);
  void add(const ExpressionNode *base, const ExpressionNode *exponent = NULL);
  inline void remove(size_t index, size_t count = 1) {
    CompactArray<const ExpressionFactor*>::remove(index, count);
    INITFACTORARRAYEBUGSTRING(this);
  }
  inline void addAll(const FactorArray &src) {
    CompactArray<const ExpressionFactor*>::addAll(src);
    FACTORARRAYADDALL(this, src);
  }

  String toString() const;
  inline static bool enableDebugString(bool enabled) {
#ifdef _DEBUG
    const bool ret = s_debugStringEnabled;
    s_debugStringEnabled = enabled;
    return ret;
#else
    return false;
#endif
  }
};

class ExpressionSymbolSet : public BitSet {
public:
  ExpressionSymbolSet();
};

class ExpressionNodeHandler {
public:
  virtual bool handleNode(const ExpressionNode *n, int level) = 0;
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
  mutable unsigned int  m_marked               : 1;  // used for garbage-collection
  mutable unsigned int  m_breakPoint           : 1;  // used for DebugThread
  mutable unsigned int  m_coefficientsConstant : 1; // used by polynomials
  mutable unsigned int  m_coefChecked          : 1; // used by polynomials
  PackedSyntaxNodeInfo(ExpressionInputSymbol symbol)
    : m_symbol(symbol)
    , m_marked(0)
    , m_breakPoint(0)
    , m_coefficientsConstant(0)
    , m_coefChecked(0)
  {
  }
} ;

class ExpressionNode {
#ifdef _DEBUG
protected:
  String                m_debugString;
  static bool           s_debugStringEnabled;
  virtual void initDebugString() = 0;
public:
  inline const String &getDebugString() const { return m_debugString; }

#define INITEXPRESSIONNODEDEBUGSTRING() { if(ExpressionNode::s_debugStringEnabled) initDebugString(); }
#else
#define INITEXPRESSIONNODEDEBUGSTRING()
#endif

private:
  static SymbolOrderMap      s_orderMap;
  static InverseFunctionMap  s_inverseFunctionMap;
  const ParserTree          &m_tree;
  PackedSyntaxNodeInfo       m_info;

  Exception createAttributeNotSupportedException(const char *attribute) const;
  ExpressionNode &operator=(const ExpressionNode &src); // not implemented
  ExpressionNode(           const ExpressionNode &src); // not implemented
protected:
  static String &addLeftMargin(String &s, int level);
public:
  ExpressionNode(const ParserTree *tree, ExpressionInputSymbol symbol);
  virtual ~ExpressionNode() {}
  inline ExpressionInputSymbol       getSymbol()                    const   { return (ExpressionInputSymbol)m_info.m_symbol;                  }
  inline  const ParserTree          *getTree()                      const   { return &m_tree;                                                 }
          const Expression          *getExpr()                      const;
  String                             getSymbolName()                const;
  inline  void                       mark()                         const   { m_info.m_marked = true;                                         }
  inline  void                       unMark()                       const   { m_info.m_marked = false;                                        }
  inline  bool                       isMarked()                     const   { return m_info.m_marked;                                         }
  inline  void                       setBreakPoint()                const   { m_info.m_breakPoint = 1;                                        }
  inline  void                       clearBreakPoint()              const   { m_info.m_breakPoint = 0;                                        }
  inline  bool                       isBreakPoint()                 const   { return m_info.m_breakPoint;                                     }
  virtual bool                       hasPos()                       const   { return false;                                                   }
  virtual const SourcePosition      &getPos()                       const   { throw createAttributeNotSupportedException("Pos");              }
  virtual const ExpressionNode      *left()                         const   { return child(0);                                                }
  virtual const ExpressionNode      *right()                        const   { return child(1);                                                }
  virtual const ExpressionNode      *getArgument()                  const   { throw createAttributeNotSupportedException("Argument");         }
  virtual const ExpressionNode      *child(unsigned int i)          const   { throw createAttributeNotSupportedException("Child");            }
  virtual int                        getChildCount()                const   { throw createAttributeNotSupportedException("ChildCount");       }
  virtual const ExpressionNodeArray &getChildArray()                const   { throw createAttributeNotSupportedException("ChildArray");       }
  virtual const FactorArray         &getFactorArray()               const   { throw createAttributeNotSupportedException("FactorArray");      }
  virtual const AddentArray         &getAddentArray()               const   { throw createAttributeNotSupportedException("AddentArray");      }
  virtual const ExpressionNodeArray &getCoefficientArray()          const   { throw createAttributeNotSupportedException("CoefficientArray"); } 
  virtual int                        getDegree()                    const   { throw createAttributeNotSupportedException("Degree");           } 
  virtual const String              &getName()                      const   { throw createAttributeNotSupportedException("Name");             }
  virtual       ExpressionVariable  &getVariable()                  const   { throw createAttributeNotSupportedException("Variable");         }
  virtual const Number              &getNumber()                    const   { throw createAttributeNotSupportedException("Number");           }
  virtual       bool                 getBool()                      const   { throw createAttributeNotSupportedException("Bool");             }
  virtual const ExpressionNode      *expand()                       const   { throw createAttributeNotSupportedException("Expand");           }
  virtual bool                       isExpandable()                 const   { return false;                                                   }

  virtual int                        compare(const ExpressionNode *n) const;

  virtual const ExpressionNode      *clone(const ParserTree *tree)  const = 0;

  virtual bool                       isConstant()                   const = 0;
  virtual ExpressionNodeType         getNodeType()                  const = 0;
  virtual bool                       traverseExpression(ExpressionNodeHandler &handler, int level) const = 0;

  virtual void                       dumpNode(String &s, int level) const = 0;
  virtual String                     toString()                     const = 0;

  inline  bool                       isName()                       const   { return getSymbol() == NAME;                                     }
  inline  bool                       isNumber()                     const   { return getSymbol() == NUMBER;                                   }
  inline  bool                       isBoolean()                    const   { return getSymbol() == TYPEBOOL;                                 }
  inline  bool                       isRational()                   const   { return isNumber() && getNumber().isRational();                  }
  inline  bool                       isInteger()                    const   { return isNumber() && getNumber().isInteger();                   }
  inline  bool                       isNameOrNumber()               const   { return isName() || isNumber();                                  }
  inline  bool                       isEven()                       const   { return isRational() && getRational().isEven();                  }
  inline  bool                       isOdd()                        const   { return isRational() && getRational().isOdd();                   }
  inline  bool                       isMultiplyable()               const   { return !isNameOrNumber();                                       }
  inline  Real                       getReal()                      const   { return getNumber().getRealValue();                              }
  inline  Rational                   getRational()                  const   { return getNumber().getRationalValue();                          }
  inline  bool                       isUnaryMinus()                 const   { return (getSymbol() == MINUS) && (getChildCount() == 1);        }
  inline  bool                       isBinaryMinus()                const   { return (getSymbol() == MINUS) && (getChildCount() == 2);        }
  inline  bool                       isEulersConstant()             const   { return isName()     && getName()  == _T("e");                   }
  inline  bool                       isPi()                         const   { return isName()     && getName()  == _T("pi");                  }
  inline  bool                       isZero()                       const   { return isNumber()   && getReal()  == 0;                         }
  inline  bool                       isOne()                        const   { return isNumber()   && getReal()  == 1;                         }
  inline  bool                       isTwo()                        const   { return isNumber()   && getReal()  == 2;                         }
  inline  bool                       isTen()                        const   { return isNumber()   && getReal()  == 10;                        }
  inline  bool                       isMinusOne()                   const   { return isNumber()   && getReal()  == -1;                        }
  inline  bool                       isMinusTwo()                   const   { return isNumber()   && getReal()  == -2;                        }
  inline  bool                       isNegative()                   const   { return isNumber()   && getReal()  < 0;                          }
  inline  bool                       isPositive()                   const   { return isNumber()   && getReal()  > 0;                          }
  inline  bool                       isTrue()                       const   { return isBoolean()  && getBool();                               }
  inline  bool                       isFalse()                      const   { return isBoolean()  && !getBool();                              }
  inline  ExpressionInputSymbol      getInverseFunction() const {
    return s_inverseFunctionMap.getInverse(getSymbol());
  }

  bool    isCoefficientArrayConstant()  const;
  bool    dependsOn(const String &name) const;
  int     getNodeCount(ExpressionNodeSelector *selector = NULL) const; // If selector specified, only nodes wher selector.select(n) is true will be counted.
  bool    containsFunctionCall() const;
                                                                       // If not specified, all nodes is counted
  bool    isBinaryOperator()            const;
  bool    isBooleanOperator()           const;
  bool    isTrigonomtricFunction()      const;
  bool    isSymmetricFunction()         const;
  bool    isAsymmetricFunction()        const;
  bool    isSymmetricExponent()         const;
  bool    isAsymmetricExponent()        const;
  int     getPrecedence()               const;

  String  parenthesizedExpressionToString(const ExpressionNode  *parent, bool dbg=false)  const;
  bool    needParentheses(                const ExpressionNode  *parent)                  const;
  int     getMaxTreeDepth()                                                               const;
  String  statementListToString( bool dbg=false)                                          const;
  inline static bool enableDebugString(bool enabled) {
#ifdef _DEBUG
    const bool ret = s_debugStringEnabled;
    s_debugStringEnabled = enabled;
    return ret;
#else
    return false;
#endif
  }
};

typedef CompactKeyType<const ExpressionNode*>  ExpressionNodeKey;

template<class E> class CompactNodeHashMap : public CompactHashMap<ExpressionNodeKey, E> {
};

ExpressionNodeArray getExpressionList(const ExpressionNode *n);
ExpressionNodeArray getStatementList( const ExpressionNode *n);

class ExpressionNodeNumber : public ExpressionNode {
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  Number m_number;
public:
  inline ExpressionNodeNumber(const ParserTree *tree, const Real &value) : ExpressionNode(tree, NUMBER) {
    m_number = value;
    INITEXPRESSIONNODEDEBUGSTRING();
  }

  inline ExpressionNodeNumber(const ParserTree *tree, const Rational &value) : ExpressionNode(tree, NUMBER) {
    m_number = value;
    INITEXPRESSIONNODEDEBUGSTRING();
  }

  inline ExpressionNodeNumber(const ParserTree *tree, const Number &value) : ExpressionNode(tree, NUMBER) {
    m_number = value;
    INITEXPRESSIONNODEDEBUGSTRING();
  }

  const Real *getRealAddress() const {
    return m_number.getRealAddress();
  }

  const Number &getNumber() const {
    return m_number;
  }

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODENUMBER;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
  void dumpNode(String &s, int level) const;

  String toString() const {
    return m_number.toString();
  }
};

class ExpressionNodeBoolean : public ExpressionNode {
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  const bool m_value;
public:
  ExpressionNodeBoolean(const ParserTree *tree, bool b) : ExpressionNode(tree, TYPEBOOL), m_value(b) {
    INITEXPRESSIONNODEDEBUGSTRING();
  }
  bool getBool() const {
    return m_value;
  }

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const {
    return true;
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEBOOLEAN;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
  void dumpNode(String &s, int level) const;

  String toString() const {
    return getBool() ? _T("true") : _T("false");
  }
};

class ExpressionNodeVariable : public ExpressionNode {
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  String              m_name;
  ExpressionVariable *m_var;
public:

  ExpressionNodeVariable(const ParserTree *tree, const String &name);

  ExpressionNodeVariable(const ParserTree *tree, const String &name, ExpressionVariable &var)
    : ExpressionNode(tree, NAME)
    , m_name(name)
  {
    m_var  = &var;
    INITEXPRESSIONNODEDEBUGSTRING();
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

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const {
    return m_var->isConstant();
  }

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEVARIABLE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
  void dumpNode(String &s, int level) const;

  String toString() const {
    return getName();
  }
};

class ExpressionNodeTree : public ExpressionNode {
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  String toString(bool dbg) const;
  ExpressionNodeArray m_childArray;
  void initChildArray(va_list argptr);
protected:
  ExpressionNodeTree(const ParserTree *tree, ExpressionInputSymbol symbol, ...); // terminate arguemnt list with NULL

public:
  ExpressionNodeTree(const ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr);
  ExpressionNodeTree(const ParserTree *tree, ExpressionInputSymbol symbol, const ExpressionNodeArray &childArray);
  ExpressionNodeTree(const ParserTree *tree, const ExpressionNodeTree *src);

  const ExpressionNode *child(unsigned int i) const {
    return m_childArray[i];
  }

  int getChildCount() const {
    return (int)m_childArray.size();
  }

  const ExpressionNodeArray &getChildArray() const {
    return m_childArray;
  }

  const ExpressionNode      *expand()       const;
  bool                       isExpandable() const;

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODETREE;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodePoly : public ExpressionNode {
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  String toString(bool dbg) const;
  ExpressionNodeArray         m_coefficientArray;
  const ExpressionNode *m_argument;

public:
  ExpressionNodePoly(const ParserTree *tree, va_list argptr);
  ExpressionNodePoly(const ParserTree *tree, const ExpressionNodeArray &coefficientArray, const ExpressionNode *argument);
  ExpressionNodePoly(const ParserTree *tree, const ExpressionNodePoly *src);

  int getDegree() const {
    return (int)m_coefficientArray.size() - 1;
  }

  const ExpressionNodeArray &getCoefficientArray() const {
    return m_coefficientArray;
  }

  const ExpressionNode *getArgument() const {
    return m_argument;
  }

  const ExpressionNode *expand() const;
  bool                  isExpandable() const {
    return true;
  }

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const;

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
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
  inline ExpressionNodeNumberWithPos(const ParserTree *tree, const SourcePosition &pos, const Real &value)
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
  ExpressionNodeVariableWithPos(const ParserTree *tree, const SourcePosition &pos, const String &name)
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
  ExpressionNodeTreeWithPos(const ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
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
  ExpressionNodePolyWithPos(const ParserTree *tree, const SourcePosition &pos, va_list argptr)
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
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  AddentArray m_elements;
public:
  ExpressionNodeSum(const ParserTree *tree, const AddentArray &elements);

  const AddentArray &getAddentArray() const {
    return m_elements;
  }

  static const ExpressionNodeSum *multiply(const ExpressionNodeSum *n1, const ExpressionNodeSum *n2);

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODESUM;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
  void dumpNode(String &s, int level) const;

  String toString() const;
};

class ExpressionNodeProduct : public ExpressionNode {
#ifdef _DEBUG
protected:
  void initDebugString();
#endif
private:
  FactorArray m_factors;
public:
  ExpressionNodeProduct(const ParserTree *tree, const FactorArray &factors);

  const FactorArray &getFactorArray() const {
    return m_factors;
  }

  int compare(const ExpressionNode *n) const;

  const ExpressionNode *clone(const ParserTree *tree) const;

  bool isConstant() const;

  ExpressionNodeType getNodeType() const {
    return EXPRESSIONNODEPRODUCT;
  }

  bool traverseExpression(ExpressionNodeHandler &handler, int level) const;
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

class SNode { // Wrapper class til ExpressionNode
private:
  const ExpressionNode *m_node;
public:
  inline SNode() : m_node(NULL) {
  }
  inline SNode(const ExpressionNode *node) : m_node(node) {
  }
  SNode(const ParserTree *tree, const Rational &v);
  inline bool isEmpty() const {
    return m_node == NULL;
  }
  inline const ExpressionNode *node() const {
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

  inline const ExpressionVariable &variable() const {
    return m_node->getVariable();
  }
  inline const String       &name()  const { return m_node->getName(); }
  inline SNode               left()  const { return m_node->left();    }
  inline SNode               right() const { return m_node->right();   }
  inline SNode               child(unsigned int index) const {
    return m_node->child(index);
  }
  inline const ExpressionNodeArray &getChildArray() const {
    return m_node->getChildArray();
  }
  inline const FactorArray &getFactorArray() const {
    return m_node->getFactorArray();
  }
  inline const AddentArray &getAddentArray() const {
    return m_node->getAddentArray();
  }
  inline const ExpressionNodeArray &getCoefficientArray() const {
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
  SNode operator-(  const SNode &n) const;   // binary -
  SNode operator-() const;                  // unary  -
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
    return isEmpty() ? "" : m_node->toString();
  }

#ifdef _DEBUG
  inline const String &getDebugString() const {
    return m_node->getDebugString();
  }
#endif

  friend SNode reciprocal(const SNode &x);
  friend SNode sqrt(      const SNode &x);
  friend SNode sqr(       const SNode &x);
  friend SNode pow(       const SNode &x, const SNode &y);
  friend SNode root(      const SNode &x, const SNode &y);
  friend SNode exp(       const SNode &x);
  friend SNode ln(        const SNode &x);
  friend SNode log10(     const SNode &x);
  friend SNode sin(       const SNode &x);
  friend SNode cos(       const SNode &x);
  friend SNode tan(       const SNode &x);
  friend SNode cot(       const SNode &x);
  friend SNode asin(      const SNode &x);
  friend SNode acos(      const SNode &x);
  friend SNode atan(      const SNode &x);
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
  SNode toTree(ExpressionInputSymbol delimiter) const;
};

class SStmtList : public SNodeArray {
public:
  SStmtList() {
  };
  SStmtList(SNode n);
  SStmtList &removeUnusedAssignments();

  operator SNode() const {
    return toTree(SEMI);
  }
};

class SExprList : public SNodeArray {
public:
  SExprList() {
  };
  SExprList(const ExpressionNodeArray &a);
  operator ExpressionNodeArray() const;
};

SNode unaryExp(  ExpressionInputSymbol symbol, SNode n);
SNode binExp(    ExpressionInputSymbol symbol, SNode n1, SNode n2);
SNode condExp(   SNode condition , SNode nTrue  , SNode nFalse);
SNode polyExp(   const SExprList &coefficientArray, SNode argument);
SNode indexSum(  SNode assignStmt, SNode endExpr, SNode expr  );
SNode indexProd( SNode assignStmt, SNode endExpr, SNode expr  );
SNode assignStmt(SNode leftSide  , SNode expr);
