#pragma once

#include <tchar.h>

#include <ostream>
#include <istream>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef UNICODE

#define tfilebuf    std::wfilebuf
#define tistream    std::wistream
#define tostream    std::wostream
#define tiostream   std::wiostream
#define tfstream    std::wfstream
#define tifstream   std::wifstream
#define tofstream   std::wofstream
#define tistrstream std::wistringstream
#define tostrstream std::wostringstream

#define tcin        std::wcin
#define tcout       std::wcout
#define tcerr       std::wcerr
#define tclog       std::wclog

#define MEMSET( dst, v  , count)             wmemset( dst, v  , count)
#define MEMCPY( dst, src, count)             wmemcpy( dst, src, count)
#define MEMMOVE(dst, src, count)             wmemmove(dst, src, count)
#define MEMCMP( s1 , s2 , count)             wmemcmp( s1 , s2 , count)

#define tstring std::wstring

#else

#define tfilebuf    filebuf
#define tistream    istream
#define tostream    ostream
#define tiostream   iostream
#define tfstream    fstream
#define tifstream   ifstream
#define tofstream   ofstream
#define tistrstream istringstream
#define tostrstream ostringstream

#define tcin      cin
#define tcout     cout
#define tcerr     cerr
#define tclog     clog

#define MEMSET( dst, v  , count)             memset( dst, v  , count)
#define MEMCPY( dst, src, count)             memcpy( dst, src, count)
#define MEMMOVE(dst, src, count)             memmove(dst, src, count)
#define MEMCMP( s1 , s2 , count)             memcmp( s1 , s2 , count)

#define tstring std::string

#endif

#define EMPTYSTRING _T("")
