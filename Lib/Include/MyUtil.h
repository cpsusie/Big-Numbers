#pragma once

#include "BasicIncludes.h"
#include "Comparator.h"

void  *xmalloc( UINT n);
void  *xrealloc(void *q, UINT n);
TCHAR *xstrdup( const TCHAR *str);
void  xfree(void *p);

#define MALLOC(type, n)     (type*)xmalloc(sizeof(type)*(n))
#define REALLOC(p, type, n) (type*)xrealloc(p, sizeof(type)*(n))
#define STRDUP(p)           xstrdup(p)
#define FREE(p)             xfree(p)

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
// read password   from console
String  inputPassword(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);

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

extern int UseSafeNew; // assign something to this, and we use safe new_handler, which throws Exception on out of memory

TCHAR *newGUID(TCHAR *dst);
TCHAR *sprintGUID(TCHAR *dst, const GUID &guid);
GUID  *newGUID(GUID *guid);
