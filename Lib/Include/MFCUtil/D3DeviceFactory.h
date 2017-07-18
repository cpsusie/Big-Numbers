#pragma once

#include <D3D9.h>
#include <D3DX9.h>

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

#ifdef _DEBUG

inline int _getrc(IUnknown *p) {
  p->AddRef(); return p->Release();
}

#define TRACE_NEW(     p)    debugLog(_T("NEW:%p:%s(%d):%s\n")                       ,(void*)p          ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_DELETE(  p)    debugLog(_T("DELETE:%p:%s(%d):%s\n")                    ,(void*)p          ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_CREATE(  p)    debugLog(_T("REFCNT:create:%p:refCount=%d:%s(%d):%s\n" ),(void*)p,_getrc(p),__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_ADDREF(  p, n) debugLog(_T("REFCNT:addref:%p:refCount=%d:%s(%d):%s\n" ),(void*)p,n        ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_RELEASE( p, n) debugLog(_T("REFCNT:release:%p:refCount=%d:%s(%d):%s\n"),(void*)p,n        ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_REFCOUNT(p)    debugLog(_T("REFCNT:trace:%p:refCount=%d:%s(%d):%s\n"  ),(void*)p,_getrc(p),__TFUNCTION__,__LINE__,_T(#p))
#else
#define TRACE_NEW(     p)
#define TRACE_DELETE(  p)
#define TRACE_CREATE(  p)
#define TRACE_ADDREF(  p, n)
#define TRACE_RELEASE( p, n)
#define TRACE_REFCOUNT(p)
#endif

#define SAFEADDREF(s)                   \
{ const int _refcnt = (s)->AddRef();    \
  TRACE_ADDREF(s,_refcnt);              \
}

#define SAFERELEASE(s)                  \
{ if(s) {                               \
    const int _refcnt = (s)->Release(); \
    TRACE_RELEASE(s,_refcnt);           \
    s = NULL;                           \
  }                                     \
}

#define SAFEDELETE(p)                  \
{ if((p) != NULL) {                    \
    TRACE_DELETE(p);                   \
    delete p;                          \
    p = NULL;                          \
  }                                    \
}

#define SAFEDELETEARRAY(p)             \
{ if((p) != NULL) {                    \
    TRACE_DELETE(p);                   \
    delete[] p;                        \
    p = NULL;                          \
  }                                    \
}

class D3DeviceFactory {
private:
  static LPDIRECT3D s_direct3D;
  static void initDirect3D();
public:
  ~D3DeviceFactory();
  static LPDIRECT3DDEVICE             createDevice(HWND hwnd, D3DPRESENT_PARAMETERS *param = NULL, UINT adapter = D3DADAPTER_DEFAULT);
  static D3DDISPLAYMODE               getDisplayMode(                                              UINT adapter = D3DADAPTER_DEFAULT);
  static CompactArray<D3DDISPLAYMODE> getDisplayModes(                                             UINT adapter = D3DADAPTER_DEFAULT);
  static D3DPRESENT_PARAMETERS        getDefaultPresentParameters(HWND hwnd                      , UINT adapter = D3DADAPTER_DEFAULT);
  static bool supportFormatConversion(D3DDEVTYPE deviceType, D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter = D3DADAPTER_DEFAULT);
};

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

