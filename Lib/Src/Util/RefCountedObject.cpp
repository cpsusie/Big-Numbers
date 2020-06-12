#include "pch.h"
#include <RefCountedObject.h>

#if defined(_DEBUG)

#include <DebugLog.h>

void RefCountedObject::logCreate() {
  debugLog(_T("REFCNT:create:%p:refCount=%d\n"), this, m_refCount);
}

void RefCountedObject::logDestroy() {
  debugLog(_T("REFCNT:destroy:%p:refCount=%d\n"), this, m_refCount);
}

void RefCountedObject::logRefCount() {
  debugLog(_T("REFCNT:change:%p:refCount=%d\n"), this, m_refCount);
}
#endif // _DEBUG
