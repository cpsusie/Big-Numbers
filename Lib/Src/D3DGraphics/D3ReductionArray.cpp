#include "pch.h"
#include <D3DGraphics/D3Cube.h>
#include "D3ReductionArray.h"

class D3IndexVertexComparator : public Comparator<D3Index> {
public:
  int compare(const D3Index &v1, const D3Index &v2);
  AbstractComparator *clone() const {
    return new D3IndexVertexComparator;
  }
};


int D3IndexVertexComparator::compare(const D3Index &v1, const D3Index &v2) {
  const D3DXVECTOR3 &p1 = v1.getPos(), &p2 = v2.getPos();
  int c = sign(p1.x - p2.x);
  if(c) return c;
  c = sign(p1.y - p2.y);
  if(c) return c;
  return sign(p1.z - p2.z);
}

D3ReductionArray &D3ReductionArray::sort() {
  __super::sort(D3IndexVertexComparator());
  return *this;
}

float D3ReductionArray::findEps(const VertexArray &a) { // static
  const D3Cube      cube = getBoundingBox(a);
  const D3DXVECTOR3 diag = cube.getMax() - cube.getMin();
  return length(diag) * 1e-4f;
}

D3ReductionArray::D3ReductionArray(const VertexArray &a)
: CompactArray<D3Index>(a.size())
, m_eps(findEps(a))
{
  UINT index = 0;
  for(Vertex v : a) {
    add(D3Index(v, index++));
  }
  if(size() < 2) return;
  sort().setTranslation();
}

D3ReductionArray &D3ReductionArray::setTranslation() {
  const UINT  n    = (UINT)size();
  D3Index    *last = &(*this)[0];
  last->m_translateTo = 0;
  m_translateTable.setCapacity(n).insert(0,(UINT)0, n);
  m_translateTable[last->m_origIndex] = last->m_translateTo;
  m_reducedArray.setCapacity(n);
  m_reducedArray.add(*last);
  for(UINT i = 1; i < n; i++) {
    D3Index &p = (*this)[i];
    p.m_translateTo = last->m_translateTo;
    if(dist(p, *last) > m_eps) {
      m_reducedArray.add(p);
      p.m_translateTo++;
      last = &p;
    }
    m_translateTable[p.m_origIndex] = p.m_translateTo;
  }
  return *this;
}
