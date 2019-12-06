#pragma once

#include "CompactStack.h"
#include "AbstractFilterArrayIterator.h"

class IdentifiedResource {
private:
  const UINT m_id;
public:
  IdentifiedResource(UINT id) : m_id(id) {
  }
  inline UINT getResourceId() const {
    return m_id;
  }
};

template<typename T> class ResourcePoolTemplate : public CompactArray<T*> {
private:
  CompactStack<UINT> m_freeId;
  const String       m_typeName;
protected:
  virtual void allocateNewResources(UINT count) {
    UINT id = (UINT)size();
    for(size_t i = 0; i < count; i++, id++) {
      T *r = newResource(id);
      m_freeId.push(id);
      add(r);
    }
  }
  virtual T *newResource(UINT id) = NULL;
public:
  ResourcePoolTemplate(const String &typeName) : m_typeName(typeName) {
  }
  virtual ~ResourcePoolTemplate() {
    deleteAll();
  }
  T *fetchResource() {
    if(m_freeId.isEmpty()) {
      allocateNewResources(5);
    }
    const UINT index = m_freeId.pop();
    return (*this)[index];
  }

  const String &getTypeName() const {
    return m_typeName;
  }
  void releaseResource(T *resource) {
    m_freeId.push(resource->getResourceId());
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
    const UINT n = m_freeId.getHeight();
    if(n == 0) {
      return BitSet(8);
    } else {
      BitSet result(size());
      for(UINT i = 0; i < n; i++) {
        result.add(m_freeId.top(i));
      }
      return result;
    }
  }
  BitSet getActiveIdSet() const {
    return getAllocatedIdSet() - getFreeIdSet();
  }

  Iterator<T*> getIterator(const BitSet *filter = NULL) const {
    return filter
         ? Iterator<T*>(new AbstractFilterArrayIterator<CompactArray<T*> >(*((CompactArray<T*>*)this), *filter))
         : ((CompactArray<T*>*)this)->getIterator();

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
