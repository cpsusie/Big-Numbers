#pragma once

#include <BitMatrix.h>
#include "BitSetIterator.h"

class BitMatrixIterator : public BitSetIterator {
private:
  DECLARECLASSNAME;
  BitMatrix  &m_matrix;
  MatrixIndex m_p;
  inline bool hasCurrent() const {
    return m_p.r < m_matrix.getRowCount();
  }
  inline void setCurrentUndefined() {
    m_p.r = -1;
  }

public:
  BitMatrixIterator(BitMatrix &m) : BitSetIterator(m), m_matrix(m) {
    setCurrentUndefined();
  }
  AbstractIterator *clone()  override;
  void             *next()   override;
  void              remove() override;
};

class BitMatrixRowIterator : public BitSetIterator {
private:
  DECLARECLASSNAME;
  BitMatrix  &m_matrix;
  MatrixIndex m_p;
  static inline size_t firstIndex(const BitMatrix &m, size_t r) {
    return m.getIndex(r,0);
  }
  static inline size_t lastIndex(const BitMatrix &m, size_t r) {
    return m.getIndex(r,m.getColumnCount()-1);
  }
  inline bool hasCurrent() const {
    return m_p.c < m_matrix.getColumnCount();
  }
  inline void setCurrentUndefined() {
    m_p.c = -1;
  }
public:
  BitMatrixRowIterator(BitMatrix &m, size_t r)
    : BitSetIterator(m, firstIndex(m,r), lastIndex(m,r))
    , m_matrix(m)
  {
    setCurrentUndefined();
  }
  AbstractIterator *clone()  override;
  void             *next()   override;
  void              remove() override;
};

class BitMatrixColumnIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  BitMatrix  &m_matrix;
  MatrixIndex m_p, m_next;
  bool        m_hasNext;
  void first(size_t c);
  inline bool hasCurrent() const {
    return m_p.r < m_matrix.getRowCount();
  }
  inline void setCurrentUndefined() {
    m_p.r = -1;
  }
public:
  BitMatrixColumnIterator(BitMatrix &m, size_t c) : m_matrix(m) {
    first(c);
  }
  AbstractIterator *clone()         override;
  bool              hasNext() const override {
    return m_hasNext;
  }
  void             *next()           override;
  void              remove()         override;
};
