#pragma once

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

void exceptionTranslator(UINT u, EXCEPTION_POINTERS *pExp);

void throwException(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
void throwException(const String &s);
void throwInvalidArgumentException(const TCHAR *method, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
void throwUnsupportedOperationException(const TCHAR *method);
void throwTimeoutException(_In_z_ _Printf_format_string_  const TCHAR * const format, ...);
void throwIndexOutOfRangeException(const TCHAR *method, UINT64 index, UINT64 size);
void throwIndexOutOfRangeException(const TCHAR *method, UINT64 index, UINT64 count, UINT64 size);
void throwSelectFromEmptyCollectionException(const TCHAR *method);
void throwMethodException(const TCHAR *className, TCHAR const * const method, _In_z_ _Printf_format_string_ const TCHAR * const format, ...);

void throwErrNoOnNameException(const String &name);
void throwErrNoOnSysCallException(const TCHAR *method);

void throwLastErrorOnSysCallNameException(const String &name);
void throwLastErrorOnSysCallException(const TCHAR *method);
void throwMethodLastErrorOnSysCallException(const TCHAR *className, const TCHAR *method);

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
