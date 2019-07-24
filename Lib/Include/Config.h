#pragma once

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

