#include "pch.h"
#include <RefCountedObject.h>

#ifdef _DEBUG
void RefCountedObject::logCreate() {
  debugLog(_T("REFCNT:create:%p:refCount=%d\n"), this, m_refCount);
}

void RefCountedObject::logDestroy() {
  debugLog(_T("REFCNT:destroy:%p:refCount=%d\n"), this, m_refCount);
}

void RefCountedObject::logRefCount() {
  debugLog(_T("REFCNT:change:%p:refCount=%d\n"), this, m_refCount);
}
#endif
