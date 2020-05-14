#include "pch.h"
#include <D3DGraphics/D3ToString.h>

D3Light &D3Light::setDefault(D3DLIGHTTYPE type) {
  switch(type) {
  case D3DLIGHT_DIRECTIONAL : setDefaultDirectional(); break;
  case D3DLIGHT_POINT       : setDefaultPoint();       break;
  case D3DLIGHT_SPOT        : setDefaultSpot();        break;
  default                   : setUndefined();          break;
  }
  return *this;
}

D3Light &D3Light::setDefaultDirectional() {
  memset(this, 0, sizeof(D3Light));
  m_index      = -1;
  Type         = D3DLIGHT_DIRECTIONAL;
  m_enabled    = true;
  setDefaultColors();
  Direction    = unitVector(D3DXVECTOR3(0.175f, -0.385f, -0.88f));
  return *this;
}

D3Light &D3Light::setDefaultPoint() {
  memset(this, 0, sizeof(D3Light));
  m_index      = -1;
  Type         = D3DLIGHT_POINT;
  m_enabled    = true;
  setDefaultColors();
  Position     = D3DXVECTOR3(1, 1, 1);
  Range        = 100;
  Attenuation1 = 1;
  return *this;
}

D3Light &D3Light::setDefaultSpot() {
  memset(this, 0, sizeof(D3Light));
  m_index      = -1;
  Type         = D3DLIGHT_SPOT;
  m_enabled    = true;
  setDefaultColors();
  Position     = D3DXVECTOR3( 1, 1, 1);
  Direction    = -unitVector(Position);
  Range        = 100;
  Attenuation1 = 1;
  Falloff      = 1;
  Theta        = radians(10); // inner angle
  Phi          = radians(15); // outer angle
  return *this;
}

D3Light &D3Light::setInnerAngle(float rad) {
  if((rad >= 0) && (rad <= D3DX_PI)) {
    Theta = rad;
    if(Theta > Phi) {
      Phi = Theta;
    }
  }
  return *this;
}

D3Light &D3Light::setOuterAngle(float rad) {
  if((rad >= 0) && (rad <= D3DX_PI)) {
    Phi = rad;
    if(Phi < Theta) {
      Theta = Phi;
    }
  }
  return *this;
}

D3Light &D3Light::setDefaultColors() {
  Ambient  = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  Diffuse  = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  Specular = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  return *this;
}

String D3Light::getName() const {
  return isDefined() ? format(_T("Light[%d]"), m_index) : EMPTYSTRING;
}

String D3Light::toString(int dec) const {
  if(!isDefined()) {
    return _T("Undefined");
  }
  String indexStr = format(_T("%-3s")
                          ,m_enabled ? _T("On ") : _T("Off")
                          );
  String colorStr = format(_T("Colors:Amb:%s Dif:%s Spec:%s")
                          ,::toString(Ambient).cstr()
                          ,::toString(Diffuse).cstr()
                          ,::toString(Specular).cstr()
                          );
  switch(Type) {
  case D3DLIGHT_DIRECTIONAL:
    return format(_T("%s:Directional. Dir:%s, %s")
                  ,indexStr.cstr()
                  ,::toString(Direction, dec).cstr()
                  ,colorStr.cstr());
  case D3DLIGHT_POINT      :
    return format(_T("%s:Point. Pos:%s Range:%.*f, %s")
                 ,indexStr.cstr()
                 ,::toString(Position, dec).cstr()
                 ,dec, Range
                 ,colorStr.cstr());
  case D3DLIGHT_SPOT       :
    return format(_T("%s:Spot. Pos:%s Dir:%s Range:%.*f Inner:%.*f Outer:%.*f, %s")
                 ,indexStr.cstr()
                 ,::toString(Position , dec).cstr()
                 ,::toString(Direction, dec).cstr()
                 ,dec, Range
                 ,dec, RAD2GRAD(Theta)
                 ,dec, RAD2GRAD(Phi)
                 ,colorStr.cstr()
                 );
  }
  return EMPTYSTRING;
}
