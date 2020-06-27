#pragma once

#include <CompactStack.h>
#include <CompactHashMap.h>
#include <SourcePosition.h>
#include "SNode.h"

namespace Expr {

class ExpressionVariable {
private:
  const String m_name;
  // Index into parserTree.m_valueTable
  int          m_valueIndex;
  UINT         m_defined  : 1;
  UINT         m_marked   : 1;
  const UINT   m_constant : 1;
  const UINT   m_loopVar  : 1;

public:
  ExpressionVariable(const String &name, bool constant, bool defined, bool loopVar);
  inline const String &getName()       const    { return m_name;                    }
  inline       int     getValueIndex() const    { return m_valueIndex;              }
  inline void          setValueIndex(int index) { m_valueIndex = index;             }
  inline void          setDefined()             { m_defined    = 1;                 }
  inline void          unMark()                 { m_marked     = 0;                 }
  inline void          mark()                   { m_marked     = 1;                 }
  inline bool          isDefined()     const    { return m_defined  != 0;           }
  inline bool          isMarked()      const    { return m_marked   != 0;           }
  inline bool          isConstant()    const    { return m_constant != 0;           }
  inline bool          isLoopVar()     const    { return m_loopVar  != 0;           }
  inline bool          isInput()       const    { return (m_constant | m_defined | m_loopVar) == 0; }
  String toString(bool fillers=true) const;
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
  inline ExpressionVariableWithValue(const String &name, bool constant, bool defined, bool loopVar, Real value)
    : ExpressionVariable(name, constant, defined, loopVar)
    , m_value(value)
  {
  }

  inline Real getValue() const {
    return m_value;
  }
};

class ExpressionVariableArray : public Array<ExpressionVariableWithValue> {
public:
  ExpressionVariableArray() {
  }
  explicit ExpressionVariableArray(size_t capacity) : Array<ExpressionVariableWithValue>(capacity) {
  }
};

class ExpressionNode;
class ParserTree;

class ExpressionNodeSelector : public Selector<const ExpressionNode*> {
};

class ExpressionNodeHandler {
private:
  CompactStack<ExpressionNode*> m_path;
  friend class ExpressionNode;
protected:
  inline const CompactStack<ExpressionNode*> &getPath() const {
    return m_path;
  }
  inline const ExpressionNode *getParent(UINT i = 1) const {
    return (i < m_path.getHeight()) ? m_path.top(i) : NULL;
  }
  inline UINT getLevel() const {
    return m_path.getHeight();
  }
public:
  virtual bool handleNode(ExpressionNode *n) = 0;
};

typedef CompactKeyType<ExpressionInputSymbol> ExpressionSymbolKey;

template<typename E> class CompactSymbolHashMap : public CompactHashMap<ExpressionSymbolKey, E> {
};

class PackedSyntaxNodeInfo {
public:
  const ExpressionInputSymbol m_symbol               : 10;
          UINT                m_marked               : 1;  // used for garbage-collection
          UINT                m_breakPoint           : 1;  // used for DebugThread
  mutable UINT                m_coefArrayConstant    : 1;  // used by polynomials
  mutable UINT                m_coefArrayChecked     : 1;  // used by polynomials
  mutable UINT                m_reduced              : 1;
  PackedSyntaxNodeInfo(ExpressionInputSymbol symbol)
    : m_symbol(      symbol)
    , m_marked(           0)
    , m_breakPoint(       0)
    , m_coefArrayConstant(0)
    , m_coefArrayChecked( 0)
    , m_reduced(          0)
  {
  }
  String toString() const;
};

class ExpressionNode {
  DECLAREDEBUGSTRING;
private:
  ParserTree          &m_tree;
protected:
  PackedSyntaxNodeInfo m_info;
  ExpressionSymbolTable &getSymbolTable() const;
private:
  ExpressionNode &operator=(const ExpressionNode &src); // not implemented
  ExpressionNode(           const ExpressionNode &src); // not implemented
protected:
  static String &addLeftMargin(String &s, int level);
  Exception createAttributeNotSupportedException(const char *attribute) const;
#define UNSUPPORTEDOP() throw createAttributeNotSupportedException(__FUNCTION__)

public:
  ExpressionNode(ParserTree *tree, ExpressionInputSymbol symbol);
  virtual ~ExpressionNode() {}

