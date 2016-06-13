#pragma once

#include "MyUtil.h"
#include "Packer.h"
#include "ByteFile.h"
#include "Comparator.h"
#include "Random.h"

extern const TCHAR *_compactArrayIteratorClassName;
extern const TCHAR *_compactArrayClassName;

template <class T> class CompactArray {
private:
  size_t  m_capacity;
  size_t  m_size;
  size_t  m_updateCount;
  T      *m_array;

  void indexError(size_t index, const TCHAR *label=_T("")) const {
    throwException(_T("%s:%sIndex %lu out of range. size=%lu"), _compactArrayClassName, label, index, m_size);
  }

  void selectError() const {
    throwException(_T("%s:Cannot select from empty array"), _compactArrayClassName);
  }

  size_t getSortCount(size_t from, size_t count) const {
    const intptr_t length = size() - from;
    return min((intptr_t)count, length);
  }

  void init(size_t capacity, size_t size) {
    m_size        = size;
    m_updateCount = 0;
    m_capacity    = capacity;
    m_array       = m_capacity ? new T[m_capacity] : NULL;
  }

  void setCapacityNoCopy(size_t capacity) {
    if(capacity == m_capacity) return;
    if(m_array) {
      delete[] m_array;
    }
    m_array    = capacity ? new T[capacity] : NULL;
    m_capacity = capacity;
  }

public:
  CompactArray() {
    init(0, 0);
  }
  
  explicit CompactArray(size_t capacity) {
    init(capacity, 0);
  }

  CompactArray(const CompactArray<T> &src) {
    init(src.m_size, src.m_size);
    if(m_size) {
      memcpy(m_array, src.m_array, m_size * sizeof(T));
    }
  }

  CompactArray<T> &operator=(const CompactArray<T> &src) {
    if(this == &src) {
      return *this;
    }
    setCapacityNoCopy(src.size());
    m_size = src.m_size;
    if(m_size) {
      memcpy(m_array, src.m_array, m_size * sizeof(T));
    }
    m_updateCount++;
    return *this;
  }

  ~CompactArray() {
    if(m_array) {
      delete[] m_array;
    }
  }
  
  void setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    T *newArray = capacity ? new T[capacity] : NULL;
    if(m_size) {
      memcpy(newArray, m_array, m_size * sizeof(T));
    }
    if(m_array) {
      delete[] m_array;
    }
    m_array    = newArray;
    m_capacity = capacity;
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  inline T &operator[](size_t index) {
    if(index >= m_size) indexError(index, _T("operator[]"));
    return m_array[index];
  }

  inline const T &operator[](size_t index) const {
    if(index >= m_size) indexError(index, _T("operator[]"));
    return m_array[index];
  }
  
  inline const T &select() const {
    if(m_size == 0) selectError();
    return m_array[randSizet() % m_size];
  }

  inline T &select() {
    if(m_size == 0) selectError();
    return m_array[randSizet() % m_size];
  }

  CompactArray<T> getRandomSample(size_t k) const {
    if(k > m_size) {
      throwInvalidArgumentException(_T("getRandomSample"), _T("k(=%u) > size(=%u)"), k, m_size);
    }
    CompactArray<T> result(k);
    for(size_t i = 0; i < k; i++) {
      result.add(m_array[i]);
    }
    if(k > 0) {
      for(size_t i = k; i < m_size; i++) {
        const size_t j = randSizet(i+1);
        if(j < k) {
          result[j] = m_array[i];
        }
      }
    }
    return result;
  }

  inline T &first() {
    if(m_size == 0) indexError(0, _T("first"));
    return m_array[0];
  }

  inline const T &first() const {
    if(m_size == 0) indexError(0, _T("first"));
    return m_array[0];
  }

  inline T &last() {
    if(m_size == 0) indexError(m_size, _T("last"));
    return m_array[m_size-1];
  }

  inline const T &last() const {
    if(m_size == 0) indexError(m_size, _T("last"));
    return m_array[m_size-1];
  }

  bool contains(const T &e) const {
    return getFirstIndex(e) >= 0;
  }

  intptr_t getFirstIndex(const T &e) const {
    if(m_size == 0) {
      return -1;
    }
    for(const T *p = m_array, *last = p + m_size; p < last;) {
      if(*(p++) == e) {
        return p - m_array - 1;
      }
    }
    return -1;
  }

  void add(const T &e) {
    if(m_size == m_capacity) {
      setCapacity(2*m_capacity+5);
    }
    m_array[m_size++] = e;
    m_updateCount++;
  }

  void add(size_t index, const T &e, size_t count = 1) {
    if(index > m_size) indexError(index, _T("add"));
    if(count == 0) return;
    const size_t newSize = m_size + count;
    if(newSize > m_capacity) {
      setCapacity(2*(newSize) + 5);
    }
    if(index < m_size) {
      memmove(m_array+index+count, m_array+index, (m_size-index)*sizeof(T));
    }
    m_size = newSize;
    while(count--) {
      m_array[index++] = e;
    }
    m_updateCount++;
  }

  void add(size_t index, const T *ep, size_t count) {
    if(index > m_size) indexError(index, _T("add"));
    if(count == 0) return;
    const size_t newSize = m_size + count;
    if(newSize > m_capacity) {
      setCapacity(2*(newSize) + 5);
    }
    if(index < m_size) {
      memmove(m_array+index+count, m_array+index, (m_size-index)*sizeof(T));
    }
    m_size = newSize;
    while(count--) {
      m_array[index++] = *(ep++);
    }
    m_updateCount++;
  }

  void append(const T *ep, size_t count) {
    add(size(), ep, count);
  }

  bool addAll(const CompactArray<T> &src) {
    if(src.isEmpty()) {
      return false;
    }
    const size_t newSize = m_size + src.m_size;
    if(newSize > m_capacity) {
      setCapacity(2*(newSize) + 5);
    }
    memcpy(m_array+m_size, src.m_array, src.m_size*sizeof(T));
    m_size = newSize;
    m_updateCount++;
    return true;
  }

  void remove(size_t index, size_t count = 1) {
    if(count == 0) {
      return;
    }
    const size_t j = index+count;
    if(j > m_size) indexError(j, format(_T("remove(%lu,%lu):"), index, count).cstr());
    if(j < m_size) {
      memmove(m_array+index, m_array+j, (m_size-j) * sizeof(T));
    }
    m_size -= count;
    if(m_size < m_capacity/2) {
      setCapacity(m_size);
    }
    m_updateCount++;
  }

  void removeLast() {
    if(m_size == 0) indexError(m_size, _T("removeLast"));
    remove(m_size-1);
  }

  CompactArray<T> &swap(size_t i1, size_t i2) {
    if(i1 >= m_size) indexError(i1, _T("swap"));
    if(i2 >= m_size) indexError(i2, _T("swap"));
    const T tmp = m_array[i1];
    m_array[i1] = m_array[i2];
    m_array[i2] = tmp;
    m_updateCount++;
    return *this;
  }

  CompactArray<T> &shuffle(size_t from, size_t count) {
    if(from >= (size_t)size()) {
      return *this;
    }
    if((count = getSortCount(from, count)) > 1) {
      for(size_t i = 0; i < count; i++ ) {
        swap(from + i, from + randSizet() % count);
      }
    }
    m_updateCount++;
    return *this;
  }

  class PermutationHandler {
    public:
      virtual bool handlePermutation(const CompactArray<T> &a) = 0;
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
  CompactArray<T> &shuffle() {
    return shuffle(0, size());
  }

  bool generateAllPermuations(PermutationHandler &handler) {
    return permuter(size(), handler);
  }

  CompactArray<T> &clear(intptr_t capacity=0) { // if capacity < 0, it's left unchanged
    if(m_size != 0) {
      m_updateCount++;
    }
    m_size = 0;
    if(capacity >= 0) {
      setCapacity(capacity);
    }
    return *this;
  }

  CompactArray<T> &sort(size_t from, size_t count, Comparator<T> &cmp) {
    if(from >= (size_t)size()) {
      return *this;
    }
    if((count = getSortCount(from, count)) > 1) {
      quickSort(m_array+from, count, sizeof(T), cmp);
      m_updateCount++;
    }
    return *this;
  }

  CompactArray<T> &sort(Comparator<T> &cmp) {
    return sort(0, size(), cmp);
  }

  CompactArray<T> &sort(size_t from, size_t count, int (*cmp)(const T &e1, const T &e2)) {
    return sort(from, count, FunctionComparator<T>(cmp));
  }

  CompactArray<T> &sort(int (*cmp)(const T &e1, const T &e2)) {
    return sort(0, size(), FunctionComparator<T>(cmp));
  }
    
  // Return index i so a[i] == key. If none exist, return -1
  intptr_t binarySearch(const T &key, Comparator<T> &cmp) const { // Assume array is sorted
    intptr_t l = 0, r = m_size-1;
    while(l <= r) {
      const intptr_t m = (l + r) / 2;
      const int c = cmp.compare(m_array[m], key);
      if(c < 0) {
        l = m + 1;
      } else if(c > 0) {
        r = m - 1;
      } else {
        return m;
      }
    }
    return -1;
  }

  intptr_t binarySearch(const T &key, int (*cmp)(const T &e1, const T &e2)) const { // Assume array is sorted
    return binarySearch(key, FunctionComparator<T>(cmp));
  }

  // Return largest index i so a[i] <= key. If none exist, return -1
  intptr_t binarySearchLE(const T &key, Comparator<T> &cmp) const { // Assume array is sorted
    intptr_t l = 0, r = m_size;
    while(l < r) {
      const intptr_t m = (l + r) / 2;
      if(cmp.compare(m_array[m], key) <= 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    return r - 1;
  }

  intptr_t binarySearchLE(const T &key, int (*cmp)(const T &e1, const T &e2)) const { // Assume array is sorted
    return binarySearchLE(key, FunctionComparator<T>(cmp));
  }

  // Return smallest index i so a[i] >= key. If none exist, return size
  intptr_t binarySearchGE(const T &key, Comparator<T> &cmp) const { // Assume array is sorted
    intptr_t l = 0, r = m_size;
    while(l < r) {
      const intptr_t m = (l + r) / 2;
      if(cmp.compare(m_array[m], key) < 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    return r;
  }

  intptr_t binarySearchGE(const T &key, int (*cmp)(const T &e1, const T &e2)) const { // Assume array is sorted
    return binarySearchGE(key, FunctionComparator<T>(cmp));
  }

  bool operator==(const CompactArray<T> &a) const {
    size_t count = size();
    if(count != a.size()) {
      return false;
    }
    for(const T *p1 = m_array, *p2 = a.m_array; count--;) {
      if(!(*(p1++) == *(p2++))) {
        return false;
      }
    }
    return true;
  }

  inline bool operator!=(const CompactArray<T> &a) const {
    return !(*this == a);
  }

  unsigned long hashCode() const {
    unsigned long sum = 0;
    size_t count = size();
    for(const T *p = m_array; count--;) {
      sum = sum * 31 + (p++)->hashCode();
    }
    return sum;
  }

  inline bool isEmpty() const {
    return m_size == 0;;
  };

  inline size_t size() const {
    return m_size;
  }

  inline const T *getBuffer() const { // can return NULL
    return m_array;
  }

  void save(ByteOutputStream &s) const { // modify this => take care of CompactFileArray
    s.putBytes((BYTE*)&m_size, sizeof(m_size));
    if(m_size) {
      s.putBytes((BYTE*)m_array, sizeof(T)*m_size);
    }
  }

  void load(ByteInputStream &s) {
    clear();
    size_t size;
    s.getBytesForced((BYTE*)&size, sizeof(size));
    setCapacity(size);
    if(size) {
      s.getBytesForced((BYTE*)m_array, sizeof(T)*size);
    }
    m_size = size;
  }

  friend Packer &operator<<(Packer &p, const CompactArray<T> &s) {
    const unsigned int elemSize = sizeof(T);
    p << elemSize << s.m_size;
    const T *q = s.m_array;
    for(size_t i = s.m_size; i--;) {
      p << *(q++);
    }
    return p;
  }

  friend Packer &operator>>(Packer &p, CompactArray<T> &s) {
    unsigned int elemSize;
    p >> elemSize;
    if(elemSize != sizeof(T)) {
      throwException(_T("Invalid element size:%d bytes. Expected %d bytes"), elemSize, sizeof(T));
    }
    size_t size;
    s.clear();
    p >> size;
    s.setCapacity(size);
    for(size_t i = size; i--;) {
      T e;
      p >> e;
      s.add(e);
    }
    return p;
  }

  String toString() const {
    String result = _T("{");
    if(m_size) {
      const T *p = m_array;
      result += (p++)->toString();
      for(size_t i = m_size-1; i--;) {
        result += _T(",");
        result += (p++)->toString();
      }
    }
    result += _T("}");
    return result;
  }

  String toStringBasicType() const {
    String result = _T("{");
    if(m_size) {
      const T *p = m_array;
      result += ::toString(*(p++));
      for(size_t i = m_size-1; i--;) {
        result += _T(",");
        result += ::toString(*(p++));
      }
    }
    result += _T("}");
    return result;
  }

  inline size_t getUpdateCount() const {
    return m_updateCount;
  }

  class CompactArrayIterator : public AbstractIterator {
  private:
    CompactArray<T> &m_a;
    size_t           m_next;
    intptr_t         m_current;
    size_t           m_updateCount;
    void checkUpdateCount() const {
      if(m_updateCount != m_a.getUpdateCount()) {
        concurrentModificationError(_compactArrayIteratorClassName);
      }
    }
  public:
    CompactArrayIterator(CompactArray<T> *a) : m_a(*a) {
      m_next        = 0;
      m_current     = -1;
      m_updateCount = m_a.getUpdateCount();
    }
    AbstractIterator *clone() {
      return new CompactArrayIterator(*this);
    }

    inline bool hasNext() const {
      return m_next < m_a.size();
    }

    void *next() {
      if(m_next >= m_a.size()) {
        noNextElementError(_compactArrayIteratorClassName);
      }
      checkUpdateCount();
      m_current = m_next++;
      return &m_a[m_current];
    }

    void remove() {
      if(m_current < 0) {
        noCurrentElementError(_compactArrayIteratorClassName);
      }
      checkUpdateCount();
      m_a.remove(m_current,1);
      m_current     = -1;
      m_updateCount = m_a.getUpdateCount();
    }
  };


  Iterator<T> getIterator() {
    return Iterator<T>(new CompactArrayIterator(this));
  }
};

typedef CompactArray<char*>  CompactStrArray;
typedef CompactArray<char>   CompactCharArray;
typedef CompactArray<short>  CompactShortArray;
typedef CompactArray<int>    CompactIntArray;
typedef CompactArray<long>   CompactLongArray;
typedef CompactArray<float>  CompactFloatArray;
typedef CompactArray<double> CompactDoubleArray;
typedef CompactArray<size_t> CompactSizetArray;

template <class T> class CompactFileArray {
private:
  mutable ByteInputFile  m_f;
  const unsigned __int64 m_dataStartOffset;
  size_t                 m_size;

  void indexError(size_t index, const TCHAR *label=_T("")) const {
    throwException(_T("CompactArray::%sIndex %lu out of range. Size=%lu, elementSize:%d")
                  ,label, index, m_size, sizeof(T));
  }

public:
  CompactFileArray(const String &fileName, unsigned __int64 startOffset) 
    : m_dataStartOffset(startOffset + sizeof(m_size))
    , m_f(fileName)
  {
    m_f.seek(startOffset);
    m_f.getBytesForced((BYTE*)&m_size, sizeof(m_size));
  }
  CompactFileArray(const CompactFileArray &src) {
    throwException(_T("CompactFileArray:Copy-constructor not allowed"));
  }
  CompactFileArray &operator=(const CompactFileArray &src) {
    throwException(_T("CompactFileArray:assignment not allowed"));
    return *this;
  }

  inline size_t size() const {
    return m_size;
  }

  T operator[](size_t index) const {
    if(index >= m_size) indexError(index, _T("operator[]"));

    m_f.seek(m_dataStartOffset + index * sizeof(T));
    T result;
    m_f.getBytesForced((BYTE*)&result, sizeof(T));
    return result;
  }
};
