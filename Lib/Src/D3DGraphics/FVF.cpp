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
