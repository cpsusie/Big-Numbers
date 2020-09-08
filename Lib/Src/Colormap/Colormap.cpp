#include "stdafx.h"
#include "Colormap.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CColormapApp theApp;

const GUID CDECL _tlid = { 0x60305F72, 0xC994, 0x4F73, { 0xB8, 0xC3, 0xD3, 0x6A, 0x9D, 0x54, 0x3C, 0x67 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 1;

// CColormapApp::InitInstance - DLL initialization
BOOL CColormapApp::InitInstance() {
    BOOL bInit = COleControlModule::InitInstance();

    if(bInit) {
        // TODO: Add your own module initialization code here.
    }
    return bInit;
}

// CColormapApp::ExitInstance - DLL termination
int CColormapApp::ExitInstance() {
    // TODO: Add your own module termination code here.
    return COleControlModule::ExitInstance();
}

// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer() {
    AFX_MANAGE_STATE(_afxModuleAddrThis);

  if(!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid)) {
    return ResultFromScode(SELFREG_E_TYPELIB);
  }
  if(!COleObjectFactoryEx::UpdateRegistryAll(TRUE)) {
    return ResultFromScode(SELFREG_E_CLASS);
  }
    return NOERROR;
}

// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer() {
    AFX_MANAGE_STATE(_afxModuleAddrThis);

  if(!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor)) {
    return ResultFromScode(SELFREG_E_TYPELIB);
  }
  if(!COleObjectFactoryEx::UpdateRegistryAll(FALSE)) {
    return ResultFromScode(SELFREG_E_CLASS);
  }
    return NOERROR;
}