  inline ExpressionInputSymbol       getSymbol()                    const   { return m_info.m_symbol;                                          }
  inline        ParserTree          &getTree()                      const   { return m_tree;                                                   }
  Expression                        &getExpression()                const;
  inline const PackedSyntaxNodeInfo &getInfo()                      const   { return m_info;                                                   }
  static String                      getSymbolName(    ExpressionInputSymbol symbol);
  static String                      getNodeTypeName(  ExpressionNodeType    nt    );
  static String                      getReturnTypeName(ExpressionReturnType  rt    );
  inline String                      getSymbolName()                const   { return getSymbolName(    getSymbol());                           }
  String                             getNodeTypeName()              const   { return getNodeTypeName(  getNodeType());                         }
  String                             getReturnTypeName()            const   { return getReturnTypeName(getReturnType());                       }
  inline  void                       mark()                                 { m_info.m_marked = true;                                          }
  inline  void                       unMark()                               { m_info.m_marked = false;                                         }
  inline  bool                       isMarked()                     const   { return m_info.m_marked;                                          }
  inline  void                       setReduced()                   const   { m_info.m_reduced = true;                                         }
  inline  bool                       isReduced()                    const   { return m_info.m_reduced;                                         }
  inline  void                       setBreakPoint()                        { m_info.m_breakPoint = 1;                                         }
  inline  void                       clearBreakPoint()                      { m_info.m_breakPoint = 0;                                         }
  inline  bool                       isBreakPoint()                 const   { return m_info.m_breakPoint;                                      }
  virtual bool                       hasPos()                       const   { return false;                                                    }
  virtual const SourcePosition      &getPos()                       const   { UNSUPPORTEDOP();                                                 }
  ExpressionNode                    *left()                         const   { return child(0).node();                                          }
  ExpressionNode                    *right()                        const   { return child(1).node();                                          }
  virtual       SNode                base()                         const   { UNSUPPORTEDOP();                                                 }
  virtual       SNode                exponent()                     const   { UNSUPPORTEDOP();                                                 }
  virtual       SNode                getArgument()                  const   { UNSUPPORTEDOP();                                                 }
  virtual       SNode                child(UINT i)                  const   { UNSUPPORTEDOP();                                                 }
  virtual int                        getChildCount()                const   { UNSUPPORTEDOP();                                                 }
  virtual const SNodeArray          &getChildArray()                const   { UNSUPPORTEDOP();                                                 }
  virtual       SNodeArray          &getChildArray()                        { UNSUPPORTEDOP();                                                 }
  virtual const AddentArray         &getAddentArray()               const   { UNSUPPORTEDOP();                                                 }
  virtual       AddentArray         &getAddentArray()                       { UNSUPPORTEDOP();                                                 }
  virtual const FactorArray         &getFactorArray()               const   { UNSUPPORTEDOP();                                                 }
  virtual       FactorArray         &getFactorArray()                       { UNSUPPORTEDOP();                                                 }
  virtual const CoefArray           &getCoefArray()                 const   { UNSUPPORTEDOP();                                                 }
  virtual       CoefArray           &getCoefArray()                         { UNSUPPORTEDOP();                                                 }
  virtual int                        getFirstCoefIndex()            const   { UNSUPPORTEDOP();                                                 }
  virtual void                       setFirstCoefIndex(int index)           { UNSUPPORTEDOP();                                                 }
  virtual bool                       isCoefArrayConstant()          const   { UNSUPPORTEDOP();                                                 }
  virtual bool                       isPositive()                   const   { UNSUPPORTEDOP();                                                 }
  virtual int                        getDegree()                    const   { UNSUPPORTEDOP();                                                 }
  virtual const String              &getName()                      const   { UNSUPPORTEDOP();                                                 }
  virtual void                       setName(const String &name)            { UNSUPPORTEDOP();                                                 }
  virtual void                       setVariable(ExpressionVariable *var)   { UNSUPPORTEDOP();                                                 }
  virtual ExpressionVariable        &getVariable()                  const   { UNSUPPORTEDOP();                                                 }
  virtual const Number              &getNumber()                    const   { UNSUPPORTEDOP();                                                 }
  virtual bool                       getBool()                      const   { UNSUPPORTEDOP();                                                 }
  virtual int                        getValueIndex()                const   { UNSUPPORTEDOP();                                                 }
  virtual void                       setValueIndex(int index)               { UNSUPPORTEDOP();                                                 }
  virtual SNode                      expand()                       const   { UNSUPPORTEDOP();                                                 }
  virtual bool                       isExpandable()                 const   { return false;                                                    }
  // return reference to lvalue
  virtual Real                      &doAssignment()                 const   { UNSUPPORTEDOP();                                                 }
  virtual ExpressionNode            *clone(ParserTree *tree)        const = 0;
  virtual ExpressionNodeType         getNodeType()                  const = 0;
  virtual ExpressionReturnType       getReturnType()                const   { return EXPR_RETURN_FLOAT;                                        }
  virtual Real                       evaluateReal()                 const;
  virtual bool                       evaluateBool()                 const   { UNSUPPORTEDOP();                                                 }

