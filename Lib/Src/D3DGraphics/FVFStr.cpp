#include "pch.h"
#include "D3NameLookupTemplate.h"

#define NV(f) D3DFVF_##f, _T(#f)

static const NameValue<DWORD> fvfNames[] = {
  NV(XYZ     )
 ,NV(XYZRHW  )
 ,NV(XYZB1   )
 ,NV(XYZB2   )
 ,NV(XYZB3   )
 ,NV(XYZB4   )
 ,NV(XYZB5   )
 ,NV(XYZW    )
 ,NV(NORMAL  )
 ,NV(PSIZE   )
 ,NV(DIFFUSE )
 ,NV(SPECULAR)
 ,NV(TEX1    )
 ,NV(TEX2    )
 ,NV(TEX3    )
 ,NV(TEX4    )
 ,NV(TEX5    )
 ,NV(TEX6    )
 ,NV(TEX7    )
 ,NV(TEX8    )
};

DefineNameLookupClass(DWORD, fvfNames,FVF);

DWORD FVFFromString(const String &str) {
  const FVFLookupTable &map = FVFLookupTable::getInstance();
  DWORD                result      = 0;
  const String         delimiter   = _T("|");
  for(Tokenizer tok(str, delimiter); tok.hasNext();) {
    result |= map.lookupName(tok.next());
  }
  return result;
}

String FVFToString(DWORD fvf) {
  String result, delim;

#undef  caseAddStr
#define caseAddStr(s) case D3DFVF_##s: result += delim + _T(#s); delim=_T("|"); break;

  switch(fvf & D3DFVF_POSITION_MASK) {
  caseAddStr(XYZ              )
  caseAddStr(XYZRHW           )
  caseAddStr(XYZB1            )
  caseAddStr(XYZB2            )
  caseAddStr(XYZB3            )
  caseAddStr(XYZB4            )
  caseAddStr(XYZB5            )
  caseAddStr(XYZW             )
  }
  
#undef  addFlag
#define addFlag(flag) if(fvf & D3DFVF_##flag) { result += delim + _T(#flag); delim=_T("|"); }

  addFlag(NORMAL              )
  addFlag(PSIZE               )
  addFlag(DIFFUSE             )
  addFlag(SPECULAR            )

  switch(fvf & D3DFVF_TEXCOUNT_MASK) {
  caseAddStr(TEX1             )
  caseAddStr(TEX2             )
  caseAddStr(TEX3             )
  caseAddStr(TEX4             )
  caseAddStr(TEX5             )
  caseAddStr(TEX6             )
  caseAddStr(TEX7             )
  caseAddStr(TEX8             )
  }

  addFlag(LASTBETA_UBYTE4  )
  addFlag(LASTBETA_D3DCOLOR)

  return result;
}
