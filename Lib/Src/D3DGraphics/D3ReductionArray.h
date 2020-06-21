#pragma once

#include <D3DGraphics/D3Math.h>

class D3Index : public Vertex {
public:
  UINT m_origIndex;
  UINT m_translateTo;
  inline D3Index() : m_origIndex(-1), m_translateTo(-1) {
  }
  inline D3Index(const Vertex &v, UINT origIndex) : Vertex(v), m_origIndex(origIndex), m_translateTo(-1) {
  }
};

class D3ReductionArray : public CompactArray<D3Index> {
private:
  const float      m_eps;
  VertexArray      m_reducedArray;
  CompactUintArray m_translateTable;
  D3ReductionArray &sort();
  D3ReductionArray &setTranslation();
  static float findEps(const VertexArray &a);
public:
  D3ReductionArray(const VertexArray &a);
  const VertexArray &getReducedArray() const {
    return m_reducedArray;
  }
  const CompactUintArray &getTranslateTable() const {
    return m_translateTable;
  }
};
