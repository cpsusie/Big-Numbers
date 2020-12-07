#pragma once

#include <BitSet.h>

class AbstractBitSetIterator : public AbstractIterator {
protected:
  BitSet &m_s;
  size_t  m_next, m_current, m_end;
  bool    m_hasNext;
  inline AbstractBitSetIterator(const BitSet *set) : m_s(*(BitSet*)set) {
  }
  inline bool hasCurrent() const {
    return m_current < m_s.getCapacity();
  }
  inline void setCurrentUndefined() {
    m_current = -1;
  }
  inline void remove(const TCHAR *method) {
    if(!hasCurrent()) noCurrentElementError(method);
    m_s.remove(m_current);
    setCurrentUndefined();
  }

public:
  bool hasNext() const override {
    return m_hasNext;
  }
};

class BitSetIterator : public AbstractBitSetIterator {
private:
  void first(size_t start, size_t end);
public:
  BitSetIterator(const BitSet *set, size_t start=0, size_t end=-1) : AbstractBitSetIterator(set) {
    first(start, end);
  }
  AbstractIterator *clone()  override;
  void             *next()   override;
  void              remove() override {
    __super::remove(__TFUNCTION__);
  }
};

class BitSetReverseIterator : public AbstractBitSetIterator {
private:
  void first(size_t start, size_t end);
public:
  BitSetReverseIterator(const BitSet *set, size_t start=-1, size_t end=0)  : AbstractBitSetIterator(set) {
    first(start, end);
  }
  AbstractIterator *clone()  override;
  void             *next()   override;
  void              remove() override {
    __super::remove(__TFUNCTION__);
  }
};
