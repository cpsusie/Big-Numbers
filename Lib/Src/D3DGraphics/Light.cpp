#include "pch.h"
#include <D3DGraphics/D3ToString.h>

LIGHT &LIGHT::setDefault(D3DLIGHTTYPE type) {
  switch(type) {
  case D3DLIGHT_DIRECTIONAL    : setDefaultDirectional(); break;
  case D3DLIGHT_POINT          : setDefaultPoint();       break;
  case D3DLIGHT_SPOT           : setDefaultSpot();        break;
  default                      : setUndefined();          break;
  }
  return *this;
}

LIGHT &LIGHT::setDefaultDirectional() {
  ZeroMemory(this, sizeof(LIGHT));
  Type         = D3DLIGHT_DIRECTIONAL;
  m_enabled    = true;
  setDefaultColors();
  Direction    = unitVector(D3DXVECTOR3(0.175f, -0.385f, -0.88f));
  return *this;
}

LIGHT &LIGHT::setDefaultPoint() {
  ZeroMemory(this, sizeof(LIGHT));
  Type         = D3DLIGHT_POINT;
  m_enabled    = true;
  setDefaultColors();
  Position     = D3DXVECTOR3(1, 1, 1);
  Range        = 100;
  Attenuation1 = 1;
  return *this;
}

LIGHT &LIGHT::setDefaultSpot() {
  ZeroMemory(this, sizeof(LIGHT));
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

LIGHT &LIGHT::setInnerAngle(float rad) {
  if((rad >= 0) && (rad <= D3DX_PI)) {
    Theta = rad;
    if(Theta > Phi) {
      Phi = Theta;
    }
  }
  return *this;
}

LIGHT &LIGHT::setOuterAngle(float rad) {
  if((rad >= 0) && (rad <= D3DX_PI)) {
    Phi = rad;
    if(Phi < Theta) {
      Theta = Phi;
    }
  }
  return *this;
}

LIGHT &LIGHT::setDefaultColors() {
  Ambient  = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  Diffuse  = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  Specular = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  return *this;
}

String LIGHT::toString(int dec) const {
  if(!isDefined()) {
    return _T("Light undefined");
  }
  String indexStr = format(_T("Light[%2d] %-3s")
                          ,m_index
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
