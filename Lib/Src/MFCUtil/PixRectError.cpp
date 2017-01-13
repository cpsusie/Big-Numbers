#include "stdafx.h"
#include <MFCUtil/PixRect.h>

String get3DErrorMsg(HRESULT hr) {

#define casepr(v) case v: return #v

  switch(hr) {
    casepr(D3DERR_WRONGTEXTUREFORMAT               );
    casepr(D3DERR_UNSUPPORTEDCOLOROPERATION        );
    casepr(D3DERR_UNSUPPORTEDCOLORARG              );
    casepr(D3DERR_UNSUPPORTEDALPHAOPERATION        );
    casepr(D3DERR_UNSUPPORTEDALPHAARG              );
    casepr(D3DERR_TOOMANYOPERATIONS                );
    casepr(D3DERR_CONFLICTINGTEXTUREFILTER         );
    casepr(D3DERR_UNSUPPORTEDFACTORVALUE           );
    casepr(D3DERR_CONFLICTINGRENDERSTATE           );
    casepr(D3DERR_UNSUPPORTEDTEXTUREFILTER         );
    casepr(D3DERR_CONFLICTINGTEXTUREPALETTE        );
    casepr(D3DERR_DRIVERINTERNALERROR              );
    casepr(D3DERR_NOTFOUND                         );
    casepr(D3DERR_MOREDATA                         );
    casepr(D3DERR_DEVICELOST                       );
    casepr(D3DERR_DEVICENOTRESET                   );
    casepr(D3DERR_NOTAVAILABLE                     );
    casepr(D3DERR_OUTOFVIDEOMEMORY                 );
    casepr(D3DERR_INVALIDDEVICE                    );
    casepr(D3DERR_INVALIDCALL                      );
    casepr(D3DERR_DRIVERINVALIDCALL                );
    casepr(D3DERR_WASSTILLDRAWING                  );
    casepr(D3DERR_DEVICEREMOVED                    );
    casepr(D3DOK_NOAUTOGEN                         );
    casepr(S_NOT_RESIDENT                          );
    casepr(S_RESIDENT_IN_SHARED_MEMORY             );
    casepr(S_PRESENT_MODE_CHANGED                  );
    casepr(S_PRESENT_OCCLUDED                      );
    casepr(D3DERR_DEVICEHUNG                       );
  default: return format(_T("Unknown D3D-error:%#08X"), hr);
  }
}

const TCHAR *PixRect::getFormatName(D3DFORMAT format) { // static
#undef casepr
#define casepr(f) case D3DFMT_##f: return _T(#f)
  switch(format) {
  casepr(UNKNOWN              );
  casepr(R8G8B8               );
  casepr(A8R8G8B8             );
  casepr(X8R8G8B8             );
  casepr(R5G6B5               );
  casepr(X1R5G5B5             );
  casepr(A1R5G5B5             );
  casepr(A4R4G4B4             );
  casepr(R3G3B2               );
  casepr(A8                   );
  casepr(A8R3G3B2             );
  casepr(X4R4G4B4             );
  casepr(A2B10G10R10          );
  casepr(A8B8G8R8             );
  casepr(X8B8G8R8             );
  casepr(G16R16               );
  casepr(A2R10G10B10          );
  casepr(A16B16G16R16         );
  casepr(A8P8                 );
  casepr(P8                   );
  casepr(L8                   );
  casepr(A8L8                 );
  casepr(A4L4                 );
  casepr(V8U8                 );
  casepr(L6V5U5               );
  casepr(X8L8V8U8             );
  casepr(Q8W8V8U8             );
  casepr(V16U16               );
  casepr(A2W10V10U10          );
  casepr(UYVY                 );
  casepr(R8G8_B8G8            );
  casepr(YUY2                 );
  casepr(G8R8_G8B8            );
  casepr(DXT1                 );
  casepr(DXT2                 );
  casepr(DXT3                 );
  casepr(DXT4                 );
  casepr(DXT5                 );
  casepr(D16_LOCKABLE         );
  casepr(D32                  );
  casepr(D15S1                );
  casepr(D24S8                );
  casepr(D24X8                );
  casepr(D24X4S4              );
  casepr(D16                  );
  casepr(D32F_LOCKABLE        );
  casepr(D24FS8               );

/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)
  /* Z-Stencil formats valid for CPU access */
  casepr(D32_LOCKABLE         );
  casepr(S8_LOCKABLE          );
#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */

  casepr(L16                  );
  casepr(VERTEXDATA           );
  casepr(INDEX16              );
  casepr(INDEX32              );

  casepr(Q16W16V16U16         );
  casepr(MULTI2_ARGB8         );
  // Floating point surface formats
  // s10e5 formats (16-bits per channel)
  casepr(R16F                 );
  casepr(G16R16F              );
  casepr(A16B16G16R16F        );
  casepr(R32F                 ); // IEEE s23e8 formats (32-bits per channel)
  casepr(G32R32F              );
  casepr(A32B32G32R32F        );
  casepr(CxV8U8               );
/* D3D9Ex only -- */
#if !defined(D3D_DISABLE_9EX)
  casepr(A1                   ); // Monochrome 1 bit per pixel format
  casepr(A2B10G10R10_XR_BIAS  ); // 2.8 biased fixed point
  casepr(BINARYBUFFER         ); // Binary format indicating that the data has no inherent type
#endif // !D3D_DISABLE_9EX
/* -- D3D9Ex only */
  casepr(FORCE_DWORD          );
  default:return _T("Unknown pixelformat");
  }
}
