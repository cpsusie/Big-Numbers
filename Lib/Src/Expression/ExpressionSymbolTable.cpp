#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionSymbolTable.h>

namespace Expr {

class AllocateNumbers : public ExpressionNodeHandler {
private:
  ExpressionSymbolTable &m_table;
  inline bool isExponent(const ExpressionNode *n) const {
    const ExpressionNode *parent = getParent();
    return parent && (parent->getSymbol() == POW) && (parent->exponent().node() == n);
  }
public:
  AllocateNumbers(ExpressionSymbolTable *table) : m_table(*table) {
  }
  bool handleNode(ExpressionNode *n);
};

bool AllocateNumbers::handleNode(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER:
    if(n->getValueIndex() < 0) {
      m_table.allocateNumber(n, true, isExponent(n));
    }
    break;
  case POLY  :
    { const CoefArray &coefArray = n->getCoefArray();
      n->setFirstCoefIndex((int)m_table.getValueTable().size());
      for(size_t i = 0; i < coefArray.size(); i++) {
        ExpressionNode *coef = coefArray[i].node();
        if(coef->isNumber()) {
          m_table.allocateNumber(coef, false, false);
        } else {
          m_table.insertValue(getRealNaN());
        }
      }
    }
    break;
  }
  return true;
}

class ResetValueIndex : public ExpressionNodeHandler {
public:
  bool handleNode(ExpressionNode *n) {
    if(n->getSymbol() == NUMBER) n->setValueIndex(-1);
    return true;
  }
};

#define TMPVARCOUNT 2 // the first 2 elements are reserverd for temporary variables in machinecode

ParserTree *ExpressionSymbolTable::getTree() {
  return m_expression.getTree();
}

void ExpressionSymbolTable::clear() {
  m_nameTable.clear();
  m_variableTable.clear();
  m_valueTable.clear();
  ParserTree *tree = getTree();
  if(tree) {
    tree->traverseTree(ResetValueIndex());
  }
}

void ExpressionSymbolTable::create(const ExpressionVariableArray *oldVariables) {
  clear();
  const ExpressionVariableArray tmp       = getAllVariables();
  const ExpressionVariableArray &varTable = oldVariables ? *oldVariables : tmp;

  for(int i = 0; i < TMPVARCOUNT; i++) {
    insertValue(getRealNaN());
  }
  allocateConstant(NULL, _T("pi"), M_PI);
  allocateConstant(NULL, _T("e") , M_E);

  ParserTree *tree = getTree();
  if(tree && !tree->isEmpty()) {
    buildTable(tree->getRoot());
    tree->traverseTree(AllocateNumbers(this));
    buildValueRefCountTable();

    for(size_t i = 0; i < varTable.size(); i++) {
      const ExpressionVariableWithValue &oldVar = varTable[i];
      if(oldVar.isInput()) {
        const ExpressionVariable *newVar = getVariable(oldVar.getName());
        if(newVar && newVar->isInput()) {
          setValue(newVar->getValueIndex(), oldVar.getValue());
        }
      }
    }
  }
}

void ExpressionSymbolTable::buildTable(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NUMBER        :
  case BOOLCONST     :
//    allocateNumber(n);
    break;
  case NAME          :
    allocateSymbol(n, false, false, false);
    break;
  case POLY          :
    { const CoefArray &coefArray = n->getCoefArray();
      for(size_t i = 0; i < coefArray.size(); i++) {
        buildTable(coefArray[i].node());
      }
      buildTable(n->getArgument().node());
    }
    break;
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    buildTableIndexedExpr(n);
    break;
  case ASSIGN        :
    buildTableAssign(n, false);
    break;
  default            :
    { const SNodeArray &a = n->getChildArray();
      for(size_t i = 0; i < a.size(); i++) {
        buildTable(a[i].node());
      }
    }
    break;
  }
}

