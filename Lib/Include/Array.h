#pragma once

#include "Collection.h"
#include "Comparator.h"
#include "ObjectManager.h"

class ArrayImpl : public AbstractCollection {
  friend class ArrayIterator;
private:
  AbstractObjectManager *m_objectManager;
  void                 **m_elem;
  size_t                 m_size;
  size_t                 m_capacity;
  size_t                 m_updateCount;

  void        resize();
  void        init(const AbstractObjectManager &objectManager, size_t size, size_t capacity);
  intptr_t    getSortCount(intptr_t from, intptr_t count) const;
  void        sort(                         size_t   from, size_t count, AbstractComparator &comparator);
  intptr_t    linearSearch(const void *key, size_t   from, size_t count, AbstractComparator &comparator) const;
  intptr_t    binarySearch(const void *key, intptr_t from, size_t count, AbstractComparator &comparator) const;
  intptr_t    binaryInsert(const void *e, AbstractComparator &comparator);
  void        indexError(               const TCHAR *method, size_t index) const;
  void        indexError(               const TCHAR *method, size_t index, size_t count) const;
  static void emptyArrayError(          const TCHAR *method);
  void        unsupportedOperationError(const TCHAR *method) const;
public:
  ArrayImpl(AbstractObjectManager &objectManager, size_t capacity);
  AbstractCollection *clone(bool cloneData)                                   const override;
  ~ArrayImpl()                                                                      override;
  // If capacity < 0, it's left unchanged
  void                 clear(intptr_t capacity);
  void                 clear()                                                      override;
  size_t               size()                                                 const override {
    return m_size;
  }
  bool                 add(                      const void *e)                      override;
  bool                 insert(     size_t index, const void *e, size_t count);
  void                 removeIndex(size_t index, size_t count);
  bool                 remove(                   const void *e)                      override;
  // unsuppoerted
  bool                 contains(                 const void *e)                const override;
  void                *select(RandomGenerator &rnd)                            const override;
  inline size_t        getCapacity()                                           const {
    return m_capacity;
  }
  void                 setCapacity(size_t capacity);

  inline void         *getElement(size_t index)                                const {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_elem[index];
  }

  AbstractIterator    *getIterator()                                           const override;
  bool                 hasOrder()                                              const override {
    return true;
  }
  void                 swap(   size_t i1, size_t i2);
  void                 shuffle(size_t from, size_t count, RandomGenerator &rnd);
  void                 reverse();
  bool                 equals(const ArrayImpl *rhs) const;
  void                 arraySort(size_t from, size_t count, int (*compare)(const void **e1, const void **e2));
  void                 arraySort(size_t from, size_t count, int (*compare)(const void  *e1, const void  *e2));
  void                 arraySort(size_t from, size_t count, AbstractComparator &comparator);

  intptr_t lSearch(const void *key, size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) const;
  intptr_t lSearch(const void *key, size_t from, size_t count, int (*compare)(const void  *e1, const void  *e2)) const;
  intptr_t lSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const;

  intptr_t bSearch(const void *key, size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) const;
  intptr_t bSearch(const void *key, size_t from, size_t count, int (*compare)(const void  *e1, const void  *e2)) const;
  intptr_t bSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const;

  intptr_t bInsert(const void *e, int (*compare)(const void **e1, const void **e2));
  intptr_t bInsert(const void *e, int (*compare)(const void  *e1, const void  *e2));
  intptr_t bInsert(const void *e, AbstractComparator &comparator);
};

