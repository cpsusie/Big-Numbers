#include "pch.h"
#include <MyUtil.h>
#include <MFCUtil/DirectXDeviceFactory.h>

LPDIRECT3D DirectXDeviceFactory::s_direct3D = NULL;

void DirectXDeviceFactory::initDirect3D() { // static
  Direct3DCreate9Ex( D3D_SDK_VERSION, &s_direct3D);
  if(s_direct3D == NULL) {
    const TCHAR *msg = _T("Cannot initialize Direct3D");
    showError(msg);
    throwException(_T("%s"), msg);
  }
  TRACE_CREATE(s_direct3D);
}

DirectXDeviceFactory::~DirectXDeviceFactory() {
  SAFERELEASE(s_direct3D);
}

static DirectXDeviceFactory deviceFactory;

LPDIRECT3DDEVICE DirectXDeviceFactory::createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param, UINT adapter) { // static
  if(s_direct3D == NULL) {
    initDirect3D();
  }

  D3DPRESENT_PARAMETERS tmpParam;
  if(param == NULL) {
    tmpParam = getDefaultPresentParameters(hwnd, adapter);
    param = &tmpParam;
  }
  LPDIRECT3DDEVICE device;

  CHECKRESULT(s_direct3D->CreateDeviceEx(adapter
                                        ,D3DDEVTYPE_HAL
                                        ,hwnd
                                        ,D3DCREATE_SOFTWARE_VERTEXPROCESSING
                                        ,param
                                        ,NULL
                                        ,&device));
  TRACE_CREATE(device);

  return device;
}

D3DPRESENT_PARAMETERS DirectXDeviceFactory::getDefaultPresentParameters(HWND hwnd, UINT adapter) { // static
  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  const CSize          winSize     = getClientRect(hwnd).Size();
  const D3DDISPLAYMODE displayMode = getDisplayMode(adapter);

  param.Windowed               = TRUE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.EnableAutoDepthStencil = TRUE;
  param.AutoDepthStencilFormat = D3DFMT_D16;
  param.BackBufferFormat       = displayMode.Format;
  param.BackBufferWidth        = winSize.cx;
  param.BackBufferHeight       = winSize.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
  return param;
}

D3DDISPLAYMODE DirectXDeviceFactory::getDisplayMode(UINT adapter) { // static
  D3DDISPLAYMODE result;
  CHECKRESULT(s_direct3D->GetAdapterDisplayMode(adapter, &result));
  return result;
}

CompactArray<D3DDISPLAYMODE> DirectXDeviceFactory::getDisplayModes(UINT adapter) { // static
  CompactArray<D3DDISPLAYMODE> result;
  D3DDISPLAYMODE adapterMode = DirectXDeviceFactory::getDisplayMode(adapter);
  const UINT modeCount = s_direct3D->GetAdapterModeCount(adapter, adapterMode.Format);
  for(UINT mode = 0; mode < modeCount; mode++) {
    D3DDISPLAYMODE dp;
    CHECKRESULT(s_direct3D->EnumAdapterModes(adapter, adapterMode.Format, mode, &dp));
    result.add(dp);
  };
  return result;
}

bool DirectXDeviceFactory::supportFormatConversion(D3DDEVTYPE deviceType, D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter) { // static
  const HRESULT hr = s_direct3D->CheckDeviceFormatConversion(adapter, deviceType, srcFormat, dstFormat);
  switch(hr) {
  case D3D_OK             : return true;
  case D3DERR_NOTAVAILABLE: return false;
  default                 : CHECKRESULT(hr);
                            return false;
  }
}
