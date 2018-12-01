#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#ifdef WINAPI_FAMILY
#undef WINAPI_FAMILY
#endif

#if _WIN64
#pragma message( "64-bit compilation")
#define IS64BIT
#elif _WIN32
#pragma message( "32-bit compilation")
#define IS32BIT
#endif

#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP

#define __ORDER_BIG_ENDIAN      0
#define __ORDER_LITTLE_ENDIAN__ 1

#define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__

#include <Windows.h>
#include <WinUser.h>
#include <WinBase.h>
#include <stdio.h>
#include <sys\stat.h>
#include "MyString.h"
#include "MathUtil.h"
#include "MyAssert.h"
#include "Comparator.h"
#include "Exception.h"

void redirectDebugLog(bool append = false, const TCHAR *fileName = NULL);
void unredirectDebugLog();
bool isDebugLogRedirected();
void vdebugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
void debugLog( _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
void debugLogLine(const TCHAR *fileName, int line);
void debugLogSetTimePrefix(bool prefixWithDate, bool prefixWithTime);
typedef void (*vprintfFunction)(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
typedef void (*printfFunction )(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);

String getMessageName(int msg);

#define DEBUGTRACE debugLogLine(__TFILE__, __LINE__)
#define ENTERFUNC  debugLog(_T("Enter %s\n"), __TFUNCTION__)
#define LEAVEFUNC  debugLog(_T("Leave %s\n"), __TFUNCTION__)
bool getDebuggerPresent();

#ifdef TRACE_MEMORY

inline int _getrc(IUnknown *p) {
  p->AddRef(); return p->Release();
}

#define TRACE_NEW(     p)    { if(p) debugLog(_T("NEW:%p:%s(%d):%s\n")               ,(void*)(p)          ,__TFUNCTION__,__LINE__,_T(#p)); }
#define TRACE_DELETE(  p)    debugLog(_T("DELETE:%p:%s(%d):%s\n")                    ,(void*)(p)          ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_CREATE(  p)    debugLog(_T("REFCNT:create:%p:refCount=%d:%s(%d):%s\n" ),(void*)(p),_getrc(p),__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_ADDREF(  p, n) debugLog(_T("REFCNT:addref:%p:refCount=%d:%s(%d):%s\n" ),(void*)(p),n        ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_RELEASE( p, n) debugLog(_T("REFCNT:release:%p:refCount=%d:%s(%d):%s\n"),(void*)(p),n        ,__TFUNCTION__,__LINE__,_T(#p))
#define TRACE_REFCOUNT(p)    debugLog(_T("REFCNT:trace:%p:refCount=%d:%s(%d):%s\n"  ),(void*)(p),_getrc(p),__TFUNCTION__,__LINE__,_T(#p))
#else
#define TRACE_NEW(     p)
#define TRACE_DELETE(  p)
#define TRACE_CREATE(  p)
#define TRACE_ADDREF(  p, n)
#define TRACE_RELEASE( p, n)
#define TRACE_REFCOUNT(p)
#endif

#define SAFEADDREF(s)                   \
{ const int _refcnt = (s)->AddRef();    \
  TRACE_ADDREF(s,_refcnt);              \
}

#define SAFERELEASE(s)                  \
{ if(s) {                               \
    const int _refcnt = (s)->Release(); \
    TRACE_RELEASE(s,_refcnt);           \
    s = NULL;                           \
  }                                     \
}

#define SAFEDELETE(p)                   \
{ if((p) != NULL) {                     \
    TRACE_DELETE(p);                    \
    delete (p);                         \
    (p) = NULL;                         \
  }                                     \
}

#define SAFEDELETEARRAY(p)              \
{ if((p) != NULL) {                     \
    TRACE_DELETE(p);                    \
    delete[] (p);                       \
    (p) = NULL;                         \
  }                                     \
}


void  *xmalloc( UINT n);
void  *xrealloc(void *q, UINT n);
TCHAR *xstrdup( const TCHAR *str);
void  xfree(void *p);

#ifdef ARRAYSIZE
#undef ARRAYSIZE
#endif

#define ARRAYSIZE(a)        (sizeof(a)/sizeof((a)[0]))
#define LASTVALUE(a)        (a[ARRAYSIZE(a)-1])

#define MALLOC(type, n)     (type*)xmalloc(sizeof(type)*(n))
#define REALLOC(p, type, n) (type*)xrealloc(p, sizeof(type)*(n))
#define STRDUP(p)           xstrdup(p)
#define FREE(p)             xfree(p)

#ifdef DEBUG
#define NODEFAULT   assert(0)
#else
#define NODEFAULT   __assume(0)
#endif

FILE            *mkfopen(const TCHAR  *name, const TCHAR     *mode);
FILE            *mkfopen(const String &name, const String    &mode);
FILE            *fopen(  const String &name, const String    &mode);
int              unlink( const String &name);
int              stat(   const String &name, struct _stat    &st);
int              stat64( const String &name, struct _stati64 &st);
int              fstat(  FILE *f,            struct _stat    &st);
int              fstat64(FILE *f,            struct _stati64 &st);
int              rename( const String &oldName, const String &newName);
int              chmod(  const String &name, int mode);
int              rmdir(  const String &name);
int              chdir(  const String &dir );
bool             isatty( FILE *f);
// mode = { _O_TEXT, _O_BINARY }. Returns old mode.
int              setFileMode(FILE *f, int mode);
HANDLE           getHandle(FILE *f);

String           getSysErrorText(int errno);
String           getErrnoText();
// uppercase version of the same systemcalls throws an Exception on error containing _sys_errlist[errno] as errortext
FILE            *FOPEN(  const TCHAR   *name, const TCHAR  *mode);
FILE            *FOPEN(  const String  &name, const String &mode);
FILE            *MKFOPEN(const TCHAR   *name, const TCHAR  *mode);
FILE            *MKFOPEN(const String  &name, const String &mode);
void             FWRITE( const void *buffer, size_t size, size_t count, FILE *f);
size_t           FREAD(        void *buffer, size_t size, size_t count, FILE *f);
void             FSEEK(  FILE *f, INT64 offset);
TCHAR           *FGETS(  TCHAR *line, size_t n, FILE *f);
TCHAR           *GETS(   TCHAR *line);
void             UNLINK( const String &name);
int              ACCESS( const String &name, int mode);
struct _stat    &STAT(   const String &name, struct _stat &st);    // return st
struct _stat     STAT(   const String &name);
struct _stati64 &STAT64( const String &name, struct _stati64 &st); // return st
struct _stati64  STAT64( const String &name);
struct _stat    &FSTAT(  FILE *f, struct _stat &st);               // return st
struct _stat     FSTAT(  FILE *f);
struct _stati64 &FSTAT64(FILE *f, struct _stati64 &st);            // return st
struct _stati64  FSTAT64(FILE *f);
void             RENAME( const String &oldName, const String &newName);
void             CHMOD(  const String &name, int mode);
void             RMDIR(  const String &name);
void             CHDIR(  const String &dir);
String           GETCWD();
INT64            GETSIZE(FILE *f);
INT64            GETPOS( FILE *f);
void             PIPE(   int  *pipe, UINT size, int textMode);
int              DUP(    int   fd);
void             DUP2(   int   fd1, int fd2);
HANDLE           GETHANDLE(FILE *f);

String readTextFile(FILE *f);
String readTextFile(const String &fileName);
// Read a text-line, returning true if anything read, false if not.
// Trailing \n and \r are removed
bool   readLine(FILE *f, String &str);

void pause();
void pause(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
#define PAUSE() pause(_T("%s line %d"), __TFILE__, __LINE__)

bool keyPressed(int vk);
bool shiftKeyPressed();
bool ctrlKeyPressed();
unsigned char toAscii(UINT virtualCode);
// read String     from stdin, terminate with enter. result string NOT containing '\n\r'
String  inputString(  _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
// read password   from console
String  inputPassword(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
// read integer from stdin
int     inputInt(     _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
// read UINT    from stdin
UINT    inputUint(    _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
// read INT64   from stdin
INT64   inputInt64(   _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
// read UINT64  from stdin
UINT64  inputUint64(  _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
// read double  from stdin
double  inputDouble(  _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

TCHAR   *searchenv(  TCHAR *dst, const TCHAR *fileName, const TCHAR *envName);
String   searchenv(  const String &fileName, const String &envName);

void argvExpand(int &argc, wchar_t **&argv);
void argvExpand(int &argc, char    **&argv);

const wchar_t **argv2wargv(const char    **argv );
const char    **wargv2argv(const wchar_t **targv);
const TCHAR   **argv2targv(const char    **argv );

// if module == NULL, path of executable
String getModuleFileName(HMODULE module = NULL);
String getUserName();
String getHostName();

TCHAR *getErrorText(TCHAR *text, int size, HRESULT hr);
String getErrorText(HRESULT hr);
String getLastErrorText();
void checkResult(const TCHAR *fileName, int line, ULONG   hresult);
void checkResult(const TCHAR *fileName, int line, HRESULT hresult);
void checkResult(const TCHAR *fileName, int line, BOOL    ok     );
#define CHECKRESULT(result) checkResult(__TFILE__, __LINE__, result)

void   sleep(int seconds);
// microseconds. if process == NULL, return time for current Process
double getProcessTime(HANDLE process = NULL);
// microseconds. if thread  == NULL, return time for current Thread
double getThreadTime( HANDLE thread  = NULL);
double getSystemTime();

extern int UseSafeNew; // assign something to this, and we use safe new_handler, which throws Exception on out of memory
String hexdumpString(const void *data, int size);
// if f == NULL, use debugLog as output
void   hexdump(      const void *data, int size, FILE *f = NULL);
String bytesToString(const void *data, int size);

TCHAR *newGUID(TCHAR *dst);
TCHAR *sprintGUID(TCHAR *dst, const GUID &guid);
GUID  *newGUID(GUID *guid);

void quickSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator);
void quickSort(void *base, size_t nelem, size_t width, int (__cdecl *compare)(const void*, const void*));


inline int ordinal(bool b) {
  return b ? 1 : 0;
}

inline int boolCmp(bool b1, bool b2) {
  return ordinal(b1) - ordinal(b2);
}


// very common used hash- and comparefunctions
inline ULONG shortHash(const short &n) {
  return n;
}

inline int shortHashCmp(const short &n1, const short &n2) {
  return n1 - n2;
}

inline ULONG ushortHash(const USHORT &n) {
  return n;
}

inline int ushortHashCmp(const USHORT &n1, const USHORT &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG intHash(const int &n) {
  return n;
}

inline int intHashCmp(const int &n1, const int &n2) {
  return n1 - n2;
}

inline ULONG uintHash(const UINT &n) {
  return n;
}

inline int  uintHashCmp(const UINT &n1, const UINT &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG longHash(const long &n) {
  return n;
}

inline int longHashCmp(const long &n1, const long &n2) {
  return n1 - n2;
}

inline ULONG ulongHash(const ULONG &n) {
  return n;
}

inline int ulongHashCmp( const ULONG &n1, const ULONG &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG int64Hash(const INT64 &n) {
  return (((ULONG*)&n)[0]) ^ (((ULONG*)&n)[1]);
}

inline int int64HashCmp(const INT64 &n1, const INT64 &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

inline ULONG uint64Hash(const UINT64 &n) {
  return (((ULONG*)&n)[0]) ^ (((ULONG*)&n)[1]);
}

inline int uint64HashCmp(const UINT64 &n1, const UINT64 &n2) {
  return (n1 > n2) ? 1 : (n1 < n2) ? -1 : 0;
}

#ifdef IS64BIT
#define sizetHash    uint64Hash
#define sizetHashCmp uint64HashCmp
#else
#define sizetHash    uintHash
#define sizetHashCmp uintHashCmp
#endif

inline ULONG floatHash(const float &f) {
  return (ULONG)f;
}

inline int floatHashCmp(const float &f1, const float &f2 ) {
  return sign(f1 - f2);
}

inline ULONG doubleHash(const double &d) {
  const ULONG *p = (ULONG*)(&d);
  return p[0] ^ p[1];
}

inline int doubleHashCmp(const double &d1, const double &d2 ) {
  return sign(d1 - d2);
}

template<class T> ULONG pointerHash(const T * const &p) {
  return sizetHash((size_t)p);
}

template<class T> int pointerHashCmp(const T * const &p1, const T * const &p2) {
  return sizetHashCmp((size_t)p1, size_t(p2));
}

#include "FileNameSplitter.h"
