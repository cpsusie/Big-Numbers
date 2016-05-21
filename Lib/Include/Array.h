#pragma once

#include "Collection.h"
#include "Tokenizer.h"
#include "Comparator.h"
#include "ObjectManager.h"

class ArrayImpl : public AbstractCollection {
private:
  DECLARECLASSNAME;
  AbstractObjectManager *m_objectManager;
  void                 **m_elem;
  unsigned long          m_size;
  unsigned long          m_capacity;
  unsigned long          m_updateCount;

  void resize();
  void init(const AbstractObjectManager &objectManager, int size, unsigned long capacity);
  int  getSortCount(int from, int count) const;
  void sort(int from, int count, AbstractComparator &comparator);
  int  linearSearch(const void *key, int from, int count, AbstractComparator &comparator) const;
  int  binarySearch(const void *key, int from, int count, AbstractComparator &comparator) const;
  int  binaryInsert(const void *e, AbstractComparator &comparator);
  void indexError(const TCHAR *method, int index) const;
  void selectError() const;
  void unsupportedOperationError(const TCHAR *method) const;
  friend class ArrayIterator;
public:
  ArrayImpl(AbstractObjectManager &objectManager, unsigned long capacity);
  ArrayImpl(AbstractObjectManager &objectManager, const AbstractCollection *src);
  ~ArrayImpl();
  AbstractCollection *clone(bool cloneData) const;
  ArrayImpl &operator=(const ArrayImpl &src);
  bool add(const void *e);
  bool add(unsigned int i, const void *e, unsigned int count);
  void removeIndex(unsigned int i, unsigned int count);
  bool remove(const void *e);
  bool contains(const void *e) const; // unsuppoerted
  const void *select() const ;
  void *select();
  inline int getCapacity() const {
    return m_capacity;
  }
  void setCapacity(unsigned long capacity);
  void clear();
  inline int size() const {
    return m_size;
  }
  inline void *getElement(unsigned int index) {
    if(index >= m_size) indexError(_T("getElement"),index);
    return m_elem[index];
  }

  inline const void *getElement(unsigned int index) const {
    if(index >= m_size) indexError(_T("const getElement"),index);
    return m_elem[index];
  }

  void swap(unsigned int i1, unsigned int i2);
  void shuffle(int from, int count);
  bool equals(const ArrayImpl *rhs) const;
  void arraySort(int from, int count, int (*compare)(const void **e1, const void **e2));
  void arraySort(int from, int count, int (*compare)(const void  *e1, const void  *e2));
  void arraySort(int from, int count, AbstractComparator &comparator);

  int lSearch(const void *key, int from, int count, int (*compare)(const void **e1, const void **e2)) const;
  int lSearch(const void *key, int from, int count, int (*compare)(const void  *e1, const void  *e2)) const;
  int lSearch(const void *key, int from, int count, AbstractComparator &comparator) const;

  int bSearch(const void *key, int from, int count, int (*compare)(const void **e1, const void **e2)) const;
  int bSearch(const void *key, int from, int count, int (*compare)(const void  *e1, const void  *e2)) const;
  int bSearch(const void *key, int from, int count, AbstractComparator &comparator) const;

  int bInsert(const void *e, int (*compare)(const void **e1, const void **e2));
  int bInsert(const void *e, int (*compare)(const void  *e1, const void  *e2));
  int bInsert(const void *e, AbstractComparator &comparator);

  AbstractIterator *getIterator();
};

