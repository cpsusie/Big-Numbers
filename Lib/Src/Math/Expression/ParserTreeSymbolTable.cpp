#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

void ParserTree::buildSymbolTable() {
  const Array<ExpressionVariable> oldVariables = getVariables();

  clearSymbolTable();
  allocateConstant(NULL, "pi", M_PI);
  allocateConstant(NULL, "e" , M_E);
  buildSymbolTable(m_root);

  for(size_t i = 0; i < oldVariables.size(); i++) {
    const ExpressionVariable &oldVar = oldVariables[i];
    if(oldVar.isInput()) {
      ExpressionVariable *newVar = getVariable(oldVar.getName());
      if(newVar && newVar->isInput()) {
        newVar->setValue(oldVar.getValue());
      }
    }
  }
}

void ParserTree::clearSymbolTable() {
  m_symbolTable.clear();
  m_variables.clear();
  m_indexNameCounter = 0;
}

void ParserTree::buildSymbolTable(const ExpressionNode *n) {
  switch(n->getSymbol()) {
  case NAME:
    allocateSymbol(n, false, false, false);
    break;

  case NUMBER:
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
  case INDEXEDSUM:
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

void ParserTree::buildSymbolTableIndexedExpression(const ExpressionNode *n) {
  const ExpressionNode *startAssignment = n->child(0);
  const ExpressionNode *loopVar         = startAssignment->left();
  const ExpressionNode *endExpr         = n->child(1);
  const ExpressionNode *expr            = n->child(2);
  String                      loopVarName     = loopVar->getName();

  buildSymbolTableAssign(startAssignment,true);
  buildSymbolTable(endExpr);
  checkDependentOnLoopVariablesOnly(startAssignment->right());
  checkDependentOnLoopVariablesOnly(endExpr);
  if(endExpr->dependsOn(loopVarName)) {
    addError(endExpr, _T("Max limit of %s cannot depend on %s"), loopVarName.cstr(), loopVarName.cstr());
  }
  buildSymbolTable(expr);
  if(isOk()) {
    int index = *m_symbolTable.get(loopVarName);
    m_symbolTable.remove(loopVarName);
    m_symbolTable.put(getNewTempName(),index);
  }
}

String ParserTree::getNewTempName() {
  return format(_T("$indexVariable%d"), m_indexNameCounter++);
}

String ParserTree::getNewLoopName(const String &oldName) const {
  for(int i = 1;; i++) {
    const String newName = oldName + String(i);
    if(m_symbolTable.get(newName) == NULL) {
      return newName;
    }
  }
}

class DependencyChecker : public ExpressionNodeHandler {
private:
  ParserTree &m_tree;
public:
  DependencyChecker(ParserTree &tree) : m_tree(tree) {}
  bool handleNode(const ExpressionNode *n, int level);
};

bool DependencyChecker::handleNode(const ExpressionNode *n, int level) {
  if(n->isName() && !n->getVariable().isLoopVar()) {
    m_tree.addError(n, _T("Control expression in the sum/product can only depend on constants and other control variables"));
  }
  return true;
}

void ParserTree::checkDependentOnLoopVariablesOnly(const ExpressionNode *n) {
  DependencyChecker checker(*this);
  n->traverseExpression(checker, 0); 
}

void ParserTree::copyValues(ParserTree &src) {
  for(Iterator<Entry<String,int> > it = src.m_symbolTable.entrySet().getIterator(); it.hasNext();) {
    Entry<String,int> &entry = it.next();
    const ExpressionVariable &srcVar = src.m_variables[entry.getValue()];
    ExpressionVariable       *dstVar = getVariable(entry.getKey());
    if(dstVar == NULL || dstVar->isConstant() || dstVar->isLoopVar()) {
      continue;
    }
    try {
      setValue(entry.getKey(), srcVar.getValue());
    } catch(Exception) {
    }
  }
}

void ParserTree::buildSymbolTableAssign(const ExpressionNode *n, bool loopAssignment) {
  assert(n->getSymbol() == ASSIGN);
  if(n->right()->dependsOn(n->left()->getName())) {
    addError(n->left(), _T("Variable %s cannot depend on itself"), n->left()->getName().cstr());
  }
  buildSymbolTable(n->right());
  if(!loopAssignment && n->right()->isConstant()) {
    ExpressionVariable *v = allocateConstant(n->left(), n->left()->getName(), evaluateRealExpr(n->right()));
    if(v) {
      ((ExpressionNodeVariable*)n->left())->setVariable(v);
    }
  } else {
    allocateSymbol(n->left(), false, true, loopAssignment);
  }
}

const ExpressionNode *ParserTree::allocateLoopVarNode(const String &prefix) const {
  const ExpressionNodeVariable *result = fetchVariableNode(getNewLoopName(prefix));
  allocateSymbol(result, false, true, true);
  return result;
}

ExpressionVariable *ParserTree::allocateSymbol(const ExpressionNode *n, bool isConstant, bool isLeftSide, bool isLoopVar) const {
  const ExpressionVariable *v = getVariable(n->getName());
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
  return (ExpressionVariable*)v;
}

ExpressionVariable *ParserTree::allocateSymbol(const String &name, const Real &value, bool isConstant, bool isLeftSide, bool isLoopVar) const {
  const int index = (int)m_variables.size();
  m_variables.add(ExpressionVariable(name, value, isConstant, isLeftSide, isLoopVar));
  m_symbolTable.put(name, index);
  return (ExpressionVariable*)getVariable(name);
}

ExpressionVariable *ParserTree::allocateConstant(const ExpressionNode *n, const String &name, const Real &value) const {
  const ExpressionVariable *v = getVariable(name);
  if(v != NULL) {
    addError(n, _T("Constant %s has already been declared"), name.cstr());
    return NULL;
  }
  return allocateSymbol(name, value, true, true, false);
}