  virtual int                        compare(   const ExpressionNode *n) const;
  virtual bool                       equal(     const ExpressionNode *n) const { UNSUPPORTEDOP();                                              }
  virtual bool                       equalMinus(const ExpressionNode *n) const { UNSUPPORTEDOP();                                              }

  virtual bool                       isConstant(Number *v = NULL)   const = 0;
  virtual bool                       traverseNode(ExpressionNodeHandler &handler) = 0;

  virtual void                       dumpNode(String &s, int level) const = 0;
  virtual String                     toString()                     const = 0;

  inline  bool                       isName()                       const   { return getSymbol() == NAME;                                      }
  inline  bool                       isNumber()                     const   { return getSymbol() == NUMBER;                                    }
  inline  bool                       isBoolean()                    const   { return getSymbol() == BOOLCONST;                                 }
  inline  bool                       isRational()                   const   { return isNumber() && getNumber().isRational();                   }
  inline  bool                       isInteger()                    const   { return isNumber() && getNumber().isInteger();                    }
  inline  bool                       isNameOrNumber()               const   { return isName() || isNumber();                                   }
  inline  bool                       isEven()                       const   { return isRational() && ::isEven(getRational());                  }
  inline  bool                       isOdd()                        const   { return isRational() && ::isOdd(getRational());                   }
  inline  bool                       isMultiplyable()               const   { return !isNameOrNumber();                                        }
  inline  Real                       getReal()                      const   { return ::getReal(getNumber());                                   }
  inline  Rational                   getRational()                  const   { return ::getRational(getNumber());                               }
  inline  bool                       isUnaryMinus()                 const   { return getSymbol() == UNARYMINUS;                                }
  inline  bool                       isEulersConstant()             const   { return isName()     && getName()  == _T("e");                    }
  inline  bool                       isPi()                         const   { return isName()     && getName()  == _T("pi");                   }
  inline  bool                       isZero()                       const   { return isNumber()   && getReal()  == 0;                          }
  inline  bool                       isOne()                        const   { return isNumber()   && getReal()  == 1;                          }
  inline  bool                       isTwo()                        const   { return isNumber()   && getReal()  == 2;                          }
  inline  bool                       isTen()                        const   { return isNumber()   && getReal()  == 10;                         }
  inline  bool                       isMinusOne()                   const   { return isNumber()   && getReal()  == -1;                         }
  inline  bool                       isMinusTwo()                   const   { return isNumber()   && getReal()  == -2;                         }
  inline  bool                       isNegativeNumber()             const   { return isNumber()   && getReal()  < 0;                           }
  inline  bool                       isPositiveNumber()             const   { return isNumber()   && getReal()  > 0;                           }
  inline  bool                       isTrue()                       const   { return isBoolean()  && getBool();                                }
  inline  bool                       isFalse()                      const   { return isBoolean()  && !getBool();                               }
  inline  bool                       hasOddExponent()               const   { return exponent().isOdd();                                       }

