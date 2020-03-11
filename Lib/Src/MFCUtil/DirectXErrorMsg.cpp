#include "pch.h"
#include <MFCUtil/DirectXDeviceFactory.h>

String getDirectXErrorMsg(HRESULT hr) {

#define casepr(v) case v: return _T(#v)

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
  default: return format(_T("Unknown D3D-error:%x"), hr);
  }
}

#ifdef _DEBUG

void checkDirectXResult(const TCHAR *method, HRESULT hr, bool exitOnError) {
  if(hr != D3D_OK) {
    if(exitOnError) {
      throwException(_T("D3D-error %s in %s"), getDirectXErrorMsg(hr).cstr(), method);
//      showError(_T("D3D-error %s in %s"), getDirectXErrorMsg(hr).cstr(), method);
//      exit(-1);
    } else {
      throwException(_T("D3D-error %s in %s"), getDirectXErrorMsg(hr).cstr(), method);
    }
  }
}

#else // _DEBUG

void checkDirectXResult(HRESULT hr, bool exitOnError) {
  if(hr != D3D_OK) {
    if(exitOnError) {
      showError(_T("D3D-error %s"), getDirectXErrorMsg(hr).cstr());
      exit(-1);
    } else {
      throwException(_T("D3D-error %s"), getDirectXErrorMsg(hr).cstr());
    }
  }
}

#endif // _DEBUG
