#include "pch.h"
#include <D3DGraphics/D3SceneObjectVisual.h>
#include <D3DGraphics/D3PickedInfo.h>

D3PickedInfo::D3PickedInfo(const D3SceneObjectVisual *visualObject, int faceIndex, int vertexIndex[3], D3DXVECTOR3 facePoint[3], float U, float V)
  : m_visualObject(visualObject)
  , m_faceIndex(   faceIndex   )
  , m_U(U), m_V(V)
{
  for(int i = 0; i < 3; i++) {
    m_vertexIndex[i] = vertexIndex[i];
    m_facePoint[i]   = facePoint[i];
  }
}

void D3PickedInfo::clear() {
  memset(this, 0, sizeof(D3PickedInfo));
  m_faceIndex = -1;
}

D3DXVECTOR3 D3PickedInfo::getMeshPoint() const {
  return m_facePoint[0] + m_U * (m_facePoint[1] - m_facePoint[0]) + m_V * (m_facePoint[2] - m_facePoint[0]);
}

String D3PickedInfo::toString(int dec) const {
  return isEmpty()
       ? EMPTYSTRING
       : format(_T("Obj:%-20s:Face:%5d:[%5d,%5d,%5d], (U,V):(%s,%s), MP:%s")
               ,m_visualObject->getName().cstr()
               ,m_faceIndex
               ,m_vertexIndex[0], m_vertexIndex[1], m_vertexIndex[2]
               ,::toString(m_U, dec).cstr(), ::toString(m_V, dec).cstr()
               ,::toString(getMeshPoint(), dec).cstr()
               );
}
