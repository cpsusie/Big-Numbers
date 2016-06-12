#include "pch.h"
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/SumElement.h>

void AddentArray::add(const ExpressionNode *n, bool positive) {
  switch(n->getSymbol()) {
  case NUMBER:
    if(!n->isZero()) {      // dont add Zero
      add(new SumElement(n, positive));
    }
    break;
  case SUM   :
    if(positive) {
      addAll(n->getAddentArray());
    } else {
      const AddentArray &a = n->getAddentArray();
      for(size_t i = 0; i < a.size(); i++) {
        const SumElement *e = a[i];
        add(e->getNode(), !e->isPositive());
      }
    }
    break;
  default    :
    add(new SumElement(n, positive));
    break;
  }
}

String AddentArray::toString() const {
  if(size() == 0) {
    return _T("0");
  }
  String result = (*this)[0]->toString();
  for(size_t i = 1; i < size(); i++) {
    const SumElement *e = (*this)[i];
    result += e->isPositive() ? _T("+") : _T("-");
    result += e->getNode()->toString();
  }
  return result;
}

#ifdef _DEBUG

bool AddentArray::s_debugStringEnabled = getDebuggerPresent();

void AddentArray::initDebugString() {
  if(size() == 0) {
    m_debugString = _T("0");
  } else {
    m_debugString = (*this)[0]->getDebugString();
    for(int i = 1; i < size(); i++) {
      const SumElement *e = (*this)[i];
      m_debugString += e->getDebugString();
    }
  }
}

void AddentArray::debugStringAddLast() {
  if(size() == 1) {
    m_debugString = last()->getDebugString();
  } else {
    m_debugString += last()->getDebugString();
  }
}

void AddentArray::debugStringAddAll(const AddentArray &src) {
  if(size() == src.size()) {
    m_debugString = src.getDebugString();
  } else {
    m_debugString += src.getDebugString();
  }
}

#endif
