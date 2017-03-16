#pragma once

#include <PragmaLib.h>

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " LIB_VERSION)
#endif
#pragma comment(lib, LIB_VERSION "D3DGraphics.lib")

#define DIRECTXROOTPATH  "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/"
#ifdef _M_X64
#define _DIRECTXPLATFORM_ "x64/"
#else
#define _DIRECTXPLATFORM_ "x86/"
#endif

#define DIRECTXLIB_VERSION DIRECTXROOTPATH _DIRECTXPLATFORM_

#pragma comment(lib, DIRECTXLIB_VERSION "d3d9.lib" )
#pragma comment(lib, DIRECTXLIB_VERSION "d3dx9.lib")
