#include "pch.h"
#include <SingletonFactory.h>
#include "BigRealResourcePool.h"
#include "Pow2Cache.h"
#include <DebugLog.h>

// The order of declaration is importatnt here.!

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitBigReal {
private:
  BigRealResourcePool *dummy;
public:
  InitBigReal() {
    dummy = &BigRealResourcePool::getInstance();
  }
  ~InitBigReal() {
    BigRealResourcePool::releaseInstance();
  }
};

const BR2DigitType BigReal::s_BIGREALBASEBR2(BIGREALBASE);

std::atomic<UINT> DigitPool::s_totalAllocatedPageCount;
#ifndef COUNT_DIGITPOOLFETCHDIGIT
bool              DigitPool::s_dumpCountWhenDestroyed  = false;
#else
bool              DigitPool::s_dumpCountWhenDestroyed  = true;
#endif // COUNT_DIGITPOOLFETCHDIGIT
DigitPool        *BigReal::s_defaultDigitPool          = NULL;
DigitPool        *BigReal::s_constDigitPool            = NULL;

static InitBigReal initBigReal;

const BigInt  &BigReal::_0 = DEFAULT_DIGITPOOL->_0();
const BigInt  &BigReal::_1 = DEFAULT_DIGITPOOL->_1();
const BigInt  &BigReal::_2 = DEFAULT_DIGITPOOL->_2();
const BigInt  BigReal::_i16_min(  _I16_MIN  );
const BigInt  BigReal::_i16_max(  _I16_MAX  );
const BigInt  BigReal::_ui16_max( _UI16_MAX );
const BigInt  BigReal::_i32_min(  _I32_MIN  );
const BigInt  BigReal::_i32_max(  _I32_MAX  );
const BigInt  BigReal::_ui32_max( _UI32_MAX );
const BigInt  BigReal::_i64_min(  _I64_MIN  );
const BigInt  BigReal::_i64_max(  _I64_MAX  );
const BigInt  BigReal::_ui64_max( _UI64_MAX );
const BigInt  BigReal::_i128_min( _I128_MIN );
const BigInt  BigReal::_i128_max( _I128_MAX );
const BigInt  BigReal::_ui128_max(_UI128_MAX);

const BigReal &BigReal::_05 = DEFAULT_DIGITPOOL->_05();
const BigReal BigReal::_flt_min(  FLT_MIN   );
const BigReal BigReal::_flt_max(  FLT_MAX   );
const BigReal BigReal::_dbl_min(  DBL_MIN   );
const BigReal BigReal::_dbl_max(  DBL_MAX   );
const BigReal BigReal::_dbl80_min(DBL80_MIN );
const BigReal BigReal::_dbl80_max(DBL80_MAX );
const BigReal BigReal::_C1third(0.333333f   );

// No need for signalling NaN
const BigReal &BigReal::_BR_QNAN = DEFAULT_DIGITPOOL->nan();
const BigReal &BigReal::_BR_PINF = DEFAULT_DIGITPOOL->pinf();    // +infinity;
const BigReal &BigReal::_BR_NINF = DEFAULT_DIGITPOOL->ninf();    // -infinity;

const BigInt  BigInt::_BINT_QNAN(quot( BigReal::_0, BigReal::_0));  // non-signaling NaN (quiet NaN)
const BigInt  BigInt::_BINT_PINF(quot( BigReal::_1, BigReal::_0));  // +infinity;
const BigInt  BigInt::_BINT_NINF(quot(-BigReal::_1, BigReal::_0));  // -infinity;

const BigRational BigRational::_0(         BigReal::_0, BigReal::_1);  // 0
const BigRational BigRational::_05(        BigReal::_1, BigReal::_2);  // 1/2
const BigRational BigRational::_1(         BigReal::_1, BigReal::_1);  // 1
const BigRational BigRational::_2(         BigReal::_2, BigReal::_1);  // 2
const BigRational BigRational::_BRAT_QNAN( BigReal::_0, BigReal::_0);  // non-signaling NaN (quiet NaN)
const BigRational BigRational::_BRAT_PINF( BigReal::_1, BigReal::_0);  // +infinity;
const BigRational BigRational::_BRAT_NINF(-BigReal::_1, BigReal::_0);  // -infinity;

