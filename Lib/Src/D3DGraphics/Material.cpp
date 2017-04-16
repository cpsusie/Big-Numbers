#include "pch.h"
#include <D3DGraphics/D3ToString.h>

void MATERIAL::setDefault() {
  ZeroMemory(this, sizeof(D3DMATERIAL));
  Ambient  = D3DXCOLOR(0.10f , 0.10f , 0.16f, 1.0f);
  Diffuse  = D3DXCOLOR(0.1f  , 0.086f, 0.29f, 1.0f);
  Specular = D3DXCOLOR(0.835f, 0.808f, 0.95f, 1.0f);
  Emissive = D3DXCOLOR(0     , 0     , 0    , 0   );
  Power    = 9.73f;
}

String MATERIAL::toString(int dec) const {
  return format(_T("Material[%d]:Power:%.*f\n  Colors:Amb:%s Dif:%s Spec:%s Emi:%s")
               ,m_index
               ,dec, Power
               ,::toString(Ambient).cstr()
               ,::toString(Diffuse , true).cstr()
               ,::toString(Specular, true).cstr()
               ,::toString(Emissive).cstr()
               );
}
