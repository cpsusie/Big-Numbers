#include "pch.h"
#include <D3DGraphics/D3World.h>

D3DXMATRIX &D3World::createViewMatrix(D3DXMATRIX &dst, bool rightHanded) const {
  return D3DXMatrixLookAt(dst, m_pos, m_pos + getDir(), getUp(), rightHanded);
}

D3World &D3World::setOrientation(const D3DXQUATERNION &q, const D3DXVECTOR3 &centerOfRotation) {
  const D3DXMATRIX  w     = *this;
  const D3DXVECTOR3 c     = inverse(w) * centerOfRotation;
  const D3DXVECTOR3 newcr = setOrientation(q) * c;
  return setPos(getPos() + centerOfRotation - newcr);
}

bool operator==(const D3World &w1, const D3World &w2) {
  return w1.getPos()         == w2.getPos()
      && w1.getOrientation() == w2.getOrientation()
      && w1.getScale()       == w2.getScale();
}

String D3World::toString(int dec) const {
  return format(_T("Pos:%s, Scale:%s\nOrientation:%s (D:%s, U:%s, R:%s)")
                ,::toString(m_pos     , dec).cstr()
                ,::toString(m_scale   , dec).cstr()
                ,::toString(m_q       , dec).cstr()
                ,::toString(getDir()  , dec).cstr()
                ,::toString(getUp()   , dec).cstr()
                ,::toString(getRight(), dec).cstr()
                );
}