void ExpressionSymbolTable::buildTableIndexedExpr(ExpressionNode *n) {
  ExpressionNode *startAssignment = n->left();
  ExpressionNode *loopVar         = startAssignment->left();
  ExpressionNode *endExpr         = n->right();
  ExpressionNode *expr            = n->child(2).node();
  const String   &loopVarName     = loopVar->getName();

  buildTableAssign(startAssignment,true);
  buildTable(endExpr);
  checkDependentOnLoopVariablesOnly(startAssignment->right());
  checkDependentOnLoopVariablesOnly(endExpr);
  if(endExpr->dependsOn(loopVarName)) {
    getTree()->addError(endExpr, _T("Max limit of %s cannot depend on %s"), loopVarName.cstr(), loopVarName.cstr());
  }
  buildTable(expr);
  if(getTree()->isOk()) {
    const UINT index = *m_nameTable.get(loopVarName.cstr());
    m_nameTable.remove(loopVarName.cstr());
  }
}

String ExpressionSymbolTable::getNewLoopName(const String &oldName) const {
  for(int i = 1;; i++) {
    const String newName = oldName + String(i);
    if(m_nameTable.get(newName.cstr()) == NULL) {
      return newName;
    }
  }
}

class DependencyChecker : public ExpressionNodeHandler {
private:
  ParserTree &m_tree;
public:
  DependencyChecker(ParserTree &tree) : m_tree(tree) {}
  bool handleNode(ExpressionNode *n);
};

bool DependencyChecker::handleNode(ExpressionNode *n) {
  if(n->isName() && !n->getVariable().isLoopVar()) {
    m_tree.addError(n, _T("Control expression in the sum/product can only depend on constants and other control variables"));
  }
  return true;
}

void ExpressionSymbolTable::checkDependentOnLoopVariablesOnly(ExpressionNode *n) {
  DependencyChecker checker(*getTree());
  n->traverseExpression(checker);
}

void ExpressionSymbolTable::buildTableAssign(ExpressionNode *n, bool loopAssignment) {
  CHECKPSYMBOL(n, ASSIGN);
  if(n->right()->dependsOn(n->left()->getName())) {
    getTree()->addError(n->left(), _T("Variable %s cannot depend on itself"), n->left()->getName().cstr());
  }
  buildTable(n->right());
  if(!loopAssignment && n->right()->isConstant()) {
    ExpressionVariable *v = allocateConstant(n->left(), n->left()->getName(), n->right()->evaluateReal());
    if(v) {
      n->left()->setVariable(v);
    }
  } else {
    allocateSymbol(n->left(), false, true, loopAssignment);
  }
}

ExpressionVariable *ExpressionSymbolTable::allocateSymbol(ExpressionNode *n, bool constant, bool leftSide, bool loopVar) {
  ExpressionVariable *v = getVariable(n->getName());
  if(v == NULL) {
    v = allocateName(n->getName(), 0, constant, leftSide, loopVar);
  } else {
    if(loopVar) {
      getTree()->addError(n, _T("Control variable %s has already been used"), n->getName().cstr());
    } else if(leftSide) {
      if(v->isConstant()) {
        getTree()->addError(n, _T("Cannot assign to constant %s"), n->getName().cstr());
      } else if(v->isDefined()) {
        getTree()->addError(n, _T("Variable %s has already been assigned a value"), n->getName().cstr());
      } else {
        getTree()->addError(n, _T("Variable %s has already been used, and is considered to be an input parameter"), n->getName().cstr());
      }
    }
  }
  n->setVariable(v);
  incrValueRefCount(v->getValueIndex());
  return v;
}

ExpressionVariable *ExpressionSymbolTable::allocateName(const String &name, const Real &value, bool constant, bool leftSide, bool loopVar) {
  if(m_nameTable.get(name.cstr()) != NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Name \"%s\" already exist"), name.cstr());
  }
  const int varIndex   = (int)m_variableTable.size();
  m_variableTable.add(ExpressionVariable(name, constant, leftSide, loopVar));
  ExpressionVariable &var = m_variableTable.last();
  m_nameTable.put(var.getName().cstr(), varIndex);
  var.setValueIndex(insertValue(value));
  return &var;
}

