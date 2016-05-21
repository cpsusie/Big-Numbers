#pragma once

//#define GIVE_LINK_MESSAGE

#ifdef _DEBUG
#define _UTIL_LIB_VERSION "MTDebug32/util.lib"
#else
#define _UTIL_LIB_VERSION "MTRelease32/util.lib"
#endif

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " _UTIL_LIB_VERSION)
#endif
#pragma comment(lib, "c:/mytools2015/lib/" _UTIL_LIB_VERSION)
