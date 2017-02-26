#pragma once

#include <PragmaLib.h>

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " LIB_VERSION)
#endif
#pragma comment(lib, LIB_VERSION "D3DGraphics.lib")

#define DIRECTXROOTPATH  "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Lib/"
#if defined _M_IX86
#define DIRECTXLIB DIRECTXROOTPATH "x86/"
#elif defined _M_X64
#define DIRECTXLIB DIRECTXROOTPATH "x64/"
#endif

#pragma comment(lib, DIRECTXLIB "d3d9.lib")
#pragma comment(lib, DIRECTXLIB "d3dx9.lib")
