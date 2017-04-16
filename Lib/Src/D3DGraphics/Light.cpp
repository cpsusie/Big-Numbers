#include "pch.h"
#include <D3DGraphics/D3ToString.h>

void LIGHT::setDefault(D3DLIGHTTYPE type) {
  switch(type) {
  case D3DLIGHT_DIRECTIONAL    : setDefaultDirectional(); break;
  case D3DLIGHT_POINT          : setDefaultPoint();       break;
  case D3DLIGHT_SPOT           : setDefaultSpot();        break;
  default                      : setDefaultDirectional(); break;
  }
}

void LIGHT::setDefaultDirectional() {
  ZeroMemory(this, sizeof(LIGHT));
  Type         = D3DLIGHT_DIRECTIONAL;
  m_enabled    = true;
  setDefaultColors();
  Direction    = unitVector(D3DXVECTOR3(0.175f, -0.385f, -0.88f));
}

void LIGHT::setDefaultPoint() {
  ZeroMemory(this, sizeof(LIGHT));
  Type         = D3DLIGHT_POINT;
  m_enabled    = true;
  setDefaultColors();
  Position     = D3DXVECTOR3(1, 1, 1);
  Range        = 100;
  Attenuation1 = 1;
}

void LIGHT::setDefaultSpot() {
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
}

void LIGHT ::setInnerAngle(float rad) {
  if((rad < 0) || (rad > D3DX_PI)) {
    return;
  }
  Theta = rad;
  if(Theta > Phi) {
    Phi = Theta;
  }
}

void LIGHT ::setOuterAngle(float rad) {
  if((rad < 0) || (rad > D3DX_PI)) {
    return;
  }
  Phi = rad;
  if(Phi < Theta) {
    Theta = Phi;
  }
}

void LIGHT::setDefaultColors() {
  Ambient  = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  Diffuse  = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
  Specular = D3DXCOLOR(   1.0f ,  1.0f ,  1.0f , 1.0f);
}

String LIGHT::toString(int dec) const {
  String indexStr = format(_T("Light[%d] %s:")
                          ,m_index
                          ,m_enabled ? _T("On ") : _T("Off")
                          );
  String colorStr = format(_T("\n  Light colors:Amb:%s Dif:%s Spec:%s")
                          ,::toString(Ambient).cstr()
                          ,::toString(Diffuse).cstr()
                          ,::toString(Specular).cstr()
                          );
  switch(Type) {
  case D3DLIGHT_DIRECTIONAL:
    return format(_T("%sDirectional. Dir:%s%s")
                  ,indexStr.cstr()
                  ,::toString(Direction, dec).cstr()
                  ,colorStr.cstr());
  case D3DLIGHT_POINT      :
    return format(_T("%sPoint. Pos:%s Range:%.*f%s")
                 ,indexStr.cstr()
                 ,::toString(Position, dec).cstr()
                 ,dec, Range
                 ,colorStr.cstr());
  case D3DLIGHT_SPOT       :
    return format(_T("%sSpot. Pos:%s Dir:%s Range:%.*f Inner:%.*f Outer:%.*f%s")
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
