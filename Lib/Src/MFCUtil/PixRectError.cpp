#include "stdafx.h"
#include <MyUtil.h>
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

#ifdef _DEBUG

void check3DResult(TCHAR *fileName, int line, HRESULT hr) {
  if(hr != D3D_OK) {
    AfxMessageBox(format(_T("D3D-error %s in %s, line %d"), get3DErrorMsg(hr).cstr(), fileName, line).cstr(), MB_ICONSTOP);
    exit(-1);
  }
}

#else

void check3DResult(HRESULT hr) {
  if(hr != D3D_OK) {
    AfxMessageBox(format(_T("D3D-error %s"), get3DErrorMsg(hr).cstr()).cstr(), MB_ICONSTOP);
    exit(-1);
  }
}

#endif