template <class T> class Array : public Collection<T> {
public:
  Array() : Collection<T>(new ArrayImpl(ObjectManager<T>(), 10)) {
  }

  explicit Array(unsigned long capacity) : Collection<T>(new ArrayImpl(ObjectManager<T>(), capacity)) {
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

  inline T &operator[](unsigned int i) {
    return *(T*)(((ArrayImpl*)m_collection)->getElement(i));
  }
  
  inline const T &operator[](unsigned int i) const {
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
    const int n = size();
    if(n != a.size()) {
      return false;
    }
    for(int i = 0; i < n; i++) {
      if(!((*this)[i] == a[i])) {
        return false;
      }
    }
    return true;
  }

  inline bool operator!=(const Array<T> &a) const {
    return !(operator==(a));
  }

  inline bool add(unsigned int i, const T &e, unsigned int count=1) {
    return ((ArrayImpl*)m_collection)->add(i, &e, count);
  }

  inline bool add(const T &e) {
    return m_collection->add(&e);
  }

  inline bool remove(const T &e) {                    // NB not virtual in Collection, because of ==
    const int n = size();
    for(int i = 0; i < n; i++) {
      if((*this)[i] == e) {
        removeIndex(i);
        return true;
      }
    }
    return false;
  }

  inline bool removeIndex(unsigned int i, unsigned int count=1) {
    ((ArrayImpl*)m_collection)->removeIndex(i, count);
    return count > 0;
  }

  inline void removeLast() {
    ((ArrayImpl*)m_collection)->removeIndex(size()-1, 1);
  }

  bool contains(const T &e) const {            // NB not virtual in Collection, because of ==
    return getFirstIndex(e) >= 0;
  }

  int getFirstIndex(const T &e) const {
    const int n = size();
    for(int i = 0; i < n; i++) {
      if((*this)[i] == e) {
        return i;
      }
    }
    return -1;
  }

  inline Array<T> &swap(unsigned int i1, unsigned int i2) {
    ((ArrayImpl*)m_collection)->swap(i1, i2);
    return *this;
  }

  inline Array<T> &shuffle(int from, int count) {
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
  bool permuter(int n, PermutationHandler &handler) {
    if(n <= 1) {
      return handler.handlePermutation(*this);
    } else {
      n--;
      if(!permuter(n, handler)) {
        return false;
      }
      for(int i = 0; i < n; i++) {
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

  inline Array<T> &sort(int from, int count, int (*compare)(const T **e1, const T **e2)) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, (int(*)(const void **, const void **))compare);
    return *this;
  }

  inline Array<T> &sort(int (*compare)(const T **e1, const T **e2)) {
    return sort(0, size(), compare);
  }

  Array<T> &sort(int from, int count, int (*compare)(const T &e1 , const T &e2)) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, (int(*)(const void *, const void *))compare);
    return *this;
  }

  inline Array<T> &sort(int (*compare)(const T &e1, const T &e2)) {
    return sort(0, size(), compare);
  }

  Array<T> &sort(int from, int count, Comparator<T> &comparator) {
    ((ArrayImpl*)m_collection)->arraySort(from, count, comparator);
    return *this;
  }

  inline Array<T> &sort(Comparator<T> &comparator) {
    return sort(0, size(), comparator);
  }

  int search(const T &key, int from, int count, int (*compare)(const T **e1, const T **e2)) const {
    return ((ArrayImpl*)m_collection)->lSearch(&key, from, count, (int(*)(const void **, const void **))compare);
  }

  int search(const T &key, int (*compare)(const T **e1, const T **e2)) const {
    return search(key, 0, size(), compare);
  }

  int search(const T &key, int from, int count, int (*compare)(const T &e1, const T &e2)) const {
    return ((ArrayImpl*)m_collection)->lSearch(&key, from, count, (int(*)(const void *, const void *))compare);
  }

  int search(const T &key, int (*compare)(const T &e1, const T &e2)) const {
    return search(key, 0, size(), compare);
  }

  int search(const T &key, int from, int count, Comparator<T> &comparator) const {
    return ((ArrayImpl*)m_collection)->lSearch(&key, from, count, comparator);
  }

  int search(const T &key, Comparator<T> &comparator) const {
    return search(key, 0, size(), comparator);
  }

  int binarySearch(const T &key, int from, int count, int (*compare)(const T **e1, const T **e2)) const {
    return ((ArrayImpl*)m_collection)->bSearch(&key, from, count, (int(*)(const void **, const void **))compare);
  }

  int binarySearch(const T &key, int (*compare)(const T **e1, const T **e2)) const {
    return binarySearch(key, 0, size(), compare);
  }

  int binarySearch(const T &key, int from, int count, int (*compare)(const T &e1, const T &e2)) const {
    return ((ArrayImpl*)m_collection)->bSearch(&key, from, count, (int(*)(const void *, const void *))compare);
  }

  int binarySearch(const T &key, int (*compare)(const T &e1, const T &e2)) const {
    return binarySearch(key, 0, size(), compare);
  }

  int binarySearch(const T &key, int from, int count, Comparator<T> &comparator) const {
    return ((ArrayImpl*)m_collection)->bSearch(&key, from, count, comparator);
  }

  int binarySearch(const T &key, Comparator<T> &comparator) const {
    return binarySearch(key, 0, size(), comparator);
  }

  int binaryInsert(const T &key, int (*compare)(const T **e1, const T **e2)) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, (int(*)(const void **, const void **))compare);
  }

  int binaryInsert(const T &key, int (*compare)(const T  *e1, const T  *e2)) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, (int(*)(const void *, const void *))compare);
  }

  int binaryInsert(const T &key, int (*compare)(const T &e1, const T &e2)) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, (int(*)(const void *, const void *))compare);
  }

  int binaryInsert(const T &key, Comparator<T> &comparator) {
    return ((ArrayImpl*)m_collection)->bInsert(&key, comparator);
  }

  inline int getCapacity() const {
    return ((ArrayImpl*)m_collection)->getCapacity();
  }

  inline void setCapacity(int capacity) {
    ((ArrayImpl*)m_collection)->setCapacity(capacity);
  }
};

typedef Array<const char*> StrArray;

class StringArray : public Array<String> {
public:
  StringArray() {}
  StringArray(const char **strArray); // terminate with NULL-pointer
  StringArray(Tokenizer &tok);
  int maxLength() const;
  int minLength() const;
  String getAsDoubleNullTerminatedString() const; // return a double 0-terminated string containing all strings of the array, each 0-terminated
};

typedef Array<char>   CharArray;
typedef Array<short>  ShortArray;
typedef Array<int>    IntArray;
typedef Array<long>   LongArray;
typedef Array<float>  FloatArray;
typedef Array<double> DoubleArray;
