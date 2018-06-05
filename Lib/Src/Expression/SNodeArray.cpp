#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

static SNodeArray &getListFromTree(ExpressionNode *n, ExpressionInputSymbol delimiterSymbol, SNodeArray &list) {
  if(n->getSymbol() == delimiterSymbol) {
    getListFromTree(n->left(), delimiterSymbol,list);
    list.add(n->right());
  } else {
    list.add(n);
  }
  return list;
}

void SNodeArray::convertFromParserTree(ExpressionNode *n, ExpressionInputSymbol delimiterSymbol) {
  DISABLEDEBUGSTRING(*this);
  getListFromTree(n,delimiterSymbol, *this);
  ENABLEDEBUGSTRING(*this);
}

void SNodeArray::clear(intptr_t capacity) {
  __super::clear(capacity);
  SETDEBUGSTRING();
}

void SNodeArray::add(SNode n) {
  __super::add(n);
  SETDEBUGSTRING();
}

void SNodeArray::remove(size_t i) {
  __super::remove(i);
  SETDEBUGSTRING();
}

void SNodeArray::addAll(const SNodeArray &a) {
  __super::addAll(a);
  SETDEBUGSTRING();
}

void SNodeArray::sort(int (*cmp)(const SNode &e1, const SNode &e2)) {
  __super::sort(cmp);
  SETDEBUGSTRING();
}

bool SNodeArray::isConstant() const {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i].isConstant()) return false;
  }
  return true;
}

bool SNodeArray::isSameNodes(const SNodeArray &a) const {
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i].isSameNode(a[i])) {
      return false;
    }
  }
  return true;
}

bool SNodeArray::equal(const SNodeArray &a) const { // recursive compare all nodes
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!Expr::equal((*this)[i].node(), a[i].node())) {
      return false;
    }
  }
  return true;
}

bool SNodeArray::equalMinus(const SNodeArray &a) const { // recursive compare all nodes ( deep compare)
  const size_t n = size();
  if(a.size() != n) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!Expr::equalMinus((*this)[i].node(), a[i].node())) {
      return false;
    }
  }
  return true;
}

SNodeArray &SNodeArray::cloneNodes(SNodeArray &dst, ParserTree *tree) const {
  DISABLEDEBUGSTRING(dst);
  dst.clear(-1);
  dst.setCapacity(size());

  for(size_t i = 0; i < size(); i++) {
    dst.add((*this)[i].node()->clone(tree));
  }
  ENABLEDEBUGSTRING(dst);
  return dst;
}

}; // namespace Expr
