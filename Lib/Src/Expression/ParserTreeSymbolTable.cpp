#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

class AllocateNumbers : public ExpressionNodeHandler {
private:
  ParserTreeSymbolTable &m_table;
public:
  AllocateNumbers(ParserTreeSymbolTable *table) : m_table(*table) {
  }
  bool handleNode(ExpressionNode *n, int level);
};

bool AllocateNumbers::handleNode(ExpressionNode *n, int level) {
  switch(n->getSymbol()) {
  case NUMBER:
    if(n->getValueIndex() < 0) {
      m_table.allocateNumber(n,true);
    }
    break;
  case POLY  :
    { const SNodeArray &coefArray = n->getCoefArray();
      n->setFirstCoefIndex((int)m_table.getValueTable().size());
      for(size_t i = 0; i < coefArray.size(); i++) {
        ExpressionNode *coef = coefArray[i].node();
        if(coef->isNumber()) {
          m_table.allocateNumber(coef,false);
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
  bool handleNode(ExpressionNode *n, int level) {
    if(n->getSymbol() == NUMBER) n->setValueIndex(-1);
    return true;
  }
};

#define TMPVARCOUNT 2 // the first 2 elements are reserverd for temporary variables in machinecode

void ParserTreeSymbolTable::init() {
  m_tree             = NULL;
}

void ParserTreeSymbolTable::clear(ParserTree *tree) {
  m_nameTable.clear();
  m_variableTable.clear();
  m_valueTable.clear();
  if(tree) {
    tree->traverseTree(ResetValueIndex());
  }
}

void ParserTreeSymbolTable::create(ParserTree *tree, const ExpressionVariableArray *oldVariables) {
  clear(tree);
  m_tree = tree;
  const ExpressionVariableArray tmp       = getAllVariables();
  const ExpressionVariableArray &varTable = oldVariables ? *oldVariables : tmp;

  for(int i = 0; i < TMPVARCOUNT; i++) {
    insertValue(getRealNaN());
  }
  allocateConstant(NULL, _T("pi"), M_PI);
  allocateConstant(NULL, _T("e") , M_E);
  if(!tree->isEmpty()) {
    buildTable(tree->getRoot());
    m_tree->traverseTree(AllocateNumbers(this));
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

void ParserTreeSymbolTable::buildTable(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NAME    :
    allocateSymbol(n, false, false, false);
    break;

  case NUMBER  :
  case TYPEBOOL:
//    allocateNumber(n);
    break;

  case POLY    :
    { const SNodeArray &coefArray = n->getCoefArray();
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
  case ASSIGN :
    buildTableAssign(n, false);
    break;
  default:
    { const SNodeArray &a = n->getChildArray();
      for(size_t i = 0; i < a.size(); i++) {
        buildTable(a[i].node());
      }
    }
    break;
  }
}

void ParserTreeSymbolTable::buildTableIndexedExpr(ExpressionNode *n) {
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
    m_tree->addError(endExpr, _T("Max limit of %s cannot depend on %s"), loopVarName.cstr(), loopVarName.cstr());
  }
  buildTable(expr);
  if(m_tree->isOk()) {
    const UINT index = *m_nameTable.get(loopVarName.cstr());
    m_nameTable.remove(loopVarName.cstr());
  }
}

String ParserTreeSymbolTable::getNewLoopName(const String &oldName) const {
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
  bool handleNode(ExpressionNode *n, int level);
};

bool DependencyChecker::handleNode(ExpressionNode *n, int level) {
  if(n->isName() && !n->getVariable().isLoopVar()) {
    m_tree.addError(n, _T("Control expression in the sum/product can only depend on constants and other control variables"));
  }
  return true;
}

void ParserTreeSymbolTable::checkDependentOnLoopVariablesOnly(ExpressionNode *n) {
  DependencyChecker checker(*m_tree);
  n->traverseExpression(checker, 0);
}

void ParserTreeSymbolTable::buildTableAssign(ExpressionNode *n, bool loopAssignment) {
  CHECKPSYMBOL(n, ASSIGN);
  if(n->right()->dependsOn(n->left()->getName())) {
    m_tree->addError(n->left(), _T("Variable %s cannot depend on itself"), n->left()->getName().cstr());
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

ExpressionVariable *ParserTreeSymbolTable::allocateSymbol(ExpressionNode *n, bool isConstant, bool isLeftSide, bool isLoopVar) {
  ExpressionVariable *v = getVariable(n->getName());
  if(v == NULL) {
    v = allocateName(n->getName(), 0, isConstant, isLeftSide, isLoopVar);
  } else {
    if(isLoopVar) {
      m_tree->addError(n, _T("Control variable %s has already been used"), n->getName().cstr());
    } else if(isLeftSide) {
      if(v->isConstant()) {
        m_tree->addError(n, _T("Cannot assign to constant %s"), n->getName().cstr());
      } else if(v->isDefined()) {
        m_tree->addError(n, _T("Variable %s has already been assigned a value"), n->getName().cstr());
      } else {
        m_tree->addError(n, _T("Variable %s has already been used, and is considered to be an input parameter"), n->getName().cstr());
      }
    }
  }
  n->setVariable(v);
  incrValueRefCount(v->getValueIndex());
  return v;
}

ExpressionVariable *ParserTreeSymbolTable::allocateName(const String &name, const Real &value, bool isConstant, bool isLeftSide, bool isLoopVar) {
  if(m_nameTable.get(name.cstr()) != NULL) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Name <%s> already exist"), name.cstr());
  }
  const int varIndex   = (int)m_variableTable.size();
  m_variableTable.add(ExpressionVariable(name, isConstant, isLeftSide, isLoopVar));
  ExpressionVariable &var = m_variableTable.last();
  m_nameTable.put(var.getName().cstr(), varIndex);
  var.setValueIndex(insertValue(value));
  return &var;
}

// assume n->getSymbol() == NAME
ExpressionVariable *ParserTreeSymbolTable::allocateConstant(ExpressionNode *n, const String &name, const Real &value) {
  const ExpressionVariable *v = getVariable(name);
  if(v != NULL) {
    m_tree->addError(n, _T("Constant %s has already been declared"), name.cstr());
    return NULL;
  }
  return allocateName(name, value, true, true, false);
}

// assume n->getSymbol() == NUMBER
void ParserTreeSymbolTable::allocateNumber(ExpressionNode *n, bool reuseIfExist) {
  if(reuseIfExist) {
    const int index = findNumberIndexByValue(n->getReal());
    if(index >= 0) {
      n->setValueIndex(index);
      incrValueRefCount(index);
      return;
    }
  }
  n->setValueIndex(insertValue(n->getReal()));
  incrValueRefCount(n->getValueIndex());
}

// insert value into m_valueTable, return index
UINT ParserTreeSymbolTable::insertValue(Real value) {
  const UINT index = (UINT)m_valueTable.size();
  m_valueTable.add(value);
  return index;
}

int ParserTreeSymbolTable::findNumberIndexByValue(const Real &value) const {
  const BitSet varIndexSet = getVariablesIndexSet();
  for(size_t i = TMPVARCOUNT; i < m_valueTable.size(); i++) { // never reuse temp-variables
    if(!varIndexSet.contains(i) && (m_valueTable[i] == value)) {
      return (int)i;
    }
  }
  return -1;
}

BitSet ParserTreeSymbolTable::getVariablesIndexSet() const {
  BitSet result(m_valueTable.size()+1);
  for (size_t i = 0; i < m_variableTable.size(); i++) {
    const ExpressionVariable &v = m_variableTable[i];
    result.add(v.getValueIndex());
  }
  return result;
}

ExpressionVariableArray ParserTreeSymbolTable::getAllVariables() const {
  ExpressionVariableArray result(m_variableTable.size());
  for (size_t i = 0; i < m_variableTable.size(); i++) {
    const ExpressionVariable &var = m_variableTable[i];
    result.add(ExpressionVariableWithValue(var, getValueRef(var)));
  }
  return result;
}

void ParserTreeSymbolTable::unmarkAllReferencedNodes() const {
  for(Iterator<ExpressionVariable> it = getVariablesIterator(); it.hasNext();) {
    it.next().unMark();
  }
}

void ParserTreeSymbolTable::incrValueRefCount(UINT valueIndex) {
  int *count = m_valueRefCountHashMap.get(valueIndex);
  if(count) {
    (*count)++;
  } else {
    m_valueRefCountHashMap.put(valueIndex,1);
  }
}

void ParserTreeSymbolTable::buildValueRefCountTable() {
  const size_t n = getValueTable().size();
  m_valueRefCountTable.clear();
  m_valueRefCountTable.setCapacity(n);
  m_valueRefCountTable.add(0,0,n);
  for(Iterator<Entry<CompactIntKeyType, int> > it = m_valueRefCountHashMap.getEntryIterator(); it.hasNext();) {
    const Entry<CompactIntKeyType, int> &e = it.next();
    m_valueRefCountTable[e.getKey()] = e.getValue();
  }
  m_valueRefCountHashMap.clear();
}

int ParserTreeSymbolTable::findMostReferencedValueIndex(int &count) const {
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

bool ParserTreeSymbolTable::isTempVarIndex(UINT valueIndex) { // static
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

String ParserTreeSymbolTable::toString() const {
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

StringArray ParserTreeSymbolTable::getIndexedNameArray() const {
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
      if(isNan(m_valueTable[i])) {
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

String ParserTreeSymbolTable::valueRefCountToString() const {
  const size_t n = m_valueRefCountTable.size();
  String result = _T("Value references\n");
  for(size_t i = 0; i < n; i++) {
    result += format(_T("  Value[%2d]:%3d references\n"),i,m_valueRefCountTable[i]);
  }
  return result;
}

}; // namespace Expr