  static  bool                       isBooleanOperator(     ExpressionInputSymbol symbol); // { AND,OR,NOT,EQ,NE,LT,LE,GT,GE }
  static  bool                       isCompareOperator(     ExpressionInputSymbol symbol); // { EQ,NE,LT,LE,GT,GE }
  static  bool                       isBinaryOperator(      ExpressionInputSymbol symbol); // { POW,ROOT,PROD,QUOT,PLUS,MINUS,MOD }
  static  bool                       isTrigonomtricFunction(ExpressionInputSymbol symbol);
  static  bool                       isSymmetricFunction(   ExpressionInputSymbol symbol);
  static  bool                       isAsymmetricFunction(  ExpressionInputSymbol symbol);
  static  ExpressionInputSymbol      getInverseFunction(    ExpressionInputSymbol symbol);
  // symbol in { EQ,NE,LE,LT,GE,GT } (EQ,NE are symmetric, the rest are changed. left<->right. ie. GE->LE, etc..
  static  ExpressionInputSymbol      reverseComparator(     ExpressionInputSymbol symbol);
  // symbol in { EQ,NE,LE,LT,GE,GT } (EQ->NE, LE->GT, etc... (this is not the same as reverseComparator)
  static  ExpressionInputSymbol      negateComparator(      ExpressionInputSymbol symbol);
  inline  ExpressionInputSymbol      getInverseFunction()           const { return getInverseFunction(getSymbol()); }
  // { AND,OR,NOT,EQ,NE,LT,LE,GT,GE }
  inline  bool                       isBooleanOperator()            const { return isBooleanOperator(     getSymbol()); }
  // { EQ,NE,LT,LE,GT,GE }
  inline  bool                       isCompareOperator()            const { return isCompareOperator(     getSymbol()); }
  // { POW,ROOT,PROD,QUOT,PLUS,MINUS,MOD }
  inline  bool                       isBinaryOperator()             const { return isBinaryOperator(      getSymbol()); }
  inline  bool                       isTrigonomtricFunction()       const { return isTrigonomtricFunction(getSymbol()); }
  virtual bool                       isSymmetricFunction()          const { return isSymmetricFunction(   getSymbol()); }
  virtual bool                       isAsymmetricFunction()         const { return isAsymmetricFunction(  getSymbol()); }

  bool                               traverseExpression(ExpressionNodeHandler &handler);
  bool                               dependsOn(const String &name)  const;
  // If selector specified, only nodes where selector.select(n) is true will be counted.
  // If not specified, all nodes are counted
  UINT                               getNodeCount(ExpressionNodeSelector *selector = NULL) const;
  UINT                               getNodeCount(const ExpressionSymbolSet &validSymbolSet) const;
  bool                               containsFunctionCall()         const;
  Real                              &getValueRef()                  const;

  bool                               isSymmetricExponent()          const;
  bool                               isAsymmetricExponent()         const;
  bool                               isLogarithmicPowExponent()     const;
  static bool                        isLogarithmicPowExponent(int e);
  bool                               isLogarithmicRoot()            const;
  static bool                        isLogarithmicRoot(const Rational &r);
  TrigonometricMode                  getTrigonometricMode()         const;
  int                                getPrecedence()                const;
  bool                               reducesToRational(        Rational *r) const;

  String                             parenthesizedExpressionToString(const ExpressionNode  *parent)  const;
  bool                               needParentheses(                const ExpressionNode  *parent)  const;
  UINT                               getMaxTreeDepth()                                               const;

#if defined(CHECK_CONSISTENCY)
  // Return nodes visited. or throw Exception if any nodes is insoncistent
  UINT    checkIsConsistent()                                             const;
  bool    isConsistentSymbolAndType()                                     const;
#endif // CHECK_CONSISTENCY
  static void checkNodeType(         const TCHAR *method, const ExpressionNode *n, ExpressionNodeType    expectedNodeType  );
  static void checkNodeType(         const TCHAR *method, const ExpressionNode *n, const NodeTypeSet    &validTypes        );
  static void checkReturnType(       const TCHAR *method, const ExpressionNode *n, ExpressionReturnType  expectedReturnType);
  static void checkSymbol(           const TCHAR *method, const ExpressionNode *n, ExpressionInputSymbol expectedSymbol    );