template<typename T> class Array : public Collection<T> {
public:
  Array() : Collection<T>(new ArrayImpl(ObjectManager<T>(), 10)) {
  }

  explicit Array(size_t capacity) : Collection(new ArrayImpl(ObjectManager<T>(), capacity)) {
  }

  Array(const CollectionBase<T> &src) : Collection(new ArrayImpl(ObjectManager<T>(), src.size())) {
    addAll(src);
  }

  Array &operator=(const CollectionBase<T> &src) {
    __super::operator=(src);
    return *this;
  }

  inline size_t getCapacity() const {
    return ((ArrayImpl*)m_collection)->getCapacity();
  }

  // Return *this
  inline Array &setCapacity(size_t capacity) {
    ((ArrayImpl*)m_collection)->setCapacity(capacity);
    return *this;
  }

  // If capacity < 0, it's left unchanged
  // Return *this
  Array &clear(intptr_t capacity) {
    ((ArrayImpl *)m_collection)->clear(capacity);
    return *this;
  }

  void clear() override {
    ((ArrayImpl *)m_collection)->clear();
  }

  inline T &operator[](size_t i) {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(i));
  }

  inline const T &operator[](size_t i) const {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(i));
  }

  inline T &first() {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(0));
  }

  inline const T &first() const {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(0));
  }

  inline T &last() {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(m_collection->size()-1));
  }

  inline const T &last() const {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(m_collection->size()-1));
  }

  // NB not virtual in Collection, because of ==
  bool operator==(const Array &a) const {
    const size_t n = size();
    if(n != a.size()) {
      return false;
    }
    for(size_t i = 0; i < n; i++) {
      if(!((*this)[i] == a[i])) {
        return false;
      }
    }
    return true;
  }

  inline bool operator!=(const Array &a) const {
    return !(operator==(a));
  }

  // Return min(k) so (*this)[k] == e, if such k exist, or else return -1
  intptr_t getFirstIndex(const T &e) const {
    const size_t n = size();
    for(size_t i = 0; i < n; i++) {
      if((*this)[i] == e) {
        return i;
      }
    }
    return -1;
  }

  // NB not virtual in Collection, because of ==
  inline bool contains(const T &e) const {
    return getFirstIndex(e) >= 0;
  }

  // Insert the same element e position [index..index+count-1]
  inline bool insert(size_t index, const T &e, size_t count=1) {
    return ((ArrayImpl*)m_collection)->insert(index, &e, count);
  }

  bool add(const T &e) override {
    return m_collection->add(&e);
  }

  inline bool remove(const T &e) {                    // NB not virtual in Collection, because of ==
    const size_t n = size();
    for(size_t i = 0; i < n; i++) {
      if((*this)[i] == e) {
        removeIndex(i);
        return true;
      }
    }
    return false;
  }

  inline bool removeIndex(size_t i, size_t count=1) {
    ((ArrayImpl*)m_collection)->removeIndex(i, count);
    return count > 0;
  }

  // Return *this
  inline Array &removeLast() {
    ((ArrayImpl*)m_collection)->removeIndex(size()-1, 1);
    return *this;
  }

  // Return *this
  inline Array &swap(size_t i1, size_t i2) {
    ((ArrayImpl*)m_collection)->swap(i1, i2);
    return *this;
  }

  // Return *this
  inline Array &shuffle(size_t from, size_t count, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    ((ArrayImpl*)m_collection)->shuffle(from, count, rnd);
    return *this;
  }

  // Return *this
  inline Array &shuffle(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    return shuffle(0, size(), rnd);
  }

  // Return *this
  inline Array &reverse() {
    ((ArrayImpl*)m_collection)->reverse();
    return *this;
  }

  class PermutationHandler {
  public:
    virtual bool handlePermutation(const Array &a) = 0;
  };

private:
  bool permuter(size_t n, PermutationHandler &handler) {
    if(n <= 1) {
      return handler.handlePermutation(*this);
    } else {
      n--;
      if(!permuter(n, handler)) {
        return false;
      }
      for(size_t i = 0; i < n; i++) {
        swap(i, n);
        if(!permuter(n, handler)) {
          return false;
        }
        swap(i, n);
      }
    }
    return true;
  }

