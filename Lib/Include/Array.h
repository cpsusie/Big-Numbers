#pragma once

#include "Collection.h"
#include "Tokenizer.h"
#include "Comparator.h"
#include "ObjectManager.h"

class ArrayImpl : public AbstractCollection {
private:
  AbstractObjectManager *m_objectManager;
  void                 **m_elem;
  size_t                 m_size;
  size_t                 m_capacity;
  size_t                 m_updateCount;

  void      resize();
  void      init(const AbstractObjectManager &objectManager, size_t size, size_t capacity);
  intptr_t  getSortCount(intptr_t from, intptr_t count) const;
  void      sort(size_t from, size_t count, AbstractComparator &comparator);
  intptr_t  linearSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const;
  intptr_t  binarySearch(const void *key, intptr_t from, size_t count, AbstractComparator &comparator) const;
  intptr_t  binaryInsert(const void *e, AbstractComparator &comparator);
  void      indexError(const TCHAR *method, size_t index) const;
  void      selectError() const;
  void      unsupportedOperationError(const TCHAR *method) const;
  friend class ArrayIterator;
public:
  ArrayImpl(AbstractObjectManager &objectManager, size_t capacity);
  ArrayImpl(AbstractObjectManager &objectManager, const AbstractCollection *src);
  ~ArrayImpl();
  AbstractCollection *clone(bool cloneData) const;
  ArrayImpl &operator=(const ArrayImpl &src);
  bool add(const void *e);
  bool add(size_t i, const void *e, size_t count);
  void removeIndex(size_t i, size_t count);
  bool remove(const void *e);
  bool contains(const void *e) const; // unsuppoerted
  const void *select() const ;
  void *select();
  inline size_t getCapacity() const {
    return m_capacity;
  }
  void setCapacity(size_t capacity);
  void clear();
  inline size_t size() const {
    return m_size;
  }
  inline void *getElement(size_t index) {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_elem[index];
  }

  inline const void *getElement(size_t index) const {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_elem[index];
  }

  void swap(size_t i1, size_t i2);
  void shuffle(size_t from, size_t count);
  bool equals(const ArrayImpl *rhs) const;
  void arraySort(size_t from, size_t count, int (*compare)(const void **e1, const void **e2));
  void arraySort(size_t from, size_t count, int (*compare)(const void  *e1, const void  *e2));
  void arraySort(size_t from, size_t count, AbstractComparator &comparator);

  intptr_t lSearch(const void *key, size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) const;
  intptr_t lSearch(const void *key, size_t from, size_t count, int (*compare)(const void  *e1, const void  *e2)) const;
  intptr_t lSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const;

  intptr_t bSearch(const void *key, size_t from, size_t count, int (*compare)(const void **e1, const void **e2)) const;
  intptr_t bSearch(const void *key, size_t from, size_t count, int (*compare)(const void  *e1, const void  *e2)) const;
  intptr_t bSearch(const void *key, size_t from, size_t count, AbstractComparator &comparator) const;

  intptr_t bInsert(const void *e, int (*compare)(const void **e1, const void **e2));
  intptr_t bInsert(const void *e, int (*compare)(const void  *e1, const void  *e2));
  intptr_t bInsert(const void *e, AbstractComparator &comparator);

  AbstractIterator *getIterator();
};

/*
template <class T> class TArrayImpl : public ArrayImpl {
public:
  TArrayImpl(AbstractObjectManager &om, size_t capacity) : ArrayImpl(om, capacity) {
  }
  TArrayImpl(AbstractObjectManager &om, const AbstractCollection *src) : ArrayImpl(om, src) {
  }
};
*/

template <class T> class Array : public Collection<T> {
public:
  Array() : Collection<T>(new ArrayImpl(ObjectManager<T>(), 10)) {
  }

  explicit Array(size_t capacity) : Collection<T>(new ArrayImpl(ObjectManager<T>(), capacity)) {
  }

  Array(const Collection<T> &src) : Collection<T>(new ArrayImpl(ObjectManager<T>(), src.size())) {
    addAll(src);
  }

  Array<T> &operator=(const Collection<T> &src) {
    if(this == &src) {
      return *this;
    }
    clear();
    addAll(src);
    return *this;
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
  
  bool operator==(const Array<T> &a) const { // NB not virtual in Collection, because of ==
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

  inline bool operator!=(const Array<T> &a) const {
    return !(operator==(a));
  }

  inline bool add(size_t i, const T &e, size_t count=1) {
    return ((ArrayImpl*)m_collection)->add(i, &e, count);
  }

  inline bool add(const T &e) {
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

  inline void removeLast() {
    ((ArrayImpl*)m_collection)->removeIndex(size()-1, 1);
  }

  bool contains(const T &e) const {            // NB not virtual in Collection, because of ==
    return getFirstIndex(e) >= 0;
  }

  intptr_t getFirstIndex(const T &e) const {
    const size_t n = size();
    for(size_t i = 0; i < n; i++) {
      if((*this)[i] == e) {
        return i;
      }
    }
    return -1;
  }

  inline Array<T> &swap(size_t i1, size_t i2) {
    ((ArrayImpl*)m_collection)->swap(i1, i2);
    return *this;
  }

  inline Array<T> &shuffle(size_t from, size_t count) {
    ((ArrayImpl*)m_collection)->shuffle(from, count);
    return *this;
  }

  inline Array<T> &shuffle() {
    return shuffle(0, size());
  }

  class PermutationHandler {
  public:
    virtual bool handlePermutation(const Array<T> &a) = 0;
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

  inline Array<T> &sort(size_t from, size_t count, int (*compare)(const T **e1, const T **e2)) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, (int(*)(const void **, const void **))compare);
    return *this;
  }

  inline Array<T> &sort(int (*compare)(const T **e1, const T **e2)) {
    return sort(0, size(), compare);
  }

  Array<T> &sort(size_t from, size_t count, int (*compare)(const T &e1 , const T &e2)) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, (int(*)(const void *, const void *))compare);
    return *this;
  }

  inline Array<T> &sort(int (*compare)(const T &e1, const T &e2)) {
    return sort(0, size(), compare);
  }

  Array<T> &sort(size_t from, size_t count, Comparator<T> &comparator) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, comparator);
    return *this;
  }

  inline Array<T> &sort(Comparator<T> &comparator) {
    return sort(0, size(), comparator);
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

  inline size_t getCapacity() const {
    return ((ArrayImpl*)m_collection)->getCapacity();
  }

  inline void setCapacity(size_t capacity) {
    ((ArrayImpl*)m_collection)->setCapacity(capacity);
  }
};

typedef Array<const char*> StrArray;

class StringArray : public Array<String> {
public:
  StringArray() {}
  StringArray(const char **strArray); // terminate with NULL-pointer
  StringArray(Tokenizer &tok);
  size_t maxLength() const;
  size_t minLength() const;
  String getAsDoubleNullTerminatedString() const; // return a double 0-terminated string containing all strings of the array, each 0-terminated
};

typedef Array<char>   CharArray;
typedef Array<short>  ShortArray;
typedef Array<int>    IntArray;
typedef Array<long>   LongArray;
typedef Array<float>  FloatArray;
typedef Array<double> DoubleArray;
