#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS 1

#ifdef WINAPI_FAMILY
#undef WINAPI_FAMILY
#endif

#if _WIN64
#pragma message( "64-bit compilation")
#define IS64BIT
#elif _WIN32
#pragma message( "32-bit compilation")
#define IS32BIT
#endif

#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP

#define __ORDER_BIG_ENDIAN      0
#define __ORDER_LITTLE_ENDIAN__ 1

#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__

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
