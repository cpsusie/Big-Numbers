#include "stdafx.h"

FieldSet::FieldSet(int f,...) {
  clear();
  va_list argptr;
  va_start(argptr, f);
  for(;f >= 0; f = va_arg(argptr, int)) {
    add(f);
  }
  va_end(argptr);
}

String FieldSet::toString() const {
  String result = _T("(");
  Iterator<int> it = getIterator();
  if(it.hasNext()) {
    result += getFieldName(it.next());
    while(it.hasNext()) {
      result += ',';
      result += getFieldName(it.next());
    }
  }
  result += _T(")");
  return result;
}

class FieldSetIterator : public AbstractIterator {
  const FieldSet &m_set;
  int m_current;
  int m_next;
  void first();
public:
  FieldSetIterator(const FieldSet &set) : m_set(set) {
    first();
  }
  AbstractIterator *clone() {
    return new FieldSetIterator(*this);
  }

  bool hasNext() const {
    return m_next < 64;
  }
  void *next();
  void remove();
};

void FieldSetIterator::first() {
  for(int i = 0; i < 2; i++) {
    for(UINT b = m_set.m_bits[i], j = 0; b; j++, b >>= 1) {
      if(b & 1) {
        m_next = i*32 + j;
        return;
      }
    }
  }
  m_next = 64;
}

void *FieldSetIterator::next() {
  for(m_current = m_next++; m_next < 64; m_next = (m_next<32)?32:64) {
    for(UINT b = m_set.m_bits[m_next/32] >> (m_next%32); b; m_next++, b >>= 1) {
      if(b & 1) {
        return &m_current;
      }
    }
  }
  return &m_current;
}

void FieldSetIterator::remove() {
  unsupportedOperationError(_T("FieldSetIterator"));
}

Iterator<int> FieldSet::getIterator() const {
  return Iterator<int>(new FieldSetIterator(*this));
}
