#pragma once

//#define GIVE_LINK_MESSAGE

#define LIBROOT "c:/mytools2015/lib/"

#ifdef _M_X64
#define _PLATFORM_ "x64/"
#else
#define _PLATFORM_ "Win32/"
#endif

#ifdef TRACE_MEMORY
#define _TMPREFIX_ "TraceMem"
#else
#define _TMPREFIX_
#endif

#ifdef _DEBUG
#define _CONFIGURATION_ "Debug/"
#else
#define _CONFIGURATION_ "Release/"
#endif

#define LIB_VERSION    LIBROOT _PLATFORM_ _CONFIGURATION_
#define TM_LIB_VERSION LIBROOT _PLATFORM_ _TMPREFIX_ _CONFIGURATION_

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " TM_LIB_VERSION)
#endif
#pragma comment(lib, TM_LIB_VERSION "Util.lib")
