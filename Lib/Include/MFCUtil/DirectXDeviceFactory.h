#pragma once

#include <D3D9.h>
#include <D3DX9.h>
#include <CompactArray.h>

typedef LPDIRECT3D9EX           LPDIRECT3D;
typedef LPDIRECT3DDEVICE9EX     LPDIRECT3DDEVICE;
typedef LPDIRECT3DTEXTURE9      LPDIRECT3DTEXTURE;
typedef LPDIRECT3DSURFACE9      LPDIRECT3DSURFACE;
typedef D3DCAPS9                D3DCAPS;
typedef D3DMATERIAL9            D3DMATERIAL;
typedef D3DLIGHT9               D3DLIGHT;
typedef D3DVIEWPORT9            D3DVIEWPORT;
typedef LPDIRECT3DVERTEXBUFFER9 LPDIRECT3DVERTEXBUFFER;
typedef LPDIRECT3DINDEXBUFFER9  LPDIRECT3DINDEXBUFFER;

class DirectXDeviceFactory {
private:
  static LPDIRECT3D s_direct3D;
  static void initDirect3D();
public:
  ~DirectXDeviceFactory();
  static LPDIRECT3DDEVICE             createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param = NULL, UINT adapter = D3DADAPTER_DEFAULT);
  static D3DDISPLAYMODE               getDisplayMode(                                              UINT adapter = D3DADAPTER_DEFAULT);
  static CompactArray<D3DDISPLAYMODE> getDisplayModes(                                             UINT adapter = D3DADAPTER_DEFAULT);
  static D3DPRESENT_PARAMETERS        getDefaultPresentParameters(HWND hwnd                      , UINT adapter = D3DADAPTER_DEFAULT);
  static bool supportFormatConversion(D3DDEVTYPE deviceType, D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter = D3DADAPTER_DEFAULT);
};

#ifdef _DEBUG

void checkDirectXResult(const TCHAR *method, HRESULT hr, bool exitOnError);

#define V(hr) checkDirectXResult(__TFUNCTION__, hr, true )
#define W(hr) checkDirectXResult(__TFUNCTION__, hr, false)
#else

void checkDirectXResult(HRESULT hr, bool exitOnError);

#define V( hr) checkDirectXResult(hr, true )
#define W(hr)  checkDirectXResult(hr, false)

#endif

#include "PragmaLib.h"

#define DIRECTXROOTPATH  "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/"
#ifdef _M_X64
#define _DIRECTXPLATFORM_ "x64/"
#else
#define _DIRECTXPLATFORM_ "x86/"
#endif

#define DIRECTXLIB_VERSION DIRECTXROOTPATH _DIRECTXPLATFORM_

#pragma comment(lib, DIRECTXLIB_VERSION "d3d9.lib" )
#pragma comment(lib, DIRECTXLIB_VERSION "d3dx9.lib")