  void throwInvalidSymbolForTreeMode(  const TCHAR *method) const;
  void throwUnknownSymbolException(    const TCHAR *method) const;
  void throwUnknownNodeTypeException(  const TCHAR *method) const;
  void throwUnknownReturnTypeException(const TCHAR *method) const;
  void throwUnExpandableException()                         const;
};

// Should only be called in Canonical treeform
bool equal(     const ExpressionNode *n1, const ExpressionNode *n2);
bool equalMinus(const ExpressionNode *n1, const ExpressionNode *n2);

class ExpressionNodeNumber : public ExpressionNode {
private:
  Number m_number;
  int    m_valueIndex;
public:
  inline ExpressionNodeNumber(ParserTree *tree, const Real &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
    SETDEBUGSTRING();
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Rational &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
    SETDEBUGSTRING();
  }

  inline ExpressionNodeNumber(ParserTree *tree, const Number &value) : ExpressionNode(tree, NUMBER) {
    m_number     = value;
    m_valueIndex = -1;
    SETDEBUGSTRING();
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

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_NUMBER;                }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const { return ::getReal(m_number);      }
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
  bool                 isConstant(Number *v = NULL)        const;
  bool                 traverseNode(ExpressionNodeHandler &handler);
  void                 dumpNode(String &s, int level)      const;
  String               toString()                          const { return ::toString(m_number);     }
};

class ExpressionNodeBoolConst : public ExpressionNode {
private:
  const bool m_value;
public:
  inline ExpressionNodeBoolConst(ParserTree *tree, bool b) : ExpressionNode(tree, BOOLCONST), m_value(b) {
    SETDEBUGSTRING();
  }
  bool getBool() const {
    return m_value;
  }
  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_BOOLCONST;             }
  ExpressionReturnType getReturnType()                     const { return EXPR_RETURN_BOOL;         }
  Real                 evaluateReal()                      const { UNSUPPORTEDOP();                 }
  bool                 evaluateBool()                      const { return m_value;                  }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
//bool                 equalMinus(const ExpressionNode *n) const; not implemented
  bool                 isConstant(Number *v = NULL)        const { return true;                     }
  bool                 traverseNode(ExpressionNodeHandler &handler);
  void                 dumpNode(String &s, int level)      const;
  String               toString()                          const { return boolToStr(m_value);       }
};

class ExpressionNodeName : public ExpressionNode {
private:
  String              m_name;
  ExpressionVariable *m_var;
public:
  ExpressionNodeName(ParserTree *tree, const String &name);
  inline ExpressionNodeName(ParserTree *tree, const String &name, ExpressionVariable &var)
    : ExpressionNode(tree, NAME)
    , m_name(name)
  {
    m_var  = &var;
    SETDEBUGSTRING();
  }

  const String &getName() const {
    return m_name;
  }
  void setName(const String &name);
  void setVariable(ExpressionVariable *var) {
    m_var = var;
  }

  ExpressionVariable &getVariable() const {
    return *m_var;
  }

  int getValueIndex() const {
    return m_var->getValueIndex();
  }

  void setValueIndex(int index) {
    m_var->setValueIndex(index);
  }

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_VARIABLE;              }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const { return getValueRef();            }
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
  bool                 isConstant(Number *v = NULL)        const;
  bool                 traverseNode(ExpressionNodeHandler &handler);
  void                 dumpNode(String &s, int level)      const;
  String               toString()                          const { return getName();                }
};

