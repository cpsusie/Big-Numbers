#pragma once

#include <Scandir.h>

inline String childName(const String &path, const String &fname) {
  return (path == _T(".")) ? fname : FileNameSplitter::getChildName(path, fname);
}

inline DirList getAllFiles(const String &dir) {
  return scandir(childName(dir, _T("*.*")), SELECTFILE);
}

inline DirList getAllSubDir(const String &dir) {
  return scandir(childName(dir, _T("*.*")), SELECTSUBDIR);
}

inline size_t getSubDirCount(const String &dir) {
  return getAllSubDir(dir).size();
}

inline int alphasort(const String &s1, const String &s2) {
  return _tcsicmp(s1.cstr(), s2.cstr());
}

inline void libError(const String &line, size_t lineNumber) {
  throwException(_T("Illegal input line %zu:%s\n"), lineNumber, line.cstr());
}

inline String createMapFileName(const String &libName) {
  return FileNameSplitter(FileNameSplitter::getChildName(libName
       , FileNameSplitter(libName).getFileName())).setExtension(_T("txt")).getFullPath();
}

extern bool overwriteAll;
bool canOverWrite(const String &fname);

#define PR_ADDNL   0x01 /* add a newline after message */
#define PR_1LINE   0x02 /* prefix printed text with \r and truncate text to fit in console window */

// flags:Any combination of PR_ADDNL and PR_1LINE
// All trailing spaces (incl \n,\r) are removed from text, before printed
void vprintMessage(BYTE flags, _In_z_ _Printf_format_string_ TCHAR const * const Format, va_list argptr);
void printMessage( BYTE flags, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
void printMessage(             _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
void printWarning(             _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
inline void printException(const Exception &e) {
  printWarning(_T("%s"), e.what());
}
inline void printNewline() {
  printMessage(PR_ADDNL, EMPTYSTRING);
}
