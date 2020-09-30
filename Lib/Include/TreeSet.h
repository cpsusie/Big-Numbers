#pragma once

#include "CompactStack.h"
#include "CommonHashFunctions.h"
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
  friend class TreeSetIterator;
private:
  AbstractObjectManager *m_objectManager;
  AbstractComparator    *m_comparator;
  TreeSetNode           *m_root;
  TreeSetNode           *m_deleteHelper;
  size_t                 m_updateCount;
  size_t                 m_size;

  bool        nodeInsert(TreeSetNode *&pp, TreeSetNode *n);
  bool        balanceL(  TreeSetNode *&pp);
  bool        balanceR(  TreeSetNode *&pp);
  bool        nodeDel(   TreeSetNode *&pp);
  bool        nodeDelete(TreeSetNode *&pp, const void *key);
  void        deleteNodeRecurse(TreeSetNode *n);
  const void *getRandom(RandomGenerator &rnd)                    const;
  void        throwEmptySetException(const TCHAR *method)        const;
protected:
  virtual TreeSetNode         *allocateNode()                    const;
  virtual TreeSetNode         *createNode( const void  *key    ) const;
  virtual TreeSetNode         *cloneNode(  TreeSetNode *n      ) const;
  virtual void                 deleteNode( TreeSetNode *n      ) const;
  virtual bool                 insertNode( TreeSetNode *n      );
  virtual void                 swapContent(TreeSetNode *n1, TreeSetNode *n2);
  const   void                *select(     RandomGenerator &rnd) const override; // return key*
          void                *select(     RandomGenerator &rnd)       override;

        TreeSetNode           *findNode(   const void  *key    );
  const TreeSetNode           *findNode(   const void  *key    ) const;
  const TreeSetNode           *getMinNode()                      const;
  const TreeSetNode           *getMaxNode()                      const;

  const AbstractObjectManager *getObjectManager()                const {
    return m_objectManager;
  }
  AbstractObjectManager       *getObjectManager() {
    return m_objectManager;
  }
  TreeSetNode                 *getRoot()                         const {
    return m_root;
  }
public:
  TreeSetImpl(const AbstractObjectManager &objectManager, const AbstractComparator &comparator);
  AbstractCollection          *clone(bool cloneData)             const override;
  ~TreeSetImpl()                                                       override;
  void                         clear()                                 override;
  size_t                       size()                            const override {
    return m_size;
  }
  bool                          add(     const void *key)              override;
  bool                          remove(  const void *key)              override;
  bool                          contains(const void *e  )        const override;
  AbstractComparator           *getComparator()                  const override {
    return m_comparator;
  }

  const void                   *getMin()                          const override;
  const void                   *getMax()                          const override;
  AbstractIterator             *getIterator()                     const override;
  bool                          hasOrder()                        const override {
    return true;
  }
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
    return m_stack.isEmpty() ? nullptr : &m_stack.top();
  }
  TreeSetNode      *findFirst();
  void              findPath(const void *key);
  TreeSetNode      *findNext();
  void              checkUpdateCount() const;
protected:
  TreeSetNode      *nextNode();
public:
  TreeSetIterator(TreeSetImpl &set);
  AbstractIterator *clone()                  override;
  bool              hasNext()          const override;
  void             *next()                   override {
    return (void*)(nextNode()->key());
  }
  void              remove()                 override;
};

template <typename T> class TreeSet : public Set<T> {
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
  TreeSet<T> &operator=(const CollectionBase<T> &src) {
    __super::operator=(src);
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
