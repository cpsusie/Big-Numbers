#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

#define TMPVARCOUNT 5 // the first 5 elements are reserverd for temporary variables in machinecode

void ParserTree::buildSymbolTable() {
  const Array<ExpressionVariableWithValue> oldVariables = getAllVariables();

  clearSymbolTable();
  for(int i = 0; i < TMPVARCOUNT; i++) {
    m_valueTable.add(0);
  }
  allocateConstant(NULL, _T("pi"), M_PI);
  allocateConstant(NULL, _T("e") , M_E);
  buildSymbolTable(m_root);

  for(size_t i = 0; i < oldVariables.size(); i++) {
    const ExpressionVariableWithValue &oldVar = oldVariables[i];
    if(oldVar.isInput()) {
      ExpressionVariable *newVar = getVariable(oldVar.getName());
      if(newVar && newVar->isInput()) {
        m_valueTable[newVar->getValueIndex()] = oldVar.getValue();
      }
    }
  }
}

void ParserTree::clearSymbolTable() {
  m_nameTable.clear();
  m_variableTable.clear();
  m_valueTable.clear();
  m_indexNameCounter = 0;
}

void ParserTree::buildSymbolTable(ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NAME:
    allocateSymbol(n, false, false, false);
    break;

  case NUMBER:
    allocateNumber(n);
    break;

  case SUM    :
    { const AddentArray &a = n->getAddentArray();
      for(size_t i = 0; i < a.size(); i++) buildSymbolTable(a[i]->getNode());
    }
    break;
  case PRODUCT:
    { const FactorArray &a = n->getFactorArray();
      for(size_t i = 0; i < a.size(); i++) buildSymbolTable(a[i]);
    }
    break;
  case POLY   :
    { const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
      for(size_t i = 0; i < coefficientArray.size(); i++) {
        buildSymbolTable(coefficientArray[i]);
      }
      buildSymbolTable(n->getArgument());
    }
    break;
  case INDEXEDSUM    :
  case INDEXEDPRODUCT:
    buildSymbolTableIndexedExpression(n);
    break;
  case ASSIGN :
    buildSymbolTableAssign(n, false);
    break;
  default:
    { const ExpressionNodeArray &a = n->getChildArray();
      for(size_t i = 0; i < a.size(); i++) {
        buildSymbolTable(a[i]);
      }
    }
    break;
  }
}

void ParserTree::buildSymbolTableIndexedExpression(ExpressionNode *n) {
  ExpressionNode *startAssignment = n->child(0);
  ExpressionNode *loopVar         = startAssignment->left();
  ExpressionNode *endExpr         = n->child(1);
  ExpressionNode *expr            = n->child(2);
  String          loopVarName     = loopVar->getName();

  buildSymbolTableAssign(startAssignment,true);
  buildSymbolTable(endExpr);
  checkDependentOnLoopVariablesOnly(startAssignment->right());
  checkDependentOnLoopVariablesOnly(endExpr);
  if(endExpr->dependsOn(loopVarName)) {
    addError(endExpr, _T("Max limit of %s cannot depend on %s"), loopVarName.cstr(), loopVarName.cstr());
  }
  buildSymbolTable(expr);
  if(isOk()) {
    int index = *m_nameTable.get(loopVarName);
    m_nameTable.remove(loopVarName);
    m_nameTable.put(getNewTempName(),index);
  }
}

String ParserTree::getNewTempName() {
  return format(_T("$indexVariable%d"), m_indexNameCounter++);
}

String ParserTree::getNewLoopName(const String &oldName) const {
  for(int i = 1;; i++) {
    const String newName = oldName + String(i);
    if(m_nameTable.get(newName) == NULL) {
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

void ParserTree::checkDependentOnLoopVariablesOnly(ExpressionNode *n) {
  DependencyChecker checker(*this);
  n->traverseExpression(checker, 0); 
}

void ParserTree::copyValues(ParserTree &src) {
  for(Iterator<Entry<String,int> > it = src.m_nameTable.entrySet().getIterator(); it.hasNext();) {
    Entry<String,int> &entry = it.next();
    const ExpressionVariable &srcVar = src.m_variableTable[entry.getValue()];
    ExpressionVariable       *dstVar = getVariable(entry.getKey());
    if(dstVar == NULL || dstVar->isConstant() || dstVar->isLoopVar()) {
      continue;
    }
    try {
      setValue(entry.getKey(), src.m_valueTable[srcVar.getValueIndex()]);
    } catch(Exception) {
    }
  }
}

void ParserTree::buildSymbolTableAssign(ExpressionNode *n, bool loopAssignment) {
  assert(n->getSymbol() == ASSIGN);
  if(n->right()->dependsOn(n->left()->getName())) {
    addError(n->left(), _T("Variable %s cannot depend on itself"), n->left()->getName().cstr());
  }
  buildSymbolTable(n->right());
  if(!loopAssignment && n->right()->isConstant()) {
    ExpressionVariable *v = allocateConstant(n->left(), n->left()->getName(), evaluateRealExpr(n->right()));
    if(v) {
      n->left()->setVariable(v);
    }
  } else {
    allocateSymbol(n->left(), false, true, loopAssignment);
  }
}

ExpressionNode *ParserTree::allocateLoopVarNode(const String &prefix) {
  ExpressionNodeVariable *result = fetchVariableNode(getNewLoopName(prefix));
  allocateSymbol(result, false, true, true);
  return result;
}

ExpressionVariable *ParserTree::allocateSymbol(ExpressionNode *n, bool isConstant, bool isLeftSide, bool isLoopVar) {
  ExpressionVariable *v = getVariable(n->getName());
  if(v == NULL) {
    v = allocateSymbol(n->getName(), 0, isConstant, isLeftSide, isLoopVar);
  } else {
    if(isLoopVar) {
      addError(n, _T("Control variable %s has already been used"), n->getName().cstr());
    } else if(isLeftSide) {
      if(v->isConstant()) {
        addError(n, _T("Cannot assign to constant %s"), n->getName().cstr());
      } else if(v->isDefined()) {
        addError(n, _T("Variable %s has already been assigned a value"), n->getName().cstr());
      } else {
        addError(n, _T("Variable %s has already been used, and is considered to be an input parameter"), n->getName().cstr());
      }
    }
  }
  ((ExpressionNodeVariable*)n)->setVariable((ExpressionVariable*)v);
  return v;
}

ExpressionVariable *ParserTree::allocateSymbol(const String &name, const Real &value, bool isConstant, bool isLeftSide, bool isLoopVar) {
  const int varIndex   = (int)m_variableTable.size();
  const int valueIndex = (int)m_valueTable.size();
  m_valueTable.add(value);
  m_variableTable.add(ExpressionVariable(name, isConstant, isLeftSide, isLoopVar));
  m_nameTable.put(name, varIndex);
  ExpressionVariable *var = getVariable(name);
  var->setValueIndex(valueIndex);
  return var;
}

ExpressionVariable *ParserTree::allocateConstant(ExpressionNode *n, const String &name, const Real &value) {
  const ExpressionVariable *v = getVariable(name);
  if(v != NULL) {
    addError(n, _T("Constant %s has already been declared"), name.cstr());
    return NULL;
  }
  return allocateSymbol(name, value, true, true, false);
}

void ParserTree::allocateNumber(ExpressionNode *n) {
  const Real value = n->getReal();
  const int valueIndex = (int)m_valueTable.size();
  m_valueTable.add(value);
  n->setValueIndex(valueIndex);
}
