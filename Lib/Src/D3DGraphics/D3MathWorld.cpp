#include "pch.h"
#include <D3DGraphics/D3MathWorld.h>

D3DXMATRIX &D3World::createViewMatrix(D3DXMATRIX &dst, bool rightHanded) const {
  return D3DXMatrixLookAt(dst, m_pos, m_pos + getDir(), getUp(), rightHanded);
}

String D3World::toString(int dec) const {
  return format(_T("Pos:%s, Orientation:%s, Scale:%s, (D:%s, U:%s, R:%s)")
                ,::toString(m_pos     , dec).cstr()
                ,::toString(m_q       , dec).cstr()
                ,::toString(m_scale   , dec).cstr()
                ,::toString(getDir()  , dec).cstr()
                ,::toString(getUp()   , dec).cstr()
                ,::toString(getRight(), dec).cstr()
                );
}
