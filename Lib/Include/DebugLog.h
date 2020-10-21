#pragma once

void redirectDebugLog(bool append = false, const TCHAR *fileName = nullptr);
void unredirectDebugLog();
bool isDebugLogRedirected();
void vdebugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
void debugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
void debugLogLine(const TCHAR *fileName, int line);
void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime);
typedef void(*vprintfFunction)(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
typedef void(*printfFunction)(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);

#if defined(_DEBUG)
#define DEBUGLOG(...) debugLog(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

#define DEBUGTRACE debugLogLine(__TFILE__, __LINE__)
#define ENTERFUNC  debugLog(_T("Enter %s\n"), __TFUNCTION__)
#define LEAVEFUNC  debugLog(_T("Leave %s\n"), __TFUNCTION__)

// if f == nullptr, use debugLog as output
void   hexdump(const void *data, size_t size, FILE *f = nullptr);

