#pragma once

#include <MyUtil.h>
#include <MFCUtil/DirectXDeviceFactory.h>
#include <MFCUtil/ColorSpace.h>
#include "D3Light.h"
#include "D3Material.h"

inline String toString(const D3Light                &light, int dec=3) {
  return light.toString(dec);
}
inline String toString(const D3Material             &material, int dec = 3) {
  return material.toString(dec);
}
String toString(      D3DLIGHTTYPE          type     );
String toString(      D3DFILLMODE           mode     );
String toString(      D3DSHADEMODE          mode     );
String toString(      D3DFORMAT             f        );
String toString(      D3DPOOL               pool     );
String toString(      D3DRESOURCETYPE       type     );
String usageToString( DWORD                 usage    );
String FVFToString(   DWORD                 fvf      );
String toString(const D3DDISPLAYMODE       &mode     );

inline String toString(D3PCOLOR             c        ,bool showAlpha=false) {
  return c.toString(showAlpha);
}
String toString(const D3DCOLORVALUE        &c        ,bool showAlpha=false);
String toString(const D3DVERTEXBUFFER_DESC &desc     );
String toString(const D3DINDEXBUFFER_DESC  &desc     );

String toString(LPD3DXMESH             mesh        );
String toString(LPDIRECT3DVERTEXBUFFER vertexBuffer);
String toString(LPDIRECT3DINDEXBUFFER  indexBuffer );

String handednessToString(bool rightHanded);
String vertexToString(const char *v, DWORD FVF  , int dec=3);