// assume n->getSymbol() == NAME
ExpressionVariable *ExpressionSymbolTable::allocateConstant(ExpressionNode *n, const String &name, const Real &value) {
  const ExpressionVariable *v = getVariable(name);
  if(v != NULL) {
    getTree()->addError(n, _T("Constant %s has already been declared"), name.cstr());
    return NULL;
  }
  return allocateName(name, value, true, true, false);
}

// assume n->getSymbol() == NUMBER
void ExpressionSymbolTable::allocateNumber(ExpressionNode *n, bool reuseIfExist, bool isExponent) {
  if(reuseIfExist) {
    const int index = findNumberIndexByValue(n->getReal());
    if(index >= 0) {
      n->setValueIndex(index);
      if(!isExponent || !n->isLogarithmicPowExponent()) {
        incrValueRefCount(index);
      }
      return;
    }
  }
  n->setValueIndex(insertValue(n->getReal()));
  if(!isExponent || !n->isLogarithmicPowExponent()) {
    incrValueRefCount(n->getValueIndex());
  }
}

// insert value into m_valueTable, return index
UINT ExpressionSymbolTable::insertValue(Real value) {
  const UINT index = (UINT)m_valueTable.size();
  m_valueTable.add(value);
  return index;
}

int ExpressionSymbolTable::findNumberIndexByValue(const Real &value) const {
  const BitSet varIndexSet = getVariablesIndexSet();
  for(size_t i = TMPVARCOUNT; i < m_valueTable.size(); i++) { // never reuse temp-variables
    if(!varIndexSet.contains(i) && (m_valueTable[i] == value)) {
      return (int)i;
    }
  }
  return -1;
}

BitSet ExpressionSymbolTable::getVariablesIndexSet() const {
  BitSet result(m_valueTable.size()+1);
  for(size_t i = 0; i < m_variableTable.size(); i++) {
    const ExpressionVariable &v = m_variableTable[i];
    result.add(v.getValueIndex());
  }
  return result;
}

ExpressionVariableArray ExpressionSymbolTable::getAllVariables() const {
  ExpressionVariableArray result(m_variableTable.size());
  for(size_t i = 0; i < m_variableTable.size(); i++) {
    const ExpressionVariable &var = m_variableTable[i];
    result.add(ExpressionVariableWithValue(var, getValueRef(var)));
  }
  return result;
}

void ExpressionSymbolTable::unmarkAllReferencedNodes() const {
  for(Iterator<ExpressionVariable> it = getVariablesIterator(); it.hasNext();) {
    it.next().unMark();
  }
}

void ExpressionSymbolTable::incrValueRefCount(UINT valueIndex) {
  int *count = m_valueRefCountHashMap.get(valueIndex);
  if(count) {
    (*count)++;
  } else {
    m_valueRefCountHashMap.put(valueIndex,1);
  }
}

// #define REARRANGE_VALUETABLE Doesn't work with polynomial coefficients

#ifdef REARRANGE_VALUETABLE

class IndexRefCount {
public:
  UINT m_index, m_refCount, m_newIndex;
  IndexRefCount()
    : m_index(0)
    , m_refCount(0)
    , m_newIndex(0)
  {
  }
  IndexRefCount(UINT index, UINT refCount)
    : m_index(index)
    , m_refCount(refCount)
    , m_newIndex(index)
  {
  }
  String toString() const;
};

String IndexRefCount::toString() const {
  return format(_T("Value[%2d]: %2d references, new Index[%2d]")
               ,m_index, m_refCount, m_newIndex);
}

static int indexRefCountRefCountCmp(const IndexRefCount &e1, const IndexRefCount &e2) {
  return (int)e2.m_refCount - (int)e1.m_refCount;
}

