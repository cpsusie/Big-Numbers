#pragma once

#include "MyString.h"

class Exception {
private:
  String m_msg;
public:
  Exception() {
  }
  Exception(const String &msg) : m_msg(msg) {
  }
  const TCHAR *what() const {
    return m_msg.cstr();
  }
};

class TimeoutException : public Exception {
public:
  TimeoutException(const TCHAR *msg) : Exception(msg) {
  }
};

extern void exceptionTranslator(UINT u, EXCEPTION_POINTERS *pExp);

extern void throwException(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
extern void throwException(const String &s);
extern void throwInvalidArgumentException(const TCHAR *function, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
extern void throwUnsupportedOperationException(const TCHAR *method);
extern void throwTimeoutException(_In_z_ _Printf_format_string_  const TCHAR *format, ...);
extern void throwMethodException(const TCHAR *className, TCHAR const * const method, _In_z_ _Printf_format_string_ const TCHAR *format, ...);
extern void throwMethodInvalidArgumentException(const TCHAR *className, const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const _Format, ...);

extern void throwErrNoOnNameException(const String &name);
extern void throwErrNoOnSysCallException(const TCHAR *function);

extern void throwLastErrorOnSysCallException(const TCHAR *function);
extern void throwMethodLastErrorOnSysCallException(const TCHAR *className, const TCHAR *method);

#ifdef IS32BIT
#define CHECKUINT64ISVALIDSIZET(size64)                         \
if (size64 > UINT_MAX) {                                        \
  throwException(_T("%s:%s (=%s) exceeds maxint32 (=%s)")       \
                ,__TFUNCTION__                                  \
                ,_T(#size64)                                    \
                , format1000(size64).cstr()                     \
                , format1000(UINT_MAX).cstr());                 \
}
#else
#define CHECKUINT64ISVALIDSIZET(size64)
#endif // IS32BIT
