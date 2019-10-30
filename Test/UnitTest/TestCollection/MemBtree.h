#pragma once

#include <Map.h>

class AbstractFormatter {
public:
  virtual String toString(const void *key) = 0;
};

#define HALFSIZE 9
#define MAXITEMCOUNT (2 * HALFSIZE)

class BTreePage;

class PageWalker {
public:
  virtual void handlePage(const BTreePage &p) = 0;
};

class BTreePageItem : public AbstractKey {
public:
  void      *m_key;
  BTreePage *m_child;

  const void *key() const {
    return m_key;
  }
};

class BTreeMapPageItem : public BTreePageItem, public AbstractEntry {
public:
  void      *m_value;

  const void *key() const {
    return BTreePageItem::key();
  }

  void *value() {
    return m_value;
  }

  const void *value() const {
    return m_value;
  }
};

class BTreePage {
private:
  UINT m_itemCount;
  virtual UINT getItemSize() const = 0;
protected:
  BTreePage *m_child0;
  void throwIndexError(const TCHAR *method, UINT i) const;
public:
  virtual void setItem(UINT i, const BTreePageItem &v) = 0;
  virtual const BTreePageItem &getItem(UINT i) const = 0;
  virtual BTreePageItem &getItem(UINT i) = 0;
  const BTreePageItem &getLastItem() const {
    return getItem(m_itemCount);
  }
  void copyItems(UINT from, UINT to, const BTreePage *src, UINT start);
  UINT getItemCount() const {
    return m_itemCount;
  }
  void *getKey(UINT i) const {
    return getItem(i).m_key;
  }
  void setItemCount(UINT value);
  void incrItemCount();
  void decrItemCount();
  void insertItem(UINT i, const BTreePageItem &t);
  void removeItem(UINT i);
  BTreePage *getChild(UINT i) const;
  void setChild(UINT i, BTreePage *v);
};

class BTreeSetPageImpl : public BTreePage {
private:
  BTreePageItem m_item[MAXITEMCOUNT];
  UINT getItemSize() const {
    return sizeof(BTreePageItem);
  }
public:
  void setItem(UINT i, const BTreePageItem &v);
  const BTreePageItem &getItem(UINT i) const;
  BTreePageItem &getItem(UINT i);
};

class BTreeMapPageImpl : public BTreePage {
private:
  BTreeMapPageItem m_item[MAXITEMCOUNT];
  UINT getItemSize() const {
    return sizeof(BTreeMapPageItem);
  }
public:
  void setItem(UINT i, const BTreePageItem &v);
  const BTreePageItem &getItem(UINT ki) const;
  BTreePageItem &getItem(UINT i);
};

class BTreeSetImpl : public AbstractSet {
private:
  AbstractObjectManager *m_objectManager;
  AbstractComparator    *m_comparator;
  BTreePage             *m_root;
  size_t                 m_updateCount;
  size_t                 m_size;
  Array<BTreePageItem>   m_passupItems;

  BTreePageItem *pageSearch(const void *key) const;
  bool pageInsert(          BTreePage *a, const BTreePageItem &item, bool &h, BTreePageItem &passup);
  bool pageRemove(          BTreePage *a, const void *key, bool &h);
  void pageUnderflow(       BTreePage *c, BTreePage *a, int s, bool &h);
  void pageDel(             BTreePage *p, BTreePage *a, int r, bool &h);
  void showPage(  const     BTreePage *p, int level, AbstractFormatter &formatter) const;
  void checkPage( const     BTreePage *p, int level, int height) const;
  UINT getHeight() const;
  void deletePage(BTreePage *a);
  void traverse(const BTreePage *p, PageWalker &pw) const;
protected:
  virtual BTreePage     *allocatePage() const;
  virtual BTreePageItem &allocatePassupItem();
  virtual void clearPassupItems();
  virtual void copyItem(BTreePageItem &dst, const BTreePageItem &src) const;
  virtual void deleteItem(BTreePageItem &item) const;
  const BTreePage *getRoot() const {
    return m_root;
  }

  AbstractObjectManager *getObjectManager() {
    return m_objectManager;
  }
  const AbstractObjectManager *getObjectManager() const {
    return m_objectManager;
  }

  BTreePageItem *findNode(const void *key) ;
  const BTreePageItem *getMinNode() const;
  const BTreePageItem *getMaxNode() const;

  const BTreePageItem *findNode(const void *key) const;
  bool insertItem(BTreePageItem &item);
public:
  BTreeSetImpl(const AbstractObjectManager &objectManager, const AbstractComparator &comparator);
  virtual ~BTreeSetImpl();
  bool add(      const void *key);
  bool remove(   const void *key);
  bool contains( const void *key) const;
  const void *select(RandomGenerator &rnd) const; // returns key*
  void *select(RandomGenerator &rnd);
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
  friend class BTreeSetIterator;