public:

  bool generateAllPermuations(PermutationHandler &handler) {
    return permuter(size(), handler);
  }

  // Return *this
  Array &sort(size_t from, size_t count, Comparator<T> &comparator) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, comparator);
    return *this;
  }

  // Return *this
  inline Array &sort(Comparator<T> &comparator) {
    return sort(0, size(), comparator);
  }

  // Return *this
  inline Array &sort(size_t from, size_t count, int (*compare)(const T **e1, const T **e2)) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, (int(*)(const void **, const void **))compare);
    return *this;
  }

  // Return *this
  inline Array &sort(int (*compare)(const T **e1, const T **e2)) {
    return sort(0, size(), compare);
  }

  // Return *this
  Array &sort(size_t from, size_t count, int (*compare)(const T &e1 , const T &e2)) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, (int(*)(const void *, const void *))compare);
    return *this;
  }

  // Return *this
  inline Array &sort(int (*compare)(const T &e1, const T &e2)) {
    return sort(0, size(), compare);
  }

  intptr_t search(const T &key, size_t from, size_t count, int (*compare)(const T **e1, const T **e2)) const {
    return ((ArrayImpl*)m_collection)->lSearch(&key, from, count, (int(*)(const void **, const void **))compare);
  }

  intptr_t search(const T &key, int (*compare)(const T **e1, const T **e2)) const {
    return search(key, 0, size(), compare);
  }

  intptr_t search(const T &key, size_t from, size_t count, int (*compare)(const T &e1, const T &e2)) const {
    return ((ArrayImpl*)m_collection)->lSearch(&key, from, count, (int(*)(const void *, const void *))compare);
  }

  intptr_t search(const T &key, int (*compare)(const T &e1, const T &e2)) const {
    return search(key, 0, size(), compare);
  }

  intptr_t search(const T &key, size_t from, size_t count, Comparator<T> &comparator) const {
    return ((ArrayImpl*)m_collection)->lSearch(&key, from, count, comparator);
  }

  intptr_t search(const T &key, Comparator<T> &comparator) const {
    return search(key, 0, size(), comparator);
  }

  intptr_t binarySearch(const T &key, size_t from, size_t count, int (*compare)(const T **e1, const T **e2)) const {
    return ((ArrayImpl*)m_collection)->bSearch(&key, from, count, (int(*)(const void **, const void **))compare);
  }

  intptr_t binarySearch(const T &key, int (*compare)(const T **e1, const T **e2)) const {
    return binarySearch(key, 0, size(), compare);
  }

  intptr_t binarySearch(const T &key, size_t from, size_t count, int (*compare)(const T &e1, const T &e2)) const {
    return ((ArrayImpl*)m_collection)->bSearch(&key, from, count, (int(*)(const void *, const void *))compare);
  }

  intptr_t binarySearch(const T &key, int (*compare)(const T &e1, const T &e2)) const {
    return binarySearch(key, 0, size(), compare);
  }

  intptr_t binarySearch(const T &key, size_t from, size_t count, Comparator<T> &comparator) const {
    return ((ArrayImpl*)m_collection)->bSearch(&key, from, count, comparator);
  }

  intptr_t binarySearch(const T &key, Comparator<T> &comparator) const {
    return binarySearch(key, 0, size(), comparator);
  }

  intptr_t binaryInsert(const T &key, int (*compare)(const T **e1, const T **e2)) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, (int(*)(const void **, const void **))compare);
  }

  intptr_t binaryInsert(const T &key, int (*compare)(const T  *e1, const T  *e2)) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, (int(*)(const void *, const void *))compare);
  }

  intptr_t binaryInsert(const T &key, int (*compare)(const T &e1, const T &e2)) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, (int(*)(const void *, const void *))compare);
  }

  intptr_t binaryInsert(const T &key, Comparator<T> &comparator) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, comparator);
  }
};

typedef Array<const TCHAR*> StrArray;

typedef Array<CHAR  > CharArray;
typedef Array<SHORT > ShortArray;
typedef Array<INT   > IntArray;
typedef Array<LONG  > LongArray;
typedef Array<FLOAT > FloatArray;
typedef Array<DOUBLE> DoubleArray;
