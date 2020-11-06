#pragma once

#include <Singleton.h>
#include <Semaphore.h>
#include <Math/BigReal/BigRealConfig.h>

class SubProdRunnableArray;
class DigitPool;
class DigitPoolArray;
class DigitPoolPool;
class LockedDigitPoolPool;
class SubProdRunnablePool;
class Pow2Cache;

class BigRealResourcePool : public Singleton {
private:
  DigitPoolPool        *m_digitPoolPool;
  LockedDigitPoolPool  *m_lockedDigitPoolPool;
  SubProdRunnablePool  *m_subProdPool;
  DigitPool            *m_defaultDigitPool, *m_constDigitPool;
  Pow2Cache            *m_pow2Cache;
  UINT                  m_resourcesInUse;
  mutable Semaphore     m_lock;

  inline BigRealResourcePool &wait() {
    m_lock.wait();
    return *this;
  }
  inline void notify() const {
    m_lock.notify();
  }
  inline void updateResourcesInUse(int count) {
    m_resourcesInUse += count;
  }
  // No lock-protection
  // Do the real fetch
  DigitPool *fetchDPool(bool withLock, BYTE initFlags);
  // No lock-protection
  // Do the real release
  void       releaseDPool(DigitPool *pool);
  static void setTerminateAllPoolsInUse(bool terminate);
  // No lock-protection
  void allocatePow2Cache();
  // No lock-protection
  void deallocatePow2Cache();
  BigRealResourcePool();
  ~BigRealResourcePool() override;
public:
  static void                 fetchSubProdRunnableArray(  SubProdRunnableArray &a, UINT runnableCount, UINT digitPoolCount);
  static void                 releaseSubProdRunnableArray(SubProdRunnableArray &a);
  static DigitPool           *fetchDigitPool(             bool withLock = false, BYTE initFlags = BR_MUTABLE);
  static void                 releaseDigitPool(           DigitPool      *pool);
  static void                 fetchDigitPoolArray(        DigitPoolArray &a, UINT count, bool withLock = false, BYTE initFlags = BR_MUTABLE);
  static void                 releaseDigitPoolArray(      DigitPoolArray &a);
  static DigitPool           *getDefaultDigitPool();
  static DigitPool           *getConstDigitPool();
  static Pow2Cache           *getPow2Cache();
  // Call terminatePoolCalculation() for all DigitPools in use
  static inline void          terminateAllPoolCalculations() {
    setTerminateAllPoolsInUse(true);
  }
  // Call resetPoolCalculation() for all DigitPools in use
  static inline void           resetAllPoolCalculations() {
    setTerminateAllPoolsInUse(false);
  }

  static String toString(); // for debug
  // Sets the priority for all running and future running threads
  // Default is THREAD_PRIORITY_BELOW_NORMAL
  // THREAD_PRIORITY_IDLE,-PRIORITY_LOWEST,-PRIORITY_BELOW_NORMAL,-PRIORITY_NORMAL,-PRIORITY_ABOVE_NORMAL

  DEFINESINGLETON(BigRealResourcePool)
};
