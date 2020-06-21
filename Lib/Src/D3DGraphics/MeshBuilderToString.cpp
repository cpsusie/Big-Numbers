#include "pch.h"
#include <D3DGraphics/MeshBuilder.h>

String MeshBuilder::toString() const {
  String result = _T("MeshBuilder:\n");

  result += format(_T("  Vertices:%zd\n"), m_vertices.size());
  String tmp;
  for(size_t i = 0; i < m_vertices.size(); i++) {
    tmp += format(_T("%5zd %s\n"), i, ::toString(m_vertices[i], 5).cstr());
  }
  result += indentString(tmp, 4);

  result += format(_T("  Normals:%zd\n"), m_normals.size());
  tmp     = EMPTYSTRING;
  for(size_t i = 0; i < m_normals.size(); i++) {
    tmp += format(_T("%5zd %s\n"), i, ::toString(m_normals[i], 5).cstr());
  }
  result += indentString(tmp, 4);

  result += format(_T("  TextureVertices:%zd\n"), m_textureVertexArray.size());
  tmp     = EMPTYSTRING;
  for(size_t i = 0; i < m_textureVertexArray.size(); i++) {
    tmp += format(_T("%5zd %s\n"), i, ::toString(m_textureVertexArray[i],5).cstr());
  }
  result += indentString(tmp, 4);

  result += format(_T("  Faces:%zd\n"), m_faceArray.size());
  tmp     = EMPTYSTRING;
  for(size_t i = 0; i < m_faceArray.size(); i++) {
    const Face      &face = m_faceArray[i];
    const VNTIArray &vna  = face.getIndexArray();
    tmp += format(_T("%5zd %zd\n"), i, vna.size());
    for(size_t v = 0; v < vna.size(); v++) {
      tmp += format(_T("  %5d %5d %5d\n"), vna[v].m_vIndex, vna[v].m_nIndex, vna[v].m_tIndex);
    }
  }
  result += indentString(tmp, 4);
  return result;
}
