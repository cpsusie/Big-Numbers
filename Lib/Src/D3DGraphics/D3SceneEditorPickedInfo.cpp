#include "pch.h"
#include <D3DGraphics/D3SceneEditorPickedInfo.h>

void D3SceneEditorPickedInfo::clear() {
  m_info.clear();
  m_hitPoint = D3DXORIGIN;
  m_dist     = 0;
}

String D3SceneEditorPickedInfo::toString() const {
  return m_info.isEmpty()
       ? _T("/")
       : _T("WP:") + ::toString(m_hitPoint)
                   + format(_T(", Dist:%.3f, Info:%s")
                           ,m_dist
                           ,m_info.toString().cstr()
                           );
}
