#pragma once

//#define GIVE_LINK_MESSAGE

#define LIBROOT "c:/mytools2015/lib/"

#ifdef IS32BIT
#ifdef _DEBUG
#define LIB_VERSION LIBROOT "MTDebug32/"
#else
#define LIB_VERSION LIBROOT "MTRelease32/"
#endif

#else

#ifdef _DEBUG
#define LIB_VERSION LIBROOT "MTDebug64/"
#else
#define LIB_VERSION LIBROOT "MTRelease64/"
#endif

#endif

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " LIB_VERSION)
#endif
#pragma comment(lib, LIB_VERSION "util.lib")
