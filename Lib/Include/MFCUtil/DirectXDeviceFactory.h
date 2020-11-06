#pragma once

#include <D3D9.h>
#include <D3DX9.h>
#include <CompactArray.h>
#include <Semaphore.h>
#include <Singleton.h>

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

class DirectXDeviceFactory : public Singleton {
private:
  LPDIRECT3D m_direct3D;
  Semaphore  m_lock;

  void initDirect3D();
  DirectXDeviceFactory();
  ~DirectXDeviceFactory() override;
public:
  LPDIRECT3DDEVICE             createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param = nullptr, UINT adapter = D3DADAPTER_DEFAULT);
  D3DDISPLAYMODE               getDisplayMode(                                                 UINT adapter = D3DADAPTER_DEFAULT);
  CompactArray<D3DDISPLAYMODE> getDisplayModes(                                                UINT adapter = D3DADAPTER_DEFAULT);
  D3DPRESENT_PARAMETERS        getDefaultPresentParameters(HWND hwnd                         , UINT adapter = D3DADAPTER_DEFAULT);
  bool                         supportFormatConversion(D3DDEVTYPE deviceType, D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter = D3DADAPTER_DEFAULT);
  DEFINESINGLETON(DirectXDeviceFactory)
};

D3DVERTEXBUFFER_DESC getvDesc(LPD3DXMESH             mesh        );
D3DINDEXBUFFER_DESC  getiDesc(LPD3DXMESH             mesh        );
D3DVERTEXBUFFER_DESC getDesc( LPDIRECT3DVERTEXBUFFER vertexBuffer);
D3DINDEXBUFFER_DESC  getDesc( LPDIRECT3DINDEXBUFFER  indexBuffer );

#if defined(_DEBUG)

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
#if defined(_M_X64)
#define _DIRECTXPLATFORM_ "x64/"
#else
#define _DIRECTXPLATFORM_ "x86/"
#endif

#define DIRECTXLIB_VERSION DIRECTXROOTPATH _DIRECTXPLATFORM_

#pragma comment(lib, DIRECTXLIB_VERSION "d3d9.lib" )
#pragma comment(lib, DIRECTXLIB_VERSION "d3dx9.lib")
