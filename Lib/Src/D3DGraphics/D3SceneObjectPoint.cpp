#include "pch.h"
#include <D3DGraphics/D3SceneObject.h>
#include <D3DGraphics/D3SceneObjectPoint.h>

const D3DXVECTOR3 D3SceneObjectPoint::getWorldPoint() const {
  return isEmpty() ? D3DXORIGIN : m_obj->getWorld() * m_meshPoint;
}

String D3SceneObjectPoint::toString() const {
  return isEmpty()
       ? _T("Empty")
       : format(_T("MeshPoint:%s, World:%s, Obj:%s"), ::toString(m_meshPoint).cstr(), ::toString(getWorldPoint()).cstr(), m_obj->getName().cstr());
}
