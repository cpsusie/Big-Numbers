#pragma once

#include <D3D9.h>
#include <D3DX9.h>

#define DIRECT3DDEVICE LPDIRECT3DDEVICE9EX

class D3DeviceFactory {
private:
  static LPDIRECT3D9EX s_direct3D;
  static void initDirect3D();
public:
  ~D3DeviceFactory();
  static DIRECT3DDEVICE        createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param = NULL, UINT adapter = D3DADAPTER_DEFAULT);
  static D3DDISPLAYMODE        getDisplayMode(                                              UINT adapter = D3DADAPTER_DEFAULT);
  static D3DPRESENT_PARAMETERS getDefaultPresentParameters(HWND hwnd                      , UINT adapter = D3DADAPTER_DEFAULT);
};

#include "PragmaLib.h"
