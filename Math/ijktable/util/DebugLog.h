#pragma once

#include <stdio.h>
#include <string>

using namespace std;

void redirectDebugLog(bool append = false, const char *fileName = NULL);
void unredirectDebugLog();
bool isDebugLogRedirected();
void vdebugLog(_In_z_ _Printf_format_string_ char const * const format, va_list argptr);
void debugLog(_In_z_ _Printf_format_string_ char const * const format, ...);
void debugLogLine(const char *fileName, int line);

#ifdef _DEBUG
#define DEBUGLOG(...) debugLog(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

#define DEBUGTRACE debugLogLine(__FILE__, __LINE__)
#define ENTERFUNC  debugLog("Enter %s\n", __FUNCTION__)
#define LEAVEFUNC  debugLog("Leave %s\n", __FUNCTION__)

string hexdumpString(const void *data, size_t size);
// if f == NULL, use debugLog as output
void   hexdump(const void *data, size_t size, FILE *f = NULL);