class ExpressionNodeTree : public ExpressionNode {
private:
  SNodeArray m_childArray;
  int initChildArray(va_list argptr); // return number of arguments
protected:
  // Terminate arguemnt list with NULL. ... is a variable list of ExpressionNode*
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

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_TREE;                  }
//ExpressionReturnType getReturnType()                     const not implemented here
//Real                 evaluateReal()                      const not implemented here
//bool                 evaluateBool()                      const not implemented here
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
  bool                 isConstant(Number *v = NULL)        const;
  bool                 traverseNode(ExpressionNodeHandler &handler);
  void                 dumpNode(String &s, int level)      const;
  String               toString()                          const;
};

class ExpressionNodeBoolExpr : public ExpressionNodeTree {
public:
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, ExpressionNode *bexpr)
    : ExpressionNodeTree(tree, symbol, bexpr, NULL)
  {
    SETDEBUGSTRING();
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, ExpressionNode *left, ExpressionNode *right)
    : ExpressionNodeTree(tree, symbol, left, right, NULL)
  {
    SETDEBUGSTRING();
  }
  inline ExpressionNodeBoolExpr(ParserTree *tree, const ExpressionNodeBoolExpr *src)
    : ExpressionNodeTree(tree,src)
  {
    SETDEBUGSTRING();
  }
   inline ExpressionNodeBoolExpr(ParserTree *tree, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeTree(tree, symbol, argptr)
  {
    SETDEBUGSTRING();
  }

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_BOOLEXPR;              }
  ExpressionReturnType getReturnType()                     const { return EXPR_RETURN_BOOL;         }
  Real                 evaluateReal()                      const { UNSUPPORTEDOP();                 }
  bool                 evaluateBool()                      const;
//int                  compare(   const ExpressionNode *n) const { as ExpressionNodeTree;           }
//bool                 equal(     const ExpressionNode *n) const { as ExpressionNodeTree;           }
//bool                 equalMinus(const ExpressionNode *n) const { as ExpressionNodeTree;           }
  bool                 isConstant(Number *v = NULL)        const;
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
//void                 dumpNode(String &s, int level)      const;    as ExpressionNodeTree
  String               toString()                          const;
};

class ExpressionNodePower : public ExpressionNodeTree {
protected:
   ExpressionNodePower(ParserTree *tree, va_list argptr)
    : ExpressionNodeTree(tree, POW, argptr)
  {
    SETDEBUGSTRING();
  }
public:
  ExpressionNodePower::ExpressionNodePower(SNode base)
    : ExpressionNodeTree(&base.getTree(), POW, base.node(), base._1().node(), NULL)
  {
  }

  ExpressionNodePower::ExpressionNodePower(SNode base, SNode exponent)
    : ExpressionNodeTree(&base.getTree(), POW, base.node(), exponent.node(), NULL)
  {
  }

  SNode base() const {
    return child(0);
  }
  SNode exponent() const {
    return child(1);
  }

  SNode             expand()       const;
  bool              isExpandable() const;



  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_POWER;                 }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const;
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
  bool                 isConstant(Number *v = NULL)        const;
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
//void                 dumpNode(String &s, int level)      const;    as ExpressionNodeTree
//String               toString()                          const { as ExpressionNodeTree;            }
};

class ExpressionNodePoly : public ExpressionNode {
private:
  CoefArray m_coefArray;
  SNode     m_arg;
  int       m_firstCoefIndex;

public:
  ExpressionNodePoly(ParserTree *tree, const CoefArray &coefArray, SNode arg);
  ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src);

  int getDegree() const {
    return (int)m_coefArray.size() - 1;
  }

  const CoefArray &getCoefArray() const {
    return m_coefArray;
  }
  CoefArray &getCoefArray() {
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

  int getFirstCoefIndex() const {
    return m_firstCoefIndex;
  }

  void setFirstCoefIndex(int index) {
    m_firstCoefIndex = index;
  }

  SNode             expand()       const;
  bool              isExpandable() const {
    return true;
  }

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_POLY;                  }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const;
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
  bool                 isConstant(Number *v = NULL)        const;
  bool                 traverseNode(ExpressionNodeHandler &handler);
  void                 dumpNode(String &s, int level)      const;
  String               toString()                          const;
};

