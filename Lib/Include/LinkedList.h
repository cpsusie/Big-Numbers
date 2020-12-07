#pragma once

#include "Collection.h"
#include "Comparator.h"
#include "HeapObjectPool.h"
#include "ObjectManager.h"

class ListNode {
public:
  void     *m_data;
  ListNode *m_prev, *m_next;
};

class ListImpl : public AbstractCollection {
private:
  AbstractObjectManager    *m_objectManager;
  HeapObjectPool<ListNode>  m_nodePool;
  ListNode                 *m_first, *m_last;
  size_t                    m_size;
  size_t                    m_updateCount;

  void            init(AbstractObjectManager &objectManager);
  ListNode       *createNode(const void *e);
  void            deleteNode(ListNode *n);
  // assume index < size()
  const ListNode *findNode(size_t index) const;
  void            removeNode(ListNode *n);
  void            indexError( const TCHAR *method, size_t index) const;
  void            removeError(const TCHAR *method) const;
  void            getError(   const TCHAR *method) const;
  friend class ListIterator;
public:
  ListImpl(AbstractObjectManager &objectManager);
  AbstractCollection *clone(bool cloneData)          const override;
  ~ListImpl()                                              override;
  size_t              size()                         const override {
    return m_size;
  }
  void                clear()                              override;
  bool                add(                  const void *e) override;
  bool                insert(     size_t i, const void *e);
  void                removeIndex(size_t i);
  bool                remove(  const void *e)              override;
  // unsupported
  bool                contains(const void *e)        const override;
  void                removeFirst();
  void                removeLast();
  void               *select(RandomGenerator &rnd)   const override;
  void               *getElement(size_t index)       const;
  void               *first()                        const;
  void               *last()                         const;

  AbstractIterator   *getIterator()                  const override;
  bool                hasOrder()                     const override {
    return true;
  }
};

template<typename T> class LinkedList : public Collection<T> {
public:
  LinkedList() : Collection<T>(new ListImpl(ObjectManager<T>())) {
  }

  LinkedList(const CollectionBase<T> &src) : Collection<T>(new ListImpl(ObjectManager<T>())) {
    addAll(src);
  }

  LinkedList<T> &operator=(const CollectionBase<T> &src) {
    __super::operator=(src);
    return *this;
  }

  inline T &operator[](size_t i) {
    return *(T*)(((ListImpl*)m_collection)->getElement(i));
  }

  inline const T &operator[](size_t i) const {
    return *(T*)(((ListImpl*)m_collection)->getElement(i));
  }

  inline T &first() {
    return *(T*)(((ListImpl*)m_collection)->first());
  }

  inline const T &first() const {
    return *(T*)(((ListImpl*)m_collection)->first());
  }

  inline T &last() {
    return *(T*)(((ListImpl*)m_collection)->last());
  }

  inline const T &last() const {
    return *(T*)(((ListImpl*)m_collection)->last());
  }

  bool operator==(const LinkedList<T> &rhs) const { // NB not virtual in Collection, because of ==
    if(size() != rhs.size()) {
      return false;
    }
    ConstIterator<T> it1 = getIterator();
    ConstIterator<T> it2 = rhs.getIterator();
    while(it1.hasNext()) {
      if(!(it1.next() == it2.next())) {
        return false;
      }
    }
    return true;
  }

  bool operator!=(const LinkedList<T> &rhs) const {
    return !(operator==(rhs));
  }

  bool insert(size_t i, const T &e) {
    return ((ListImpl*)m_collection)->insert(i, &e);
  }

  bool add(const T &e) override {
    return m_collection->add(&e);
  }

  bool remove(const T &e) {                    // NB not virtual in Collection, because of ==
    for(auto it = getIterator(); it.hasNext();) {
      if(it.next() == e) {
        it.remove();
        return true;
      }
    }
    return false;
  }


  bool removeIndex(size_t i) {
    ((ListImpl*)m_collection)->removeIndex(i);
    return true;
  }

  void removeFirst() {
    ((ListImpl*)m_collection)->removeFirst();
  }

  void removeLast() {
    ((ListImpl*)m_collection)->removeLast();
  }

  bool contains(const T &e) const {            // NB not virtual in Collection, because of ==
    for(auto it = getIterator(); it.hasNext();) {
      if(it.next() == e) {
        return true;
      }
    }
    return false;
  }
};

class StrList : public LinkedList<const TCHAR*> {
};

class StringList : public LinkedList<String> {
};

class CharList : public LinkedList<char> {
};

class ShortList : public LinkedList<short> {
};

class IntList : public LinkedList<int> {
};

class LongList : public LinkedList<long> {
};

class FloatList : public LinkedList<float> {
};

class DoubleList : public LinkedList<double> {
};
