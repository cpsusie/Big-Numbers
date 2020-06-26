#include "pch.h"
#include <MyUtil.h>
#include <MFCUtil/DirectXDeviceFactory.h>

DEFINESINGLETON(DirectXDeviceFactory);

DirectXDeviceFactory &DirectXDeviceFactory::getInstance() { // static
  return getDirectXDeviceFactory();
};

DirectXDeviceFactory::DirectXDeviceFactory(SingletonFactory *factory)
: Singleton(factory)
, m_direct3D(NULL)
{
  initDirect3D();
}

DirectXDeviceFactory::~DirectXDeviceFactory() {
  SAFERELEASE(m_direct3D);
}

void DirectXDeviceFactory::initDirect3D() {
  m_lock.wait();
  try {
    Direct3DCreate9Ex(D3D_SDK_VERSION, &m_direct3D);
    if(m_direct3D == NULL) {
      const TCHAR *msg = _T("Cannot initialize Direct3D");
      showError(msg);
      throwException(_T("%s"), msg);
    }
    TRACE_CREATE(m_direct3D);
    m_lock.notify();
  } catch(...) {
    m_lock.notify();
    throw;
  }
}

LPDIRECT3DDEVICE DirectXDeviceFactory::createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param, UINT adapter) {
  if(m_direct3D == NULL) {
    initDirect3D();
  }

  D3DPRESENT_PARAMETERS tmpParam;
  if(param == NULL) {
    tmpParam = getDefaultPresentParameters(hwnd, adapter);
    param = &tmpParam;
  }
  m_lock.wait();
  try {
    LPDIRECT3DDEVICE device;
    CHECKRESULT(m_direct3D->CreateDeviceEx(adapter
                                          ,D3DDEVTYPE_HAL
                                          ,hwnd
                                          ,D3DCREATE_SOFTWARE_VERTEXPROCESSING
                                          ,param
                                          ,NULL
                                          ,&device));
    TRACE_CREATE(device);
    m_lock.notify();
    return device;
  } catch(...) {
    m_lock.notify();
    throw;
  }
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

D3DDISPLAYMODE DirectXDeviceFactory::getDisplayMode(UINT adapter) {
  D3DDISPLAYMODE result;
  CHECKRESULT(m_direct3D->GetAdapterDisplayMode(adapter, &result));
  return result;
}

CompactArray<D3DDISPLAYMODE> DirectXDeviceFactory::getDisplayModes(UINT adapter) {
  D3DDISPLAYMODE adapterMode = DirectXDeviceFactory::getDisplayMode(adapter);
  const UINT     modeCount   = m_direct3D->GetAdapterModeCount(adapter, adapterMode.Format);
  CompactArray<D3DDISPLAYMODE> result;
  for(UINT mode = 0; mode < modeCount; mode++) {
    D3DDISPLAYMODE dp;
    CHECKRESULT(m_direct3D->EnumAdapterModes(adapter, adapterMode.Format, mode, &dp));
    result.add(dp);
  };
  return result;
}

bool DirectXDeviceFactory::supportFormatConversion(D3DDEVTYPE deviceType, D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter) {
  m_lock.wait();
  const HRESULT hr = m_direct3D->CheckDeviceFormatConversion(adapter, deviceType, srcFormat, dstFormat);
  m_lock.notify();
  switch(hr) {
  case D3D_OK             : return true;
  case D3DERR_NOTAVAILABLE: return false;
  default                 : CHECKRESULT(hr);
                            return false;
  }
}
