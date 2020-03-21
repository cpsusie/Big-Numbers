#pragma once

#include <Runnable.h>
#include <Math/BigReal/BigRealResourcePool.h>
#include <Math/BigReal/BigReal.h>
#include "Pow2Cache.h"

class DigitPoolArray : public CompactArray<DigitPool*> {
};

class SubProdRunnable : public Runnable, public BigRealResource {
private:
  friend class BigRealResourcePool;
  const String  &m_name;
  const BigReal *m_x, *m_y, *m_f;
  BigReal       *m_result;
  int            m_level;
  void clear();
public:
  SubProdRunnable(int id, const String &name) : BigRealResource(id), m_name(name)
  {
    clear();
  }
  void setInOut(BigReal &result, const BigReal &x, const BigReal &y, const BigReal &f, int level);
  UINT run();
};

class SubProdRunnableArray : public RunnableArray {
private:
  friend class BigRealResourcePool;
  DigitPoolArray m_digitPoolArray;
  void clear(UINT runnableCount, UINT digitPoolCount);
  void reset();

public:
  ~SubProdRunnableArray(); // declared virtual in CompactArray
  inline SubProdRunnable &getRunnable(UINT index) const {
    return *(SubProdRunnable*)(*this)[index];
  }
  inline DigitPool *getDigitPool(UINT index) const {
    return m_digitPoolArray[index];
  }
  inline UINT getRunnableCount() const {
    return (UINT)size();
  }
  inline UINT getDigitPoolCount() const {
    return m_digitPoolArray.size();
  }
};

class SubProdRunnablePool : public ResourcePoolTemplate<SubProdRunnable> {
protected:
  SubProdRunnable *newResource(UINT id) {
    SubProdRunnable *r = new SubProdRunnable(id, format(_T("%s(%u)"), getTypeName().cstr(), id)); TRACE_NEW(r);
    return r;
  }
public:
  SubProdRunnablePool() : ResourcePoolTemplate<SubProdRunnable>(_T("SubProd")) {
  }
};

class DigitPoolPool : public ResourcePoolTemplate<DigitPool> {
protected:
  DigitPool *newResource(UINT id) {
    DigitPool *dp = new DigitPool(id, format(_T("%s(%u)"), getTypeName().cstr(), id)); TRACE_NEW(dp);
    return dp;
  }
public:
  DigitPoolPool() : ResourcePoolTemplate<DigitPool>(_T("DigitPool")) {
  }
};

class LockedDigitPoolPool : public ResourcePoolTemplate<DigitPoolWithLock> {
protected:
  DigitPoolWithLock *newResource(UINT id) {
    DigitPoolWithLock *dp = new DigitPoolWithLock(id, format(_T("%s(%u)"), getTypeName().cstr(), id)); TRACE_NEW(dp);
    return dp;
  }

public:
  LockedDigitPoolPool() : ResourcePoolTemplate<DigitPoolWithLock>(_T("LockedDigitPool")) {
  }
};
