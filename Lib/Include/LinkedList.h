#pragma once

#include "MyString.h"
#include "Collection.h"
#include "Comparator.h"
#include "ObjectManager.h"

class ListNode {
public:
  void     *m_data;
  ListNode *m_prev, *m_next;
};

#define LISTNODEPAGESIZE 10000

class ListNodePage {
private:
  friend class ListImpl;
  ListNode      m_nodeArray[LISTNODEPAGESIZE];
  ListNodePage *m_nextPage;
  ListNodePage(ListNodePage *nextPage);
};

class ListImpl : public AbstractCollection {
private:
  DECLARECLASSNAME;
  AbstractObjectManager *m_objectManager;
  ListNodePage          *m_firstPage;
  ListNode              *m_freeList;
  ListNode              *m_first, *m_last;
  unsigned int           m_size;
  unsigned long          m_updateCount;

  inline void allocatePage() { // m_freeList is always NULL when this is called
    m_firstPage = new ListNodePage(m_firstPage);
    m_freeList  = m_firstPage->m_nodeArray;
  }
  void releaseAllPages();
  inline ListNode *fetchNode() {
    if(m_freeList == NULL) allocatePage();
    ListNode *p = m_freeList;
    m_freeList  = p->m_next;
    return p;
  }
  inline void releaseNode(ListNode *node) {
    node->m_next = m_freeList;
    m_freeList   = node;
  }

  void init(AbstractObjectManager &objectManager);
  ListNode *createNode(const void *e);
  void deleteNode(ListNode *n);
  const ListNode *findNode(unsigned int index) const;
  void removeNode(ListNode *n);
  void throwOutOfRangeException(const TCHAR *method, unsigned int index) const;
  friend class ListIterator;
public:
  ListImpl(AbstractObjectManager &objectManager);
  ~ListImpl();
  AbstractCollection *clone(bool cloneData) const;
  ListImpl &operator=(const ListImpl &src);
  bool add(const void *e);
  bool add(unsigned int i, const void *e);
  void removeIndex(unsigned int i);
  bool remove(const void *e);
  void removeFirst();
  void removeLast();
  bool contains(const void *e) const; // unsupported
  const void *select() const ;
  void *select();
  void clear();
  int size() const {
    return m_size;
  }
        void *getElement(unsigned int index);
  const void *getElement(unsigned int index) const;
        void *first();
  const void *first() const;
        void *last();
  const void *last() const;

  AbstractIterator *getIterator();
};

template <class T> class LinkedList : public Collection<T> {
public:
  LinkedList() : Collection<T>(new ListImpl(ObjectManager<T>())) {
  }

  LinkedList(const Collection<T> &src) : Collection<T>(new ListImpl(ObjectManager<T>())) {
    addAll(src);
  }

  LinkedList<T> &operator=(const Collection<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }

  inline T &operator[](unsigned int i) {
    return *(T*)(((ListImpl*)m_collection)->getElement(i));
  }

  inline const T &operator[](unsigned int i) const {
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

  inline T &select() {
    return *(T*)m_collection->select();
  }

  bool operator==(const LinkedList<T> &rhs) const { // NB not virtual in Collection, because of ==
    if(m_collection->size() != rhs.m_collection->size()) {
      return false;
    }
    Iterator<T> it1 = ((LinkedList<T>*)this)->getIterator();
    Iterator<T> it2 = ((LinkedList<T>&)rhs).getIterator();
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

  bool add(unsigned int i, const T &e) {
    return ((ListImpl*)m_collection)->add(i, &e);
  }

  bool add(const T &e) {
    return m_collection->add(&e);
  }

  bool remove(const T &e) {                    // NB not virtual in Collection, because of ==
    for(Iterator<T> it = getIterator(); it.hasNext();) {
      if(it.next() == e) {
        it.remove();
        return true;
      }
    }
    return false;
  }


  bool removeIndex(unsigned int i) {
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
    for(Iterator<T> it = ((LinkedList<T>*)this)->getIterator(); it.hasNext();) {
      if(it.next() == e) {
        return true;
      }
    }
    return false;
  }
};

class StrList : public LinkedList<const char*> {
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
