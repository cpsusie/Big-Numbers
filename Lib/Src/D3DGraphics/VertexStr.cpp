#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

String vertexToString(const char *v, DWORD fvf, int dec) {
  const int    width = dec + 3;
  String       result;
  const TCHAR *delimiter = EMPTYSTRING;

#define addTagName(        tag ) { result += delimiter; result += tag; result += _T(":"); }
#define addTypeStr(        type) { result += toString(*((type*)v));             v += sizeof(type); delimiter = _T(", "); }
#define addTypeStrD(       type) { result += toString(*((type*)v), dec);        v += sizeof(type); delimiter = _T(", "); }
#define addTypeStrDW(      type) { result += toString(*((type*)v), dec, width); v += sizeof(type); delimiter = _T(", "); }
#define caseAddElement( s, type) case D3DFVF##_##s:     { addTagName(_T(#s)); addTypeStr( type); } break
#define caseAddElementD(s, type) case D3DFVF##_##s:     { addTagName(_T(#s)); addTypeStrD(type); } break
#define addFlagElement( s, type) if(fvf & D3DFVF##_##s) { addTagName(_T(#s)); addTypeStr( type); }
#define addFlagElementD(s, type) if(fvf & D3DFVF##_##s) { addTagName(_T(#s)); addTypeStrD(type); }
#define addTextureStr(    index) addTagName(format(_T("TEX%d"), index)); \
                                 addTypeStrD(TextureVertex);             \
                                 break

#define caseTexture(coordIndex) case D3DFVF_TEX##coordIndex: addTextureStr(coordIndex)
  switch(fvf & D3DFVF_POSITION_MASK) {
  caseAddElementD(XYZ              ,D3DXVECTOR3    );
  caseAddElementD(XYZRHW           ,D3DXVECTOR4    );
/*
  caseAddBytes(XYZB1            )
  caseAddBytes(XYZB2            )
  caseAddBytes(XYZB3            )
  caseAddBytes(XYZB4            )
  caseAddBytes(XYZB5            )
*/
  caseAddElementD(XYZW             ,D3DXVECTOR4    );
  }
  addFlagElementD(NORMAL           ,D3DXVECTOR3    );

  addFlagElementD(PSIZE            ,float          );

  addFlagElement( DIFFUSE          ,D3PCOLOR       );
  addFlagElement( SPECULAR         ,D3PCOLOR       );

  switch(fvf & D3DFVF_TEXCOUNT_MASK) {
  caseTexture(1);
  caseTexture(2);
  caseTexture(3);
  caseTexture(4);
  caseTexture(5);
  caseTexture(6);
  caseTexture(7);
  caseTexture(8);
  }
/*
  addFlagBytes(LASTBETA_UBYTE4  );
  addFlagBytes(LASTBETA_D3DCOLOR);
*/
  return result;
}
