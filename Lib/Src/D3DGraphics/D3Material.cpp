#include "pch.h"
#include <D3DGraphics/D3ToString.h>

D3Material &D3Material::setDefault() {
  ZeroMemory(this, sizeof(D3Material));
  Ambient  = D3DXCOLOR(0.10f , 0.10f , 0.16f, 1.0f);
  Diffuse  = D3DXCOLOR(0.1f  , 0.086f, 0.29f, 1.0f);
  Specular = D3DXCOLOR(0.835f, 0.808f, 0.95f, 1.0f);
  Emissive = D3DXCOLOR(0     , 0     , 0    , 0   );
  Power    = 9.73f;
  return *this;
}

D3DMATERIAL D3Material::createMaterialWithColor(D3DCOLOR color) { // static
  const D3DCOLORVALUE cv = colorToColorValue(color);
  D3DMATERIAL mat;
  ZeroMemory(&mat, sizeof(mat));
  mat.Diffuse  = cv;
  mat.Emissive = cv;
  return mat;
}

D3Material &D3Material::setOpacity(float v) {
  Diffuse.a = minMax(v, 0.0f, 1.0f);
  return *this;
}

float D3Material::getOpacity() const {
  return Diffuse.a;
}

String D3Material::toString(int dec) const {
  if(!isDefined()) {
    return _T("Material undefined");
  }
  return format(_T("Material[%2d]:Power:%.*f\n  Colors:Amb:%s Dif:%s Spec:%s Emi:%s")
               ,m_id
               ,dec, Power
               ,::toString(Ambient).cstr()
               ,::toString(Diffuse , true).cstr()
               ,::toString(Specular, true).cstr()
               ,::toString(Emissive).cstr()
               );
}
