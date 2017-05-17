#pragma once

class RefCountedObject {
private:
  int m_refCount;
#ifdef _DEBUG
  void logCreate();
  void logRefCount();
  void logDestroy();
#define RCO_LOGCREATE()   logCreate()
#define RCO_LOGREFCOUNT() logRefCount()
#define RCO_LOGDESTROY()  logDestroy()
#else
#define RCO_LOGCREATE()
#define RCO_LOGREFCOUNT()
#define RCO_LOGDESTROY()
#endif

public:
  RefCountedObject() : m_refCount(1) {
    RCO_LOGCREATE();
  }
  virtual ~RefCountedObject() {
    RCO_LOGDESTROY();
  }
  inline int  addref()  { m_refCount++; RCO_LOGREFCOUNT(); return m_refCount; }
  inline int  release() { m_refCount--; RCO_LOGREFCOUNT(); return m_refCount; }
};
