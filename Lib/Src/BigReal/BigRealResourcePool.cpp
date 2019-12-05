#include "pch.h"
#include <SingletonFactory.h>
#include "BigRealResourcePool.h"
#include <DebugLog.h>

DEFINESINGLETONFACTORY(BigRealResourcePool);

BigRealResourcePool &BigRealResourcePool::getInstance() {
  static BigRealResourcePoolFactory factory;
  return factory.getInstance();
}

BigRealResourcePool::BigRealResourcePool() {
  wait();

  m_subProdPool         = new SubProdRunnablePool; TRACE_NEW(m_subProdPool        );
  m_digitPoolPool       = new DigitPoolPool;       TRACE_NEW(m_digitPoolPool      );
  m_lockedDigitPoolPool = new LockedDigitPoolPool; TRACE_NEW(m_lockedDigitPoolPool);

  DigitPoolArray a;
  for(UINT i = 0; i < 8       ; i++) a.add(m_digitPoolPool->fetchResource());
  for(UINT i = 0; i < a.size(); i++) m_digitPoolPool->releaseResource(a[i]);
  a.clear(-1);
  for(UINT i = 0; i < 1       ; i++) a.add(m_lockedDigitPoolPool->fetchResource());
  for(UINT i = 0; i < a.size(); i++) m_lockedDigitPoolPool->releaseResource((DigitPoolWithLock*)a[i]);
  a.clear(-1);

  notify();
}

BigRealResourcePool::~BigRealResourcePool() {
  wait();
  SAFEDELETE(m_subProdPool       );
  SAFEDELETE(m_digitPoolPool     );
  SAFEDELETE(m_lockedDigitPoolPool);
  notify();
}

void BigRealResourcePool::fetchSubProdRunnableArray(SubProdRunnableArray &a, UINT runnableCount, UINT digitPoolCount) { // static
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  try {
    a.clear(runnableCount, digitPoolCount);
    for(UINT i = 0; i < runnableCount; i++) {
      a.add(instance.m_subProdPool->fetchResource());
    }
    for(UINT i = 0; i < digitPoolCount; i++) {
      a.m_digitPoolArray.add(instance.fetchDPool(false, BR_MUTABLE));
    }
    instance.notify();
  } catch (...) {
    instance.notify();
    throw;
  }
}

void BigRealResourcePool::releaseSubProdRunnableArray(SubProdRunnableArray &a) {
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  try {
    const UINT runnableCount = a.getRunnableCount();
    for(UINT i = 0; i < runnableCount; i++) {
      SubProdRunnable *sp = &a.getRunnable(i);
      sp->clear();
      instance.m_subProdPool->releaseResource(sp);
    }
    const UINT digitPoolCount = a.getDigitPoolCount();
    for(UINT i = 0; i < digitPoolCount; i++) {
      instance.releaseDPool(a.getDigitPool(i));
    }
    a.reset();
    instance.notify();
  } catch (...) {
    instance.notify();
    throw;
  }
}

DigitPool *BigRealResourcePool::fetchDigitPool(bool withLock, BYTE initFlags) { // static
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  try {
    DigitPool *pool = instance.fetchDPool(withLock, initFlags);
    instance.notify();
    return pool;
  } catch (...) {
    instance.notify();
    throw;
  }
}

void BigRealResourcePool::releaseDigitPool(DigitPool *pool) { // static
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  try {
    instance.releaseDPool(pool);
    instance.notify();
  } catch(...) {
    instance.notify();
    throw;
  }
}

void BigRealResourcePool::fetchDigitPoolArray(DigitPoolArray &a, UINT count, bool withLock, BYTE initFlags) { // static
  a.clear(count);
  if(count == 0) return;
  BigRealResourcePool &instance = getInstance();

  instance.wait();
  try {
    for(UINT i = 0; i < count; i++) {
      a.add(instance.fetchDPool(withLock, initFlags));
    }
    instance.notify();
  } catch (...) {
    instance.notify();
    throw;
  }
}

void BigRealResourcePool::releaseDigitPoolArray(DigitPoolArray &a) {  // static
  const UINT count = (UINT)a.size();
  if(count == 0) return;
  BigRealResourcePool &instance = getInstance();

  instance.wait();
  try {
    for(UINT i = 0; i < count; i++) {
      instance.releaseDPool(a[i]);
    }
    a.clear();
    instance.notify();
  } catch (...) {
    instance.notify();
    throw;
  }
}

DigitPool *BigRealResourcePool::fetchDPool(bool withLock, BYTE initFlags) {
  DigitPool *pool = withLock
                  ? m_lockedDigitPoolPool->fetchResource()
                  : m_digitPoolPool->fetchResource();

  assert(pool->continueCalculation());
  pool->setInitFlags(initFlags);
  pool->saveRefCount();
  return pool;
}

void BigRealResourcePool::releaseDPool(DigitPool *pool) {
  const UINT refCount = pool->getRefCount();
  if(refCount != pool->getRefCountOnFetch()) {
    throwException(_T("%s:DigitPool \"%s\" has refCount=%u on release, refCount=%u on fetch")
                  ,__TFUNCTION__
                  ,pool->getName().cstr()
                  ,refCount
                  ,pool->getRefCountOnFetch());
  }
  pool->resetPoolCalculation();

  if(pool->isWithLock()) {
    m_lockedDigitPoolPool->releaseResource((DigitPoolWithLock*)pool);
  } else {
    m_digitPoolPool->releaseResource(pool);
  }
}

void BigRealResourcePool::terminateAllPoolCalculations() { // // static
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  DigitPoolPool &pp = *instance.m_digitPoolPool;
  Iterator<DigitPool*> it = pp.getIterator(&pp.getActiveIdSet());
  while(it.hasNext()) {
    it.next()->terminatePoolCalculation();
  }
  instance.notify();
}

String BigRealResourcePool::toString() { // static
  BigRealResourcePool &instance = getInstance();
  String result;
  instance.wait();

  result = format(_T("SubProds:%s DigitPools:%s LockedDigitPools:%s")
                 ,instance.m_subProdPool->toString().cstr()
                 ,instance.m_digitPoolPool->toString().cstr()
                 ,instance.m_lockedDigitPoolPool->toString().cstr()
                 );
  instance.notify();
  return result;
}
