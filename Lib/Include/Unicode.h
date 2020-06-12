#pragma once

#include <tchar.h>

#include <ostream>
#include <istream>
#include <fstream>
#include <iostream>
#include <sstream>

#if defined(UNICODE)

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

#define tfilebuf    std::filebuf
#define tistream    std::istream
#define tostream    std::ostream
#define tiostream   std::iostream
#define tfstream    std::fstream
#define tifstream   std::ifstream
#define tofstream   std::ofstream
#define tistrstream std::istringstream
#define tostrstream std::ostringstream

#define tcin      std::cin
#define tcout     std::cout
#define tcerr     std::cerr
#define tclog     std::clog

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
