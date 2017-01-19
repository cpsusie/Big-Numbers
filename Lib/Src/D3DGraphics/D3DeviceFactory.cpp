#include "pch.h"
#include <MFCUtil/WinTools.h>

DECLARE_THISFILE;

LPDIRECT3D9EX D3DeviceFactory::s_direct3D = NULL;

void D3DeviceFactory::initDirect3D() { // static
  V(Direct3DCreate9Ex( D3D_SDK_VERSION, &s_direct3D));
  if(s_direct3D == NULL) {
    const TCHAR *msg = _T("Cannot initialize Direct3D");
    Message(_T("%s"), msg);
    throwException(_T("%s"), msg);
  }
}

D3DeviceFactory::~D3DeviceFactory() {
  if(s_direct3D != NULL) {
    s_direct3D->Release();
    s_direct3D = NULL;
  }
}

static D3DeviceFactory deviceFactory;

DIRECT3DDEVICE D3DeviceFactory::createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param, UINT adapter) { // static
  if(s_direct3D == NULL) {
    initDirect3D();
  }

  D3DPRESENT_PARAMETERS tmpParam;
  if(param == NULL) {
    tmpParam = getDefaultPresentParameters(hwnd, adapter);
    param = &tmpParam;
  }

  DIRECT3DDEVICE device;

  V(s_direct3D->CreateDeviceEx(adapter
                              ,D3DDEVTYPE_HAL
                              ,hwnd
                              ,D3DCREATE_SOFTWARE_VERTEXPROCESSING
                              ,param
                              ,NULL
                              ,&device));
  return device;
}

D3DDISPLAYMODE D3DeviceFactory::getDisplayMode(UINT adapter) { // static 
  D3DDISPLAYMODE result;
  V(s_direct3D->GetAdapterDisplayMode(adapter, &result));
  return result;
}

D3DPRESENT_PARAMETERS D3DeviceFactory::getDefaultPresentParameters(HWND hwnd, UINT adapter) { // static
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

