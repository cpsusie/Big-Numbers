#include "pch.h"

String FVFToString(DWORD fvf) {
  String result, delim;

#undef  caseAddStr
#define caseAddStr(s) case s: result += delim + _T(#s); delim=_T("|"); break;

  switch(fvf & D3DFVF_POSITION_MASK) {
  caseAddStr(D3DFVF_XYZ              )
  caseAddStr(D3DFVF_XYZRHW           )
  caseAddStr(D3DFVF_XYZB1            )
  caseAddStr(D3DFVF_XYZB2            )
  caseAddStr(D3DFVF_XYZB3            )
  caseAddStr(D3DFVF_XYZB4            )
  caseAddStr(D3DFVF_XYZB5            )
  caseAddStr(D3DFVF_XYZW             )
  }

#undef  addFlag
#define addFlag(flag) if(fvf & flag) { result += delim + _T(#flag); delim=_T("|"); }

  addFlag(D3DFVF_NORMAL              );
  addFlag(D3DFVF_PSIZE               );
  addFlag(D3DFVF_DIFFUSE             );
  addFlag(D3DFVF_SPECULAR            );

  switch(fvf & D3DFVF_TEXCOUNT_MASK) {
  caseAddStr(D3DFVF_TEX1             );
  caseAddStr(D3DFVF_TEX2             );
  caseAddStr(D3DFVF_TEX3             );
  caseAddStr(D3DFVF_TEX4             );
  caseAddStr(D3DFVF_TEX5             );
  caseAddStr(D3DFVF_TEX6             );
  caseAddStr(D3DFVF_TEX7             );
  caseAddStr(D3DFVF_TEX8             );
  }

  addFlag(D3DFVF_LASTBETA_UBYTE4  );
  addFlag(D3DFVF_LASTBETA_D3DCOLOR);

  return result;
}

static const int textureCoordFlotCount[] = {
  2  // D3DFVF_TEXTUREFORMAT2 0 Two floating point values
 ,3  // D3DFVF_TEXTUREFORMAT3 1 Three floating point values
 ,4  // D3DFVF_TEXTUREFORMAT4 2 Four floating point values
 ,1  // D3DFVF_TEXTUREFORMAT1 3 One floating point value
};

#define MASK_TEXTUREFORMAT(fvf, CoordIndex) (((fvf) >> ((CoordIndex)*2 + 16)) & 0x3)

int FVFToSize(DWORD fvf) {
  int bytes = 0;

#undef  caseAddBytes
#define caseAddBytes(s, type) case s: bytes += sizeof(type); break;

  switch(fvf & D3DFVF_POSITION_MASK) {
  caseAddBytes(D3DFVF_XYZ              ,D3DXVECTOR3    )
  caseAddBytes(D3DFVF_XYZRHW           ,D3DXVECTOR4    )
/*
  caseAddBytes(D3DFVF_XYZB1            )
  caseAddBytes(D3DFVF_XYZB2            )
  caseAddBytes(D3DFVF_XYZB3            )
  caseAddBytes(D3DFVF_XYZB4            )
  caseAddBytes(D3DFVF_XYZB5            )
*/
  caseAddBytes(D3DFVF_XYZW             ,D3DXVECTOR4)
  }

#undef  addFlagBytes
#define addFlagBytes(flag, type) if(fvf & flag) { bytes += sizeof(type); }

  addFlagBytes(D3DFVF_NORMAL           ,D3DXVECTOR3    );
  addFlagBytes(D3DFVF_PSIZE            ,float          );
  addFlagBytes(D3DFVF_DIFFUSE          ,DWORD          );
  addFlagBytes(D3DFVF_SPECULAR         ,DWORD          );

  switch(fvf & D3DFVF_TEXCOUNT_MASK) {
  case D3DFVF_TEX1: bytes += 2*sizeof(float); break;
  case D3DFVF_TEX2:
  case D3DFVF_TEX3:
  case D3DFVF_TEX4:
  case D3DFVF_TEX5:
  case D3DFVF_TEX6:
  case D3DFVF_TEX7:
  case D3DFVF_TEX8:
    throwException(_T("%s:TEX2-TEX8 not implemented yet"), __TFUNCTION__);
    break;
  }
/*
  addFlagBytes(D3DFVF_LASTBETA_UBYTE4  );
  addFlagBytes(D3DFVF_LASTBETA_D3DCOLOR);
*/
  return bytes;
}
