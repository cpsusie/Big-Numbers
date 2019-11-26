#pragma once

#include <Math/BigReal.h>
#include <Runnable.h>

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

template<class T> class AbstractFilteredIterator : public AbstractIterator {
private:
  CompactArray<T>  &m_a;
  BitSet            m_activeSet;
  Iterator<size_t>  m_it;
public:
  AbstractFilteredIterator(const CompactArray<T> &a, const BitSet &set)
    : m_a((CompactArray<T>&)a)
    , m_activeSet(set)
  {
    m_it = m_activeSet.getIterator();
  }
  AbstractIterator *clone() {
    return new AbstractFilteredIterator(m_a, m_activeSet);
  }

  inline bool hasNext() const {
    return m_it.hasNext();
  }

  void *next() {
    if(m_it.hasNext()) {
      noNextElementError(__TFUNCTION__);
    }
    return &m_a[m_it.next()];
  }

  void remove() {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
};

template <class T> class ResourcePool : public CompactArray<T*> {
private:
  const String      m_typeName;
  CompactStack<int> m_freeId;
protected:
  virtual void allocateNewResources(size_t count) {
    int id = (int)size();
    for(size_t i = 0; i < count; i++, id++) {
      m_freeId.push(id);
      T *r = new T(id, format(_T("Resource %s(%d)"), m_typeName.cstr(), id)); TRACE_NEW(r);
      add(r);
    }
  }
public:
  ResourcePool(const String &typeName) : m_typeName(typeName) {
  }
  virtual ~ResourcePool() {
    deleteAll();
  }
  T *fetchResource() {
    if(m_freeId.isEmpty()) {
      allocateNewResources(5);
    }
    const int index = m_freeId.pop();
    return (*this)[index];
  }

  void releaseResource(const BigRealResource *resource) {
    m_freeId.push(resource->getId());
  }

  void deleteAll() {
    for(size_t i = 0; i < size(); i++) {
      SAFEDELETE((*this)[i]);
    }
    clear();
    m_freeId.clear();
  }
  BitSet getAllocatedIdSet() const {
    if(size() == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      return result.invert();
    }
  }
  BitSet getFreeIdSet() const {
    const int n = m_freeId.getHeight();
    if(n == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      for (int i = 0; i < n; i++) {
        result.add(m_freeId.top(i));
      }
      return result;
    }
  }
  BitSet getActiveIdSet() const {
    return getAllocatedIdSet() - getFreeIdSet();
  }

  Iterator<T*> getAllIterator() const {
    return __super::getIterator();

  }
  Iterator<T*> getActiveIterator() const {
    return Iterator<T*>(new AbstractFilteredIterator<T*>(*this, getActiveIdSet()));
  }

  String toString() const {
    const BitSet allocatedIdSet = getAllocatedIdSet();
    const BitSet freeIdSet = getFreeIdSet();
    return format(_T("Free:%s. In use:%s")
                 ,freeIdSet.toString().cstr()
                 ,(allocatedIdSet - freeIdSet).toString().cstr()
    );
  }
};

class SubProdRunnablePool : public ResourcePool<SubProdRunnable> {
public:
  SubProdRunnablePool() : ResourcePool<SubProdRunnable>(_T("SubProd")) {
  }
};

class DigitPoolPool : public ResourcePool<DigitPool> {
public:
  DigitPoolPool() : ResourcePool<DigitPool>(_T("DigitPool")) {
  }
};

class LockedDigitPoolPool : public ResourcePool<DigitPoolWithLock> {
public:
  LockedDigitPoolPool() : ResourcePool<DigitPoolWithLock>(_T("LockedDigitPool")) {
  }
};
