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

#define TMEMSET( dst, v  , count)             wmemset( dst, v  , count)
#define TMEMCPY( dst, src, count)             wmemcpy( dst, src, count)
#define TMEMMOVE(dst, src, count)             wmemmove(dst, src, count)
#define TMEMCMP( s1 , s2 , count)             wmemcmp( s1 , s2 , count)

#define tstring std::wstring

#define ttoi std::wcstol

#else // !unicode

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

#define TMEMSET( dst, v  , count)             memset( dst, v  , count)
#define TMEMCPY( dst, src, count)             memcpy( dst, src, count)
#define TMEMMOVE(dst, src, count)             memmove(dst, src, count)
#define TMEMCMP( s1 , s2 , count)             memcmp( s1 , s2 , count)

#define tstring std::string

#define ttoi std::stoi

#endif // unicode

class StreamDelimiter {
public:
};

inline tostream &operator<<(tostream &out, const StreamDelimiter &d) {
  out << std::endl;
  return out;
}
