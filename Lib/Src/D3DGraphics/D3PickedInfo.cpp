#include "pch.h"
#include <D3DGraphics/D3PickedInfo.h>

String D3PickedInfo::toString(int dec) const {
  return isEmpty()
       ? EMPTYSTRING
       : format(_T("Face:%5d:[%5d,%5d,%5d], (U,V):(%s,%s), MP:%s")
               ,m_faceIndex
               ,m_vertexIndex[0], m_vertexIndex[1], m_vertexIndex[2]
               ,::toString(m_U, dec).cstr(), ::toString(m_V, dec).cstr()
               ,::toString(getMeshPoint(), dec).cstr()
               );
}
