#pragma once

#if defined(TRACE_MEMORY)

#include "DebugLog.h"

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
