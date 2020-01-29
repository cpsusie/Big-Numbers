#pragma once

#include <Config.h>

#ifndef DEBUG_POLYGONIZER
#define D3DGRAPHICS_TM_LIB_VERSION TM_LIB_VERSION
#else
#define D3DGRAPHICS_TM_LIB_VERSION LIBROOT _PLATFORM_ _TMPREFIX_ "DEBUG_POLYGONIZER/"
#endif

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " D3DGRAPHICS_TM_LIB_VERSION
#endif
#pragma comment(lib, D3DGRAPHICS_TM_LIB_VERSION "D3DGraphics.lib")
