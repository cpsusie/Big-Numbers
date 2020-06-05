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
String          lightTypeToString(     D3DLIGHTTYPE          type     );
D3DLIGHTTYPE    lightTypeFromString(   const String         &str      );
String          fillModeToString(      D3DFILLMODE           mode     );
D3DFILLMODE     fillModeFromString(    const String         &str      );
String          shadeModeToString(     D3DSHADEMODE          mode     );
D3DSHADEMODE    shadeModeFromString(   const String         &str      );
String          formatToString(        D3DFORMAT             format   );
D3DFORMAT       formatFromString(      const String         &str      );
String          poolToString(          D3DPOOL               pool     );
D3DPOOL         poolFromString(        const String         &str      );
String          resourceTypeToString(  D3DRESOURCETYPE       type     );
D3DRESOURCETYPE resourceTypeFromString(const String         &str      );
String          usageToString(         DWORD                 usage    );
DWORD           usageFromString(       const String         &str      );
String          FVFToString(           DWORD                 fvf      );
DWORD           FVFFromString(         const String         &str      );
String          toString(              const D3DDISPLAYMODE &mode     );
String          handednessToString(    bool                  rightHanded);
// return true if str == "righthanded", false if str == lefthanded, else throw
bool            handednessFromString(  const String          &str     );

String toString(const D3DVERTEXBUFFER_DESC &desc     ,bool multiLines=true);
String toString(const D3DINDEXBUFFER_DESC  &desc     ,bool multiLines=true);

#define FORMAT_BUFFERDESC   0x01
#define FORMAT_BUFFERDESCNL 0x02
#define FORMAT_BUFFERDATA   0x04
#define FORMAT_VERTEXBUFFER 0x08
#define FORMAT_INDEXBUFFER  0x10
#define FORMAT_BUFFERALL (FORMAT_BUFFERDESC | FORMAT_BUFFERDATA)

// if FORMAT_BUFFERDATA bit in flags is set, it implies FORMAT_BUFFERDESCNL too
String toString(LPD3DXMESH             mesh        , BYTE flags = FORMAT_BUFFERALL | FORMAT_VERTEXBUFFER | FORMAT_INDEXBUFFER);
String toString(LPDIRECT3DVERTEXBUFFER vertexBuffer, BYTE flags = FORMAT_BUFFERALL);
String toString(LPDIRECT3DINDEXBUFFER  indexBuffer , BYTE flags = FORMAT_BUFFERALL);

String vertexToString(const char *v, DWORD FVF  , int dec=3);