  void traverse(PageWalker &pw) const;
  void showTree(AbstractFormatter &formatter) const;
  void checkTree();
};

class BTreeMapImpl : public BTreeSetImpl, public AbstractMap {
private:
  AbstractObjectManager  *m_dataManager;
  Array<BTreeMapPageItem> m_passupItems;
  void showPage(const BTreePage *p, int level, AbstractFormatter &keyFormatter, AbstractFormatter &dataFormatter) const;
protected:
  BTreePage     *allocatePage() const;
  BTreePageItem &allocatePassupItem();
  void clearPassupItems();
  void copyItem(BTreePageItem &dst, const BTreePageItem &src) const;
  void deleteItem(BTreePageItem &item) const;

public:
  BTreeMapImpl(const AbstractObjectManager &keyManager, const AbstractObjectManager &dataManager, const AbstractComparator &comparator);
  ~BTreeMapImpl();
  bool put(const void *key,       void *elem);
  bool put(const void *key, const void *elem);
  bool remove(const void *key);
        void *get(const void *key);
  const void *get(const void *key) const;
  AbstractEntry *selectEntry(RandomGenerator &rnd) const;
  const AbstractEntry *getMinEntry() const;
  const AbstractEntry *getMaxEntry() const;
  size_t size() const {
    return BTreeSetImpl::size();
  }

  void clear() {
    BTreeSetImpl::clear();
  }

  bool hasOrder() const {
    return BTreeSetImpl::hasOrder();
  }

  AbstractComparator *getComparator() {
    return BTreeSetImpl::getComparator();
  }

  const AbstractComparator *getComparator() const {
    return BTreeSetImpl::getComparator();
  }

  AbstractSet *cloneSet(bool cloneData) {
    throwUnsupportedOperationException(__TFUNCTION__);
    return NULL;
  }

  AbstractMap *cloneMap(bool cloneData) const;

  AbstractIterator *getIterator();

  AbstractIterator *getKeyIterator() {
    return BTreeSetImpl::getIterator();
  }

  void showTree(AbstractFormatter &keyFormatter, AbstractFormatter &dataFormatter) const;
};

// --------------------------------------------------------------------------------

template <class K> class Formatter : public AbstractFormatter {
  String toString(const void *key) {
    return toString(*(K*)key);
  }
public:
  virtual String toString(const K &key) = 0;
};

template <class K> class BTreeSet : public Set<K> {
public:
  BTreeSet(int (*cmp)(const K &key1, const K &key2))
    : Set<K>(new BTreeSetImpl(ObjectManager<K>(), FunctionComparator<K>(cmp)))
  {
  }
  BTreeSet(int (*cmp)(const K *key1, const K *key2))
    : Set<K>(new BTreeSetImpl(ObjectManager<K>(), FunctionComparator<K>(cmp)))
  {
  }
  BTreeSet(Comparator<K> &cmp)
    : Set<K>(new BTreeSetImpl(ObjectManager<K>(), cmp))
  {
  }

  void traverse(PageWalker &pw) const {
    ((BTreeSetImpl*)m_set)->traverse(pw);
  }
  void checkTree() const {
    ((BTreeSetImpl*)m_set)->checkTree();
  }
  void showTree(Formatter<K> &formatter) const {
    ((BTreeSetImpl*)m_set)->showTree(formatter);
  }
};

template <class K, class E> class BTreeMap : public Map<K,E> {
public:
  BTreeMap(int (*comparator)(const K &key1, const K &key2))
    : Map<K,E>(new BTreeMapImpl(ObjectManager<K>(), ObjectManager<E>(), FunctionComparator<K>(comparator)))
  {
  }
  BTreeMap(int (*comparator)(const K *key1, const K *key2))
    : Map<K,E>(new BTreeMapImpl(ObjectManager<K>(), ObjectManager<E>(), FunctionComparator<K>(comparator)))
  {
  }
  BTreeMap(Comparator<K> &comparator)
    : Map<K,E>(new BTreeMapImpl(ObjectManager<K>(), ObjectManager<E>(), comparator))
  {
  }
  void traverse(PageWalker &pw) const {
    ((BTreeMapImpl*)m_map)->traverse(pw);
  }
  void checkTree() const {
    ((BTreeMapImpl*)m_map)->checkTree();
  }
  void showTree(Formatter<K> &keyFormatter, Formatter<E> &dataFormatter) const {
    ((BTreeMapImpl*)m_map)->showTree(keyFormatter,dataFormatter);
  }
};
