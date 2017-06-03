#pragma once

#include "MyUtil.h"
#include "CompactStack.h"
#include "Set.h"

class TreeSetNode : public AbstractKey {
private:
  void *m_key;
  char  m_balance;
  TreeSetNode *m_left, *m_right;
public:
  const void *key() const {
    return m_key;
  }

  TreeSetNode *left() {
    return m_left;
  }

  TreeSetNode *right() {
    return m_right;
  }

  friend class TreeSetImpl;
};

class TreeSetImpl : public AbstractSet {
private:
  AbstractObjectManager *m_objectManager;
  AbstractComparator    *m_comparator;
  TreeSetNode           *m_root;
  TreeSetNode           *m_deleteHelper;
  size_t                 m_updateCount;
  size_t                 m_size;

  bool nodeInsert(TreeSetNode *&pp, TreeSetNode *n);
  bool balanceL(  TreeSetNode *&pp);
  bool balanceR(  TreeSetNode *&pp);
  bool nodeDel(   TreeSetNode *&pp);
  bool nodeDelete(TreeSetNode *&pp, const void *key);
  void deleteNodeRecurse(TreeSetNode *n);
  const void *getRandom() const;
  friend class TreeSetIterator;
  void throwEmptySetException(const TCHAR *method) const;
protected:
  virtual TreeSetNode *allocateNode() const;
  virtual TreeSetNode *createNode(const void *key) const;
  virtual TreeSetNode *cloneNode(TreeSetNode *n) const;
  virtual void deleteNode(TreeSetNode *n) const;
  virtual bool insertNode(TreeSetNode *n);
  const void *select() const; // return key*
  void *select();
  virtual void swapContent(TreeSetNode *n1, TreeSetNode *n2);

  TreeSetNode *findNode(const void *key);

  const TreeSetNode *findNode(const void *key) const;
  const TreeSetNode *getMinNode() const;
  const TreeSetNode *getMaxNode() const;

  AbstractObjectManager *getObjectManager() {
    return m_objectManager;
  }

  const AbstractObjectManager *getObjectManager() const {
    return m_objectManager;
  }

  TreeSetNode *getRoot() {
    return m_root;
  }
public:
  TreeSetImpl(const AbstractObjectManager &objectManager, const AbstractComparator &comparator);
  virtual ~TreeSetImpl();
  bool add(const void *key);
  bool remove(const void *key);
  bool contains(const void *e) const;
  size_t size() const {
    return m_size;
  }
  void clear();
  bool hasOrder() const {
    return true;
  }
  AbstractComparator *getComparator() {
    return m_comparator;
  }

  const AbstractComparator *getComparator() const {
    return m_comparator;
  }

  const void *getMin() const;
  const void *getMax() const;
  AbstractCollection *clone(bool cloneData) const;
  AbstractIterator *getIterator();
};

class TreeSetIteratorStackElement {
public:
  TreeSetNode *m_node;
  char         m_state;

  inline TreeSetIteratorStackElement(TreeSetNode *node, char state) {
    m_node  = node;
    m_state = state;
  }

  inline TreeSetIteratorStackElement() {
  }
};

class TreeSetIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  TreeSetImpl                              &m_set;
  CompactStack<TreeSetIteratorStackElement> m_stack;
  TreeSetNode                              *m_next, *m_current;
  size_t                                    m_updateCount;

  inline void push(TreeSetNode *node, char state) {
    m_stack.push(TreeSetIteratorStackElement(node,state));
  }
  inline void pop() {
    m_stack.pop();
  }
  inline TreeSetIteratorStackElement *top() {
    return m_stack.isEmpty() ? NULL : &m_stack.top();
  }
  TreeSetNode *findFirst();
  void findPath(const void *key);
  TreeSetNode *findNext();
  void checkUpdateCount() const;
protected:
  TreeSetNode *nextNode();
public:
  TreeSetIterator(TreeSetImpl &set);
  AbstractIterator *clone();
  virtual void *next() {
    return (void*)(nextNode()->key());
  }
  bool hasNext() const;
  void remove();
};

template <class T> class TreeSet : public Set<T> {
public:
  TreeSet(int (*comparator)(const T &key1, const T &key2))
    : Set<T>(new TreeSetImpl(ObjectManager<T>(), FunctionComparator<T>(comparator)))
  {
  }
  TreeSet(int (*comparator)(const T *key1, const T *key2))
    : Set<T>(new TreeSetImpl(ObjectManager<T>(), FunctionComparator<T>(comparator)))
  {
  }
  TreeSet(Comparator<T> &comparator) 
    : Set<T>(new TreeSetImpl(ObjectManager<T>(), comparator))
  {
  }
  TreeSet<T> &operator=(const Collection<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
  }
};

class ShortTreeSet : public TreeSet<short>  {
public:
  ShortTreeSet() : TreeSet<short>(shortHashCmp) {
  }
};

class UShortTreeSet : public TreeSet<USHORT>  {
public:
  UShortTreeSet() : TreeSet<USHORT>(ushortHashCmp) {
  }
};

class IntTreeSet : public TreeSet<int> {
public:
  IntTreeSet() : TreeSet<int>(intHashCmp) {
  }
};

class UIntTreeSet : public TreeSet<UINT> {
public:
  UIntTreeSet() : TreeSet<UINT>(uintHashCmp) {
  }
};

class LongTreeSet : public TreeSet<long> {
public:
  LongTreeSet() : TreeSet<long>(longHashCmp) {
  }
};

class ULongTreeSet : public TreeSet<ULONG> {
public:
  ULongTreeSet() : TreeSet<ULONG>(ulongHashCmp) {
  }
};

class Int64TreeSet : public TreeSet<INT64> {
public:
  Int64TreeSet() : TreeSet<INT64>(int64HashCmp) {
  }
};

class UInt64TreeSet : public TreeSet<UINT64> {
public:
  UInt64TreeSet() : TreeSet<UINT64>(uint64HashCmp) {
  }
};

class FloatTreeSet : public TreeSet<float> {
public:
  FloatTreeSet()  : TreeSet<float>(floatHashCmp) {
  }
};

class DoubleTreeSet : public TreeSet<double> {
public:
  DoubleTreeSet()  : TreeSet<double>(doubleHashCmp) {
  }
};

class StrTreeSet : public TreeSet<const TCHAR*> {
public:
  StrTreeSet() : TreeSet<const TCHAR*>(strHashCmp) {
  }
};

class StrITreeSet : public TreeSet<const TCHAR*> {
public:
  StrITreeSet() : TreeSet<const TCHAR*>(striHashCmp) {
  }
};

class StringTreeSet : public TreeSet<String> {
public:
  StringTreeSet() : TreeSet<String>(stringHashCmp) {
  }
};

class StringITreeSet : public TreeSet<String> {
public:
  StringITreeSet() : TreeSet<String>(stringiHashCmp) {
  }
};