class DefaultDigitPoolModifier {
public:
  inline DefaultDigitPoolModifier() {
    BigReal::s_defaultDigitPool->setInitFlags(BR_MUTABLE);
  }
};

static DefaultDigitPoolModifier setDefaultDPToMutable;

typedef enum {
  REQUEST_GETINSTANCE
 ,REQUEST_RELEASE
} POOLREQUEST;

DEFINESINGLETONFACTORY(BigRealResourcePool);

BigRealResourcePool *BigRealResourcePool::poolRequeest(int request) { // static
  static BigRealResourcePoolFactory factory;
  switch(request) {
  case REQUEST_GETINSTANCE:
    return &factory.getInstance();
  case REQUEST_RELEASE:
    factory.releaseInstance();
    break;
  }
  return NULL;
}

BigRealResourcePool &BigRealResourcePool::getInstance() { // static
  return *poolRequeest(REQUEST_GETINSTANCE);
}

void BigRealResourcePool::releaseInstance() { // static
  poolRequeest(REQUEST_RELEASE);
}

BigRealResourcePool::BigRealResourcePool() {
  Double80::initClass();
  DigitPool::s_totalAllocatedPageCount = 0;

  m_resourcesInUse      = 0;
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

  BigReal::s_defaultDigitPool = fetchDPool(false, 0); // this will be changed to BR_MUTABLE when all class constants have been defined and initialized
  BigReal::s_constDigitPool   = fetchDPool(true, 0);
  BigReal::s_defaultDigitPool->setName(_T("DEFAULT"));
  BigReal::s_constDigitPool->setName(_T("CONST"));
  Pow2Cache::s_instance       = new Pow2Cache(fetchDPool(false, 0), fetchDPool(false, BR_MUTABLE)); TRACE_NEW(Pow2Cache::s_instance);
}

BigRealResourcePool::~BigRealResourcePool() {
  DigitPool *p2DigitPool = Pow2Cache::s_instance->m_digitPool;
  DigitPool *p2WorkPool  = Pow2Cache::s_instance->m_workPool;

  SAFEDELETE(Pow2Cache::s_instance);

  releaseDPool(p2DigitPool);
  releaseDPool(p2WorkPool );
  releaseDPool(BigReal::s_defaultDigitPool);
  releaseDPool(BigReal::s_constDigitPool  );

  BigReal::s_constDigitPool   = NULL;
  BigReal::s_defaultDigitPool = NULL;

  SAFEDELETE(m_subProdPool       );
  SAFEDELETE(m_digitPoolPool     );
  SAFEDELETE(m_lockedDigitPoolPool);
}

void BigRealResourcePool::fetchSubProdRunnableArray(SubProdRunnableArray &a, UINT runnableCount, UINT digitPoolCount) { // static
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  try {
    a.clear(runnableCount, digitPoolCount);
    for(UINT i = 0; i < runnableCount; i++) {
      a.add(instance.m_subProdPool->fetchResource());
    }
    instance.updateResourcesInUse(runnableCount);
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
    instance.updateResourcesInUse(-((int)(runnableCount)));

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
  updateResourcesInUse(1);
  pool->resetName();
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
  updateResourcesInUse(-1);
}

void BigRealResourcePool::setTerminateAllPoolsInUse(bool terminate) { // static
  BigRealResourcePool &instance = getInstance();
  instance.wait();
  DigitPoolPool &dpp = *instance.m_digitPoolPool;
  Iterator<DigitPool*> it = dpp.getIterator(&dpp.getActiveIdSet());
  while(it.hasNext()) {
    DigitPool *dp = it.next();
    if(terminate) {
      dp->terminatePoolCalculation();
    } else {
      dp->resetPoolCalculation();
    }
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
