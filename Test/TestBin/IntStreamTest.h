#pragma once

#include <Math/Int128.h>

void testIntStream();

#ifdef __NEVER__

#define USE_INT64_AS_INTTYPE
#ifdef USE_INT64_AS_INTTYPE
typedef INT64           _inttype;
typedef UINT64          _uinttype;
#define MINIVALUE       _I64_MIN
#define MAXIVALUE       _I64_MAX
#define MAXUIVALUE      _UI64_MAX
#define BITCOUNT        64
#define _itypetoa       _i64toa
#define _uitypetoa      _ui64toa
#define _itypetow       _i64tow
#define _uitypetow      _ui64tow
#define _atoitype       _atoi64
#define _atouitype      _atoui64
#define _wcstoitype     _wcstoi64
#define _wcstouitype    _wcstoui64
#define randuitype      getRandInt64
#else // USE_INT64_AS_INTTYPE

typedef _int128         _inttype;
typedef _uint128        _uinttype;
#define MINIVALUE       _I128_MIN
#define MAXIVALUE       _I128_MAX
#define MAXUIVALUE      _UI128_MAX
#define BITCOUNT        128
#define _itypetoa       _i128toa
#define _uitypetoa      _ui128toa
#define _itypetow       _i128tow
#define _uitypetow      _ui128tow
#define _atoitype       _atoi128
#define _atouitype      _atoui128
#define _wcstoitype     _wcstoi128
#define _wcstouitype    _wcstoui128
#define randuitype      getRandInt128

#endif // USE_INT64_AS_INTTYPE

#ifdef _UNICODE
#define _itypetot       _itypetow
#define _uitypetot      _uitypetow
#define _tcstoitype     _wcstoitype
#define _tcstouitype    _wcstouitype
#else
#define _itypetot       _itypetoa
#define _uitypetot      _uitypetoa
#define _tcstoitype     _atoitype
#define _tcstouitype    _atouitype
#endif // _UNICODE

#define MINRADIX 2
#define MAXRADIX 36

#endif