class ExpressionNodeAssign : public ExpressionNodeTree {
public:
  ExpressionNodeAssign(ParserTree *tree, va_list argptr)
    : ExpressionNodeTree(tree, ASSIGN, argptr)
  {
    SETDEBUGSTRING();
  }
  ExpressionNodeAssign(ExpressionNode *leftSide, ExpressionNode *expr)
    : ExpressionNodeTree(&(leftSide->getTree()), ASSIGN, leftSide, expr, NULL)
  {
    SETDEBUGSTRING();
  }
  ExpressionNodeAssign(ParserTree *tree, const ExpressionNodeAssign *src);
  Real &doAssignment() const;





  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_ASSIGN;                }
  ExpressionReturnType getReturnType()                     const { return EXPR_NORETURNTYPE;        }
  Real                 evaluateReal()                      const { UNSUPPORTEDOP();                 }
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const { UNSUPPORTEDOP();                 }
//bool                 equal(     const ExpressionNode *n) const { as ExpressionNodeTree;           }
//bool                 equalMinus(const ExpressionNode *n) const { as ExpressionNodeTree;           }
//bool                 isConstant(Number *v = NULL)        const { as ExpressionNodeTree;           }
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
//void                 dumpNode(String &s, int level)      const;    as ExpressionNodeTree
  String               toString()                          const;
};

class ExpressionNodeStmtList : public ExpressionNodeTree {
private:
  const ExpressionReturnType m_returnType;
  SNode doAssignments() const; // return last expression
public:
  ExpressionNodeStmtList(ParserTree *tree, const SNodeArray &childArray);
  ExpressionNodeStmtList(ParserTree *tree, const ExpressionNodeStmtList *src);





  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_STMTLIST;              }
  ExpressionReturnType getReturnType()                     const { return m_returnType;             }
  Real                 evaluateReal()                      const;
  bool                 evaluateBool()                      const;
  int                  compare(   const ExpressionNode *n) const { UNSUPPORTEDOP();                 }
//bool                 equal(     const ExpressionNode *n) const { as ExpressionNodeTree;           }
  bool                 equalMinus(const ExpressionNode *n) const;
//bool                 isConstant(Number *v = NULL)        const { as ExpressionNodeTree;           }
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
//void                 dumpNode(String &s, int level)      const;    as ExpressionNodeTree
  String               toString()                          const;
};

class ExpressionNodeAddent : public ExpressionNodeTree {
private:
  bool m_positive;
public:
  ExpressionNodeAddent(SNode n, bool positive);
  ExpressionNodeAddent(ParserTree *tree, const ExpressionNodeAddent *src);

  bool isPositive() const {
    return m_positive;
  }

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_ADDENT;                }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const;
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
//bool                 isConstant(Number *v = NULL)        const { as ExpressionNodeTree            }
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
  void                 dumpNode(String &s, int level)      const;
  String               toString()                          const;
};

class ExpressionNodeSum : public ExpressionNodeTree {
private:
  void validateNodeArray(const AddentArray &addentArray) const; // check, that all nodes have type NT_ADDENT
public:
  ExpressionNodeSum(ParserTree *tree, const AddentArray &addentArray);
  ExpressionNodeSum(ParserTree *tree, const ExpressionNodeSum *src);

  AddentArray &getAddentArray() {
    return (AddentArray&)getChildArray();
  }
  const AddentArray &getAddentArray() const {
    return (AddentArray&)getChildArray();
  }

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_SUM;                   }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const;
//bool                 evaluateBool()                      const { as ExpressionNode                }
//int                  compare(   const ExpressionNode *n) const { as ExpressionNodeTree;           }
//bool                 equal(     const ExpressionNode *n) const { as ExpressionNodeTree;           }
  bool                 equalMinus(const ExpressionNode *n) const;
//bool                 isConstant(Number *v = NULL)        const;
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
//void                 dumpNode(String &s, int level)      const;    as ExpressionNodeTree
  String               toString()                          const;
};

class ExpressionNodeProduct : public ExpressionNodeTree {
private:
  void validateFactorArray(const FactorArray &a) const; // check, that all nodes have type NT_POWER
public:
  ExpressionNodeProduct(ParserTree *tree, const FactorArray &factors);
  ExpressionNodeProduct(ParserTree *tree, const ExpressionNodeProduct *src);

