#pragma once

//#define TRACE_MEMORY

#ifdef TRACE_MEMORY

#include "DebugLog.h"

void  *xmalloc(          size_t tsize, int count, const char *method, int line, const char *type);
void  *xrealloc(void *q, size_t tsize, int count, const char *method, int line, const char *type);
char  *xstrdup( const char *str                 , const char *method, int line);
void   xfree(   void *p                         , const char *method, int line);

#define MALLOC(type, n)     (type*)xmalloc(    sizeof(type),n, __FUNCTION__,__LINE__,#type)
#define REALLOC(p, type, n) (type*)xrealloc(p, sizeof(type),n, __FUNCTION__,__LINE__,#type)
#define STRDUP(p)           xstrdup(        p                , __FUNCTION__,__LINE__      )
#define FREE(p)             xfree(          p                , __FUNCTION__,__LINE__      )

inline int _getrc(IUnknown *p) {
  p->AddRef(); return p->Release();
}

#define TRACE_NEW(     p)    { if(p) debugLog("NEW:%p:%s(%d):%s\n"               ,(void*)(p)          ,__FUNCTION__,__LINE__,#p); }
#define TRACE_DELETE(  p)    debugLog("DELETE:%p:%s(%d):%s\n"                    ,(void*)(p)          ,__FUNCTION__,__LINE__,#p)
#define TRACE_CREATE(  p)    debugLog("REFCNT:create:%p:refCount=%d:%s(%d):%s\n" ,(void*)(p),_getrc(p),__FUNCTION__,__LINE__,#p)
#define TRACE_ADDREF(  p, n) debugLog("REFCNT:addref:%p:refCount=%d:%s(%d):%s\n" ,(void*)(p),n        ,__FUNCTION__,__LINE__,#p)
#define TRACE_RELEASE( p, n) debugLog("REFCNT:release:%p:refCount=%d:%s(%d):%s\n",(void*)(p),n        ,__FUNCTION__,__LINE__,#p)
#define TRACE_REFCOUNT(p)    debugLog("REFCNT:trace:%p:refCount=%d:%s(%d):%s\n"  ,(void*)(p),_getrc(p),__FUNCTION__,__LINE__,#p)
#else

void  *xmalloc(size_t n);
void  *xrealloc(void *q, size_t n);
char  *xstrdup(const char *str);
void   xfree(void *p);

#define MALLOC(type, n)     (type*)xmalloc(sizeof(type)*(n))
#define REALLOC(p, type, n) (type*)xrealloc(p, sizeof(type)*(n))
#define STRDUP(p)           xstrdup(p)
#define FREE(p)             xfree(p)

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
