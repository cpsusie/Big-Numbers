#include "pch.h"
#include <D3DGraphics/D3World.h>

D3World &D3World::resetPos() {
  return setPos(D3DXORIGIN);
}

D3DXVECTOR3 D3World::getDir() const {
  return ::rotate(E[0], m_q);
}

D3DXVECTOR3 D3World::getUp() const {
  return ::rotate(E[2], m_q);
}

D3DXVECTOR3 D3World::getRight() const {
  return cross(getDir(), getUp());
}

D3DXVECTOR3 D3World::getLeft() const {
  return cross(getUp(), getDir());
}

static D3DXMATRIX &createMatrixLookAt(D3DXMATRIX &view, const D3DXVECTOR3 &eye, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up, bool rightHanded) {
  return rightHanded
       ? *D3DXMatrixLookAtRH(&view, &eye, &lookAt, &up)
       : *D3DXMatrixLookAtLH(&view, &eye, &lookAt, &up);
}

D3DXMATRIX &D3World::createViewMatrix(D3DXMATRIX &dst, bool rightHanded) const {
  return createMatrixLookAt(dst, m_pos, m_pos + getDir(), getUp(), rightHanded);
}

D3World &D3World::setLookAt(const D3DXVECTOR3 &lookAt) {
  D3DXVECTOR3 newDir = lookAt - m_pos;
  const float dirlen = length(newDir);
  if(dirlen != 0) {
    newDir /= dirlen;
    m_q *= createRotation(getDir(), newDir);
  }
  return *this;
}

D3World &D3World::setLookAt(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up) {
  return setPos(pos).setOrientation(createOrientation(lookAt - pos, up));
}

D3World &D3World::setOrientation(const D3DXQUATERNION &q, const D3DXVECTOR3 &centerOfRotation) {
  const D3DXMATRIX  w     = *this;
  const D3DXVECTOR3 c     = inverse(w) * centerOfRotation;
  const D3DXVECTOR3 newcr = setOrientation(q) * c;
  return setPos(getPos() + centerOfRotation - newcr);
}

D3World &D3World::rotate(const D3DXQUATERNION &rot) {
  return setOrientation(getOrientation()*rot);
}

D3World &D3World::rotate(const D3DXQUATERNION &rot, const D3DXVECTOR3 &centerOfRotation) {
  return setOrientation(getOrientation()*rot, centerOfRotation);
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
