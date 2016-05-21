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

extern void exceptionTranslator(unsigned int u, EXCEPTION_POINTERS* pExp);

extern void throwException(const TCHAR *format, ...);
extern void throwException(const String &s);
extern void throwInvalidArgumentException(const TCHAR *function, const TCHAR *format, ...);
extern void throwTimeoutException(const TCHAR *format, ...);
extern void throwMethodException(const TCHAR *className, const TCHAR *method, const TCHAR *format, ...);
extern void throwMethodInvalidArgumentException(const TCHAR *className, const TCHAR *method, const TCHAR *format, ...);
extern void throwMethodUnsupportedOperationException(const TCHAR *className, const TCHAR *method);

extern void throwErrNoOnNameException(const String &name);
extern void throwErrNoOnSysCallException(const TCHAR *function);

extern void throwLastErrorOnSysCallException(const TCHAR *function);
extern void throwMethodLastErrorOnSysCallException(const TCHAR *className, const TCHAR *method);