  FactorArray &getFactorArray() {
    return (FactorArray&)getChildArray();
  }
  const FactorArray &getFactorArray() const {
    return (FactorArray&)getChildArray();
  }

  ExpressionNode      *clone(ParserTree *tree)             const;
  ExpressionNodeType   getNodeType()                       const { return NT_PRODUCT;               }
//ExpressionReturnType getReturnType()                     const { as ExpressionNode                }
  Real                 evaluateReal()                      const;
//bool                 evaluateBool()                      const { as ExpressionNode                }
  int                  compare(   const ExpressionNode *n) const;
  bool                 equal(     const ExpressionNode *n) const;
  bool                 equalMinus(const ExpressionNode *n) const;
//bool                 isConstant(Number *v = NULL)        const { as ExpressionNodeTree;           }
//bool                 traverseNode(ExpressionNodeHandler &handler); as ExpressionNodeTree
//void                 dumpNode(String &s, int level)      const;    as ExpressionNodeTree
  String               toString()                          const;
};

// ----------------------------------- Used by parser to save sourceposition in text -------------------------

#define DEFINESOURCEPOSTRAITS                      \
  const SourcePosition &getPos() const {           \
    return m_pos;                                  \
  }                                                \
  bool hasPos() const {                            \
    return true;                                   \
  }

class ExpressionNodeNumberWithPos : public ExpressionNodeNumber {
private:
  const SourcePosition m_pos;
public:
  inline ExpressionNodeNumberWithPos(ParserTree *tree, const SourcePosition &pos, const Real &value)
    : ExpressionNodeNumber(tree, value), m_pos(pos)
  {
  }
  DEFINESOURCEPOSTRAITS
};

class ExpressionNodeNameWithPos : public ExpressionNodeName {
private:
  const SourcePosition m_pos;
public:
  ExpressionNodeNameWithPos(ParserTree *tree, const SourcePosition &pos, const String &name)
    : ExpressionNodeName(tree, name), m_pos(pos)
  {
  }
  DEFINESOURCEPOSTRAITS
};

class ExpressionNodeTreeWithPos : public ExpressionNodeTree {
private:
  const SourcePosition m_pos;
public:
  ExpressionNodeTreeWithPos(ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeTree(tree, symbol, argptr), m_pos(pos)
  {
  }
  DEFINESOURCEPOSTRAITS
};

class ExpressionNodeBoolExprWithPos : public ExpressionNodeBoolExpr {
private:
  const SourcePosition m_pos;
public:
  ExpressionNodeBoolExprWithPos(ParserTree *tree, const SourcePosition &pos, ExpressionInputSymbol symbol, va_list argptr)
    : ExpressionNodeBoolExpr(tree, symbol, argptr), m_pos(pos)
  {
  }
  DEFINESOURCEPOSTRAITS
};

class ExpressionNodePowerWithPos : public ExpressionNodePower {
private:
  const SourcePosition m_pos;
public:
  ExpressionNodePowerWithPos(ParserTree *tree, const SourcePosition &pos, va_list argptr)
    : ExpressionNodePower(tree, argptr), m_pos(pos)
  {
  }
  DEFINESOURCEPOSTRAITS
};

class ExpressionNodePolyWithPos : public ExpressionNodePoly {
private:
  const SourcePosition m_pos;
public:
  ExpressionNodePolyWithPos(ParserTree *tree, const SourcePosition &pos, const CoefArray &coefArray, SNode arg)
    : ExpressionNodePoly(tree, coefArray, arg)
  {
  }
  DEFINESOURCEPOSTRAITS
};

class ExpressionNodeAssignWithPos : public ExpressionNodeAssign {
private:
  const SourcePosition m_pos;
public:
  ExpressionNodeAssignWithPos(ParserTree *tree, const SourcePosition &pos, va_list argptr)
    : ExpressionNodeAssign(tree, argptr), m_pos(pos)
  {
  }
  DEFINESOURCEPOSTRAITS
};

// ------------------------------------------------------------------------------------------------------

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
