#pragma once

#include <CompactHashMap.h>
#include "ExpressionNode.h"

namespace Expr {

class NameTable : public CompactStrIHashMap<UINT,500> { // Variable-names are case-insensitive
public:
  String toString() const;
};

class ExpressionSymbolTable {
  friend class ValueTableOptimizer;
private:
  Expression                   &m_expression;
  // map name -> index in m_variableTable
  NameTable                     m_nameTable;
  Array<ExpressionVariable>     m_variableTable;
  mutable CompactRealArray      m_valueTable;
  CompactIntArray               m_valueRefCountTable;
  CompactIntHashMap<int,500>    m_valueRefCountHashMap;
  void                  incrValueRefCount(UINT valueIndex);
  void                  buildValueRefCountTable();
  ParserTree           *getTree();
  void                  buildTable(           ExpressionNode *n);
  void                  buildTableIndexedExpr(ExpressionNode *n);
  void                  buildTableAssign(     ExpressionNode *n, bool loopAssignment);
  ExpressionVariable   *allocateSymbol(       ExpressionNode *n                    , bool constant, bool leftSide, bool loopVar);
  ExpressionVariable   *allocateConstant(     ExpressionNode *n, const String &name, const Real &value);
  ExpressionVariable   *allocateName(         const String &name, const Real &value, bool constant, bool leftSide, bool loopVar);
  void                  allocateNumber(       ExpressionNode *n, bool reuseIfExist, bool isExponent);
  // Insert value into m_valueTable, return index of position
  UINT                  insertValue(Real value);
  // Return set with indices in m_valueTable, for all elements usedby variables
  BitSet                getVariablesIndexSet() const;
  // Find i, so m_valueTable[i] == value, and m_valueTable[i] is not used by a vaiable. return -1, if not found
  int                   findNumberIndexByValue(const Real &value) const;
  void                  checkDependentOnLoopVariablesOnly(   ExpressionNode *n);
  ExpressionSymbolTable(           const ExpressionSymbolTable   &src); // not implemented
  ExpressionSymbolTable &operator=(const ExpressionSymbolTable   &src); // not implemented
  friend class ParserTree;
  friend class AllocateNumbers;
  friend class ExpressionNodeName;
public:
  ExpressionSymbolTable(Expression *expr) : m_expression(*expr) {
  }
  void                             create(const ExpressionVariableArray *oldVariables);
  void                             clear();
  inline ExpressionVariable       *getVariable(const String &name) {
    const UINT *index = m_nameTable.get(name.cstr());
    return index ? &m_variableTable[*index] : NULL;
  }
  inline const ExpressionVariable *getVariable(const String &name) const {
    const UINT *index = m_nameTable.get(name.cstr());
    return index ? &m_variableTable[*index] : NULL;
  }
  String                           getNewLoopName(const String &oldName) const;
  inline Iterator<ExpressionVariable> getVariablesIterator() const {
    return ((ExpressionSymbolTable*)this)->m_variableTable.getIterator();
  }
  ExpressionVariableArray getAllVariables() const;
  void unmarkAllReferencedNodes() const;
  inline void setValue(UINT valueIndex, const Real &value) const {
    m_valueTable[valueIndex] = value;
  }
  inline void setValue(const String &name, const Real &value) const {
    const ExpressionVariable *v = getVariable(name);
    if(v != NULL) setValue(v->getValueIndex(), value);
  }

  inline Real getValue(UINT valueIndex) const {
    return m_valueTable[valueIndex];
  }
  inline Real &getValueRef(const ExpressionVariable &var) const {
    return m_valueTable[var.getValueIndex()];
  }
  inline Real &getValueRef(UINT valueIndex) const {
    return m_valueTable[valueIndex];
  }
  inline const CompactRealArray &getValueTable() const {
    return m_valueTable;
  }
  inline const CompactIntArray &getValueRefCountTable() const {
    return m_valueRefCountTable;
  }
  int findMostReferencedValueIndex(int &count) const; // count is out
  static bool isTempVarIndex(UINT valueIndex);
  String toString() const;
  String valueRefCountToString() const;
  StringArray getIndexedNameArray() const;
};

}; // namespace Expr
