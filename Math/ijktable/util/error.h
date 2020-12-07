#pragma once

#include <string>

using namespace std;

class Exception {
private:
  const string m_what;
public:
  Exception(const string &msg) : m_what(msg) {
  }
  const char *what() const {
    return m_what.c_str();
  }
};

void throwException(                                        _In_z_ _Printf_format_string_ char const * const format, ...);
void throwInvalidArgumentException(     const char *method, _In_z_ _Printf_format_string_ char const * const format, ...);
void throwUnsupportedOperationException(const char *method);
void throwIndexOutOfRangeException(     const char *method, uint64 index, uint64 size);
void throwIndexOutOfRangeException(     const char *method, uint64 index, uint64 count, uint64 size);
void throwPROCEDURE_ERROR(              const char *method, const string &msg);
void throwPROCEDURE_ERROR(              const char *method, _In_z_ _Printf_format_string_ char const * const format, ...);

void throwErrNoOnNameException(   const string &name);
void throwErrNoOnSysCallException(const char   *method);

#ifdef _DEBUG
void xassert(const char *fileName, int line, const char *exp);
#define Assert(exp) (void)( (exp) || (xassert(__FILE__, __LINE__, #exp), 0) )
#else
#define Assert(exp)
#endif