static int indexRefCountOldIndexCmp(const IndexRefCount &e1, const IndexRefCount &e2) {
  return (int)e1.m_index - (int)e2.m_index;
}

class ValueTableOptimizer : public ExpressionNodeHandler {
private:
  ExpressionSymbolTable &m_symbolTable;
  CompactArray<IndexRefCount>  m_refArray;
public:
  ValueTableOptimizer(ExpressionSymbolTable *symbolTable) : m_symbolTable(*symbolTable) {
  }
  ValueTableOptimizer &findOptimalPermutation();
  ValueTableOptimizer &savePermutation();
  bool handleNode(ExpressionNode *n);

  String toString() const;
};

ValueTableOptimizer &ValueTableOptimizer::findOptimalPermutation() {
  const UINT n = (UINT)m_symbolTable.m_valueRefCountTable.size();
  m_refArray.clear();
  m_refArray.setCapacity(n-2);
  for(UINT i = 0; i < n; i++) {
    m_refArray.add(IndexRefCount(i, m_symbolTable.m_valueRefCountTable[i]));
  }
  m_refArray.sort(2, n-2, indexRefCountRefCountCmp);
  int l = (n+1) / 2, h = l + 1;
  for(UINT i = 2; i < n; i++) {
    IndexRefCount &e = m_refArray[i];
    if((i & 1) == 0) {
      e.m_newIndex = l--;
    } else {
      e.m_newIndex = h++;
    }
  }
  m_refArray.sort(2, n - 2, indexRefCountOldIndexCmp);
  return *this;
}

ValueTableOptimizer &ValueTableOptimizer::savePermutation() {
  CompactRealArray &valueTable         = m_symbolTable.m_valueTable;
  CompactIntArray  &valueRefCountTable = m_symbolTable.m_valueRefCountTable;
  CompactRealArray tmp(valueTable);
  CompactIntArray  tmpRef(valueRefCountTable);
  UINT n = (UINT)tmp.size();
  for(UINT i = 0; i < n; i++) {
    const IndexRefCount &irc = m_refArray[i];
    tmp[   irc.m_newIndex] = valueTable[i];
    tmpRef[irc.m_newIndex] = valueRefCountTable[i];
  }
  valueTable         = tmp;
  valueRefCountTable = tmpRef;
  n = (UINT)m_symbolTable.m_variableTable.size();
  for(UINT i = 0; i < n; i++) {
    ExpressionVariable &var = m_symbolTable.m_variableTable[i];
    var.setValueIndex(m_refArray[var.getValueIndex()].m_newIndex);
  }
  m_symbolTable.getTree()->traverseTree(*this);
  return *this;
}

bool ValueTableOptimizer::handleNode(ExpressionNode *n) {
  if(n->getSymbol() == NUMBER) {
    const int oldIndex = n->getValueIndex();
    const int newIndex = m_refArray[oldIndex].m_newIndex;
    n->setValueIndex(newIndex);
  }
  return true;
}

String ValueTableOptimizer::toString() const {
  const size_t n = m_refArray.size();
  String result = _T("Optimized Value references\n");
  for(size_t i = 0; i < n; i++) {
    result += format(_T("  %s\n"), m_refArray[i].toString().cstr());
  }
  return result;

}

#endif // REARRANGE_VALUETABLE

void ExpressionSymbolTable::buildValueRefCountTable() {
  const size_t n = getValueTable().size();
  m_valueRefCountTable.clear();
  m_valueRefCountTable.setCapacity(n);
  m_valueRefCountTable.add(0,0,n);
  for(Iterator<Entry<CompactIntKeyType, int> > it = m_valueRefCountHashMap.getEntryIterator(); it.hasNext();) {
    const Entry<CompactIntKeyType, int> &e = it.next();
    m_valueRefCountTable[e.getKey()] = e.getValue();
  }
  m_valueRefCountHashMap.clear();
#ifdef REARRANGE_VALUETABLE
  ValueTableOptimizer(this).findOptimalPermutation().savePermutation();
#endif // REARRANGE_VALUETABLE
}

