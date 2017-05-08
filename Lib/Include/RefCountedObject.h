#pragma once

class RefCountedObject {
private:
  int m_refCount;
#ifdef _DEBUG
  void logCreate();
  void logRefCount();
  void logDestroy();
#define LOGCREATE()   logCreate()
#define LOGREFCOUNT() logRefCount()
#define LOGDESTROY()  logDestroy()
#else
#define LOGCREATE()
#define LOGREFCOUNT()
#define LOGDESTROY()
#endif

public:
  RefCountedObject() : m_refCount(1) {
    LOGCREATE();
  }
#ifdef _DEBUG
  ~RefCountedObject() {
    logDestroy();
  }
#endif
  inline int  addref()  { m_refCount++; LOGREFCOUNT(); return m_refCount; }
  inline int  release() { m_refCount--; LOGREFCOUNT(); return m_refCount; }
};
