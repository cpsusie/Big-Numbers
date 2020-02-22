#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>

bool VertexNormalTextureIndex::operator==(const VertexNormalTextureIndex &v) const {
  return (m_vIndex == v.m_vIndex)
      && (m_nIndex == v.m_nIndex)
      && (m_tIndex == v.m_tIndex);
}

ULONG VertexNormalTextureIndex::hashCode() const {
  return (m_vIndex * 100981 + m_nIndex) * 997 + m_tIndex;
}

String VertexNormalTextureIndex::toString() const {
  return format(_T("VertexNormalTextureIndex(v:%4d,n:%4d,t:%4d"), m_vIndex, m_nIndex, m_tIndex);
}
