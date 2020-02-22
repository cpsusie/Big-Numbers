#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>

void Face::invertOrientation() {
  UINT n = getIndexCount() - 1;
  for (UINT i = 0; i < n;) {
    m_data.swap(i++, n--);
  }
}

String Face::toString() const {
  return format(_T("Face(Color:%#8x, IndexCount:%zu\nIndexArray:%s\n")
    , m_diffuseColor, m_data.size(), m_data.toString().cstr());
}
