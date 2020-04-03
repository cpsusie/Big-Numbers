#pragma once

#include <windows.h>
#include <string>
#include "MemoryTrace.h"

#ifdef _M_X64
#define _PLATFORM_ "x64/"
#else
#define _PLATFORM_ "win32/"
#endif

#ifdef _DEBUG
#define _CONFIGURATION_ "Debug/"
#else
#define _CONFIGURATION_ "Release/"
#endif

#define UTILLIB_ROOT "c:/mytools2015/math/ijktable/util/"
#define UTILLIB_NAME "util.lib"

#define UTILLIB_VERSION UTILLIB_ROOT _PLATFORM_ _CONFIGURATION_

#pragma comment(lib, UTILLIB_VERSION UTILLIB_NAME)

using namespace std;

typedef unsigned char    uchar;
typedef unsigned short   ushort;
typedef unsigned long    ulong;
typedef unsigned int     uint;
typedef __int64          int64;
typedef unsigned __int64 uint64;

#define EMPTYSTRING ""

#define DECLARECLASSNAME            static char const* const s_className
#define DEFINECLASSNAME(className)  char const* const className::s_className = #className

#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif

#define ARRAYSIZE(a)        (sizeof(a)/sizeof((a)[0]))
#define LASTVALUE(a)        (a[ARRAYSIZE(a)-1])

FILE            *mkfopen(const TCHAR   *name, const TCHAR  *mode);
FILE            *mkfopen(const string  &name, const string &mode);
FILE            *FOPEN(  const TCHAR   *name, const TCHAR  *mode);
FILE            *FOPEN(  const string  &name, const string &mode);
FILE            *MKFOPEN(const TCHAR   *name, const TCHAR  *mode);
FILE            *MKFOPEN(const string  &name, const string &mode);

inline int ordinal(bool b) {
  return b ? 1 : 0;
}

inline int sign(float x) {
  return x < 0 ? -1 : x > 0 ? 1 : 0;
}

inline int sign(double x) {
  return x < 0 ? -1 : x > 0 ? 1 : 0;
}

inline int sign(int64 x) {
  return x < 0 ? -1 : x > 0 ? 1 : 0;
}

double fraction(double v);
float  fraction(float  v);

string vformat(_In_z_ _Printf_format_string_ char const * const format, va_list argptr);
string format( _In_z_ _Printf_format_string_ char const * const format, ...);

string format1000(int              n);
string format1000(uint             n);
string format1000(long             n);
string format1000(ulong            n);
string format1000(int64            n);
string format1000(uint64           n);
string formatSize(size_t           n);

string spaceString(uint length, char ch = ' ');

template<typename T> class AbstractStringifier {
public:
  virtual string toString(const T &e) = 0;
};

class IntStringifier : public AbstractStringifier<int> {
public:
  string toString(const int &e);
};

class UIntStringifier : public AbstractStringifier<uint> {
public:
  string toString(const uint &e);
};

class Int64Stringifier : public AbstractStringifier<int64> {
public:
  string toString(const int64 &e);
};

class UInt64Stringifier : public AbstractStringifier<uint64> {
public:
  string toString(const uint64 &e);
};

class SizeTStringifier : public AbstractStringifier<size_t> {
public:
  string toString(const size_t &e);
};

class CharacterFormater : public AbstractStringifier<size_t> {
public:
  static CharacterFormater *stdAsciiFormater;
  static CharacterFormater *extendedAsciiFormater;
  static CharacterFormater *octalEscapedAsciiFormater;
  static CharacterFormater *hexEscapedAsciiFormater;
  static CharacterFormater *hexEscapedExtendedAsciiFormater;
};

class StreamDelimiter {
public:
};

inline ostream &operator<<(ostream &out, const StreamDelimiter &d) {
  out << std::endl;
  return out;
}
