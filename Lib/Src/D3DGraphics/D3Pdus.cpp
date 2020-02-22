#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3Pdus.h>

D3PosDirUpScale::D3PosDirUpScale(bool rightHanded)
: m_rightHanded(rightHanded)
, m_pos(getDefaultPos())
, m_scale(getDefaultScale())
, m_dir(getDefaultDir())
, m_up(getDefaultUp())
{
  updateView();
}


D3PosDirUpScale &D3PosDirUpScale::setRightHanded(bool rightHanded) {
  if(rightHanded != m_rightHanded) {
    m_rightHanded = rightHanded;
    updateView();
  }
  return *this;
}

D3PosDirUpScale &D3PosDirUpScale::setOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  if(length(cross(dir, up)) != 0) {
    m_dir = unitVector(dir);
    m_up  = unitVector(up);
    updateView();
  } else {
    const D3DXVECTOR3 oldDir = getDir(), oldUp = getUp();
    const D3DXVECTOR3 r      = getRight();
    D3DXVECTOR3       newUp  = cross(r, dir);
    if(length(newUp) != 0) {
      m_dir = unitVector(dir);
      m_up  = unitVector(newUp);
      updateView();
    }
  }
  return *this;
}

D3PosDirUpScale &D3PosDirUpScale::setWorldMatrix(const D3DXMATRIX &world) {
  m_pos = D3DXVECTOR3(world._41, world._42, world._43);
  m_dir = D3DXVECTOR3(-world._31, -world._32, -world._33);
  m_up = D3DXVECTOR3(world._21, world._22, world._23);
  const float sx = length(D3DXVECTOR3(world._11, world._12, world._13));
  const float sy = length(D3DXVECTOR3(world._21, world._22, world._23));
  const float sz = length(D3DXVECTOR3(world._31, world._32, world._33));
  m_scale = D3DXVECTOR3(sx, sy, sz);
  m_dir /= sz;
  m_up  /= sy;
  return updateView();
}

D3PosDirUpScale &D3PosDirUpScale::updateView() {
  D3DXVECTOR3 lookAt = m_pos + m_dir;
  D3DXMatrixLookAt(m_view, m_pos, lookAt, m_up, m_rightHanded);
  return *this;
}

bool D3PosDirUpScale::operator==(const D3PosDirUpScale &pdus) const {
  return (m_pos == pdus.m_pos)
    && (m_dir == pdus.m_dir)
    && (m_up == pdus.m_up)
    && (m_scale == pdus.m_scale)
    && (getRightHanded() == pdus.getRightHanded());
}

String D3PosDirUpScale::toString(int dec) const {
  const StringArray va(Tokenizer(::toString(getViewMatrix(), dec), _T("\n")));
  const StringArray wa(Tokenizer(::toString(getWorldMatrix(), dec), _T("\n")));
  const int         ml = (int)va.maxLength();
  StringArray a;
  for (size_t i = 0; i < va.size(); i++) {
    a.add(format(_T("%-5s %-*.*s %-6s %-*.*s")
                ,(i == 0) ? _T("View:") : EMPTYSTRING
                ,ml, ml, va[i].cstr()
                ,(i == 0) ? _T("World:") : EMPTYSTRING
                ,ml, ml, wa[i].cstr()
                ));
  }
  return format(_T("P:%s D:%s U:%s R:%s S:%s %s\n%s")
               ,::toString(m_pos     , dec).cstr()
               ,::toString(m_dir     , dec).cstr()
               ,::toString(m_up      , dec).cstr()
               ,::toString(getRight(), dec).cstr()
               ,::toString(m_scale   , dec).cstr()
               ,handednessToString(getRightHanded()).cstr()
               ,a.toString(_T("\n")).remove(0, 1).removeLast().cstr()
               );
}