int ExpressionSymbolTable::findMostReferencedValueIndex(int &count) const {
  const CompactIntArray &refCountTable = getValueRefCountTable();
  const size_t           n             = refCountTable.size();
  BitSet unusableValueIndexSet(n);
  for(size_t v = 0; v < m_variableTable.size(); v++) {
    const ExpressionVariable &var = m_variableTable[v];
    if(var.isLoopVar() || var.isDefined()) {
      unusableValueIndexSet.add(var.getValueIndex());
    }
  }
  int    maxRefCount   =  0;
  UINT   mostUsedIndex = -1;
  for(UINT index = TMPVARCOUNT; index < n; index++) {
    if(unusableValueIndexSet.contains(index)) continue;
    const int rc = refCountTable[index];
    if(rc > maxRefCount) {
      maxRefCount   = rc;
      mostUsedIndex = index;
    }
  }
  count = maxRefCount;
  return mostUsedIndex;
}

bool ExpressionSymbolTable::isTempVarIndex(UINT valueIndex) { // static
  return valueIndex < TMPVARCOUNT;
}

String NameTable::toString() const {
  String result = _T("{");
  const TCHAR *del = NULL;

  for(Iterator<Entry<CompactStrIKeyType, UINT> > it = ((NameTable*)this)->getEntryIterator(); it.hasNext();) {
    const Entry<CompactStrIKeyType, UINT> &e = it.next();
    if(del) result += del; else del = _T(", ");
    result += format(_T("(%s,%u)"), e.getKey(), e.getValue());
  }
  result += _T("}");
  return result;
}

String ExpressionSymbolTable::toString() const {
  String result = _T("SymbolTable:\n");
  result += _T("  Hashmap:\n    ");
  result += m_nameTable.toString();

  result += _T("\n  Variables:\n");
  if(m_variableTable.size() > 0) {
    for(size_t i = 0; i < m_variableTable.size(); i++) {
      const ExpressionVariable &var = m_variableTable[i];
      result += format(_T("    %2d: %s\n"), (int)i, var.toString().cstr());
    }
  }
  result += _T("  ValueTable:\n  ");
  const size_t n = m_valueTable.size();
  const size_t colCount = 5;
  for(size_t i = 0; i < n; i++) {
    result += format(_T("  %2d: %-9s")
                    ,(int)i
                    ,::toString(m_valueTable[i]).cstr());
    if(((i%colCount)==(colCount-1)) || (i == n-1)) {
      result += _T("\n");
      if(i < n-1) result += _T("  ");
    };
  }

  result += valueRefCountToString();
  return result;
}

StringArray ExpressionSymbolTable::getIndexedNameArray() const {
  StringArray a;
  for(size_t i = 0; i < m_valueTable.size(); i++) {
    a.add(EMPTYSTRING);
  }
  for(size_t i = 0; i < m_variableTable.size(); i++) {
    const ExpressionVariable &var = m_variableTable[i];
    a[var.getValueIndex()] = var.getName();
  }
  int tmpCounter = 0;
  for(size_t i = 0; i < a.size(); i++) {
    if(a[i].isEmpty()) {
      if(isnan(m_valueTable[i])) {
        a[i] = format(_T("$tmp%d"), tmpCounter++);
      } else {
        a[i] = ::toString(m_valueTable[i]);
      }
    }
  }
  for(size_t i = 0; i < a.size(); i++) {
    a[i] = format(_T("value[%2d]:%s"), i, a[i].cstr());
  }
  return a;
}

String ExpressionSymbolTable::valueRefCountToString() const {
  const size_t n = m_valueRefCountTable.size();
  String result = _T("Value references\n");
  for(size_t i = 0; i < n; i++) {
    result += format(_T("  Value[%2d]:%3d references\n"),i,m_valueRefCountTable[i]);
  }
  return result;
}

}; // namespace Expr
