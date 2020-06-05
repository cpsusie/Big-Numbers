#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include "D3NameLookupTemplate.h"

#if defined(addFlag)
#undef  addFlag
#endif
#if defined(NV)
#undef NV
#endif
#define addFlag(flag) if(usage & D3DUSAGE_##flag) { result += delim + _T(#flag); delim = _T("|"); }
#define NV(v)         D3DUSAGE_##v, _T(#v)

String usageToString(DWORD usage) {
  if(usage == 0) {
    return _T("0");
  }
  String result, delim;
  addFlag(RENDERTARGET                    )
  addFlag(DEPTHSTENCIL                    )
  addFlag(DYNAMIC                         )
#if !defined(D3D_DISABLE_9EX)
  addFlag(NONSECURE                       )
#endif
// When passed to CheckDeviceFormat, D3DUSAGE_AUTOGENMIPMAP may return
// D3DOK_NOAUTOGEN if the device doesn't support autogeneration for that format.
// D3DOK_NOAUTOGEN is a success code, not a failure code... the SUCCEEDED and FAILED macros
// will return true and false respectively for this code.
  addFlag(AUTOGENMIPMAP                   )
  addFlag(DMAP                            )
  addFlag(QUERY_LEGACYBUMPMAP             )
  addFlag(QUERY_SRGBREAD                  )
  addFlag(QUERY_FILTER                    )
  addFlag(QUERY_SRGBWRITE                 )
  addFlag(QUERY_POSTPIXELSHADER_BLENDING  )
  addFlag(QUERY_VERTEXTEXTURE             )
  addFlag(QUERY_WRAPANDMIP                )
/* Usages for Vertex/Index buffers */
  addFlag(WRITEONLY                       )
  addFlag(SOFTWAREPROCESSING              )
  addFlag(DONOTCLIP                       )
  addFlag(POINTS                          )
  addFlag(RTPATCHES                       )
  addFlag(NPATCHES                        )
#if !defined(D3D_DISABLE_9EX)
  addFlag(TEXTAPI                         )
  addFlag(RESTRICTED_CONTENT              )
  addFlag(RESTRICT_SHARED_RESOURCE        )
  addFlag(RESTRICT_SHARED_RESOURCE_DRIVER )
#endif
  return result;
}

static const NameValue<DWORD> usageNames[] = {
  NV(RENDERTARGET                    )
 ,NV(DEPTHSTENCIL                    )
 ,NV(DYNAMIC                         )
#if !defined(D3D_DISABLE_9EX)
 ,NV(NONSECURE                       )
#endif
// When passed to CheckDeviceFormat, D3DUSAGE_AUTOGENMIPMAP may return
// D3DOK_NOAUTOGEN if the device doesn't support autogeneration for that format.
// D3DOK_NOAUTOGEN is a success code, not a failure code... the SUCCEEDED and FAILED macros
// will return true and false respectively for this code.
 ,NV(AUTOGENMIPMAP                   )
 ,NV(DMAP                            )
 ,NV(QUERY_LEGACYBUMPMAP             )
 ,NV(QUERY_SRGBREAD                  )
 ,NV(QUERY_FILTER                    )
 ,NV(QUERY_SRGBWRITE                 )
 ,NV(QUERY_POSTPIXELSHADER_BLENDING  )
 ,NV(QUERY_VERTEXTEXTURE             )
 ,NV(QUERY_WRAPANDMIP                )
/* Usages for Vertex/Index buffers */
 ,NV(WRITEONLY                       )
 ,NV(SOFTWAREPROCESSING              )
 ,NV(DONOTCLIP                       )
 ,NV(POINTS                          )
 ,NV(RTPATCHES                       )
 ,NV(NPATCHES                        )
#if !defined(D3D_DISABLE_9EX)
 ,NV(TEXTAPI                         )
 ,NV(RESTRICTED_CONTENT              )
 ,NV(RESTRICT_SHARED_RESOURCE        )
 ,NV(RESTRICT_SHARED_RESOURCE_DRIVER )
#endif
};

DefineNameLookupClass(DWORD, usageNames,Usage);

DWORD usageFromString(const String &str) {
  const UsageLookupTable &map = UsageLookupTable::getInstance();
  DWORD                result      = 0;
  const String         delimiter   = _T("|");
  for(Tokenizer tok(str, delimiter); tok.hasNext();) {
    result |= map.lookupName(tok.next());
  }
  return result;
}
