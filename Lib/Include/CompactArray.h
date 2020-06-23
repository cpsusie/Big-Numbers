#pragma once

#include "Packer.h"
#include "ByteFile.h"
#include "Comparator.h"
#include "Iterator.h"
#include "Random.h"

extern const TCHAR *_compactArrayIteratorClassName;

template <typename T> class CompactArray {
private:
  size_t  m_capacity;
  size_t  m_size;
  size_t  m_updateCount;
  T      *m_array;

  void indexError(const TCHAR *method, size_t index) const {
    throwIndexOutOfRangeException(method, index, m_size);
  }

  void selectError(const TCHAR *method) const {
    throwSelectFromEmptyCollectionException(method);
  }

  size_t getSortCount(size_t from, size_t count) const {
    const intptr_t length = size() - from;
    return min((intptr_t)count, length);
  }

  void init(size_t capacity, size_t size) {
    m_size        = size;
    m_updateCount = 0;
    m_capacity    = capacity;
    m_array       = m_capacity ? new T[m_capacity] : NULL; TRACE_NEW(m_array);
  }

  void setCapacityNoCopy(size_t capacity) {
    if(capacity == m_capacity) return;
    SAFEDELETEARRAY(m_array);
    m_array    = capacity ? new T[capacity] : NULL; TRACE_NEW(m_array);
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

  virtual ~CompactArray() {
    SAFEDELETEARRAY(m_array);
  }

  CompactArray<T> &setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) {
      return *this;
    }
    T *newArray = capacity ? new T[capacity] : NULL; TRACE_NEW(newArray);
    if(m_size) {
      __assume(m_size  );
      if(newArray == NULL) {
        throwException(_T("new failed"));
      }
      __assume(newArray);
      memcpy(newArray, m_array, m_size * sizeof(T));
    }
    SAFEDELETEARRAY(m_array);
    m_array    = newArray;
    m_capacity = capacity;
    return *this;
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  inline T &operator[](size_t index) {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_array[index];
  }

  inline const T &operator[](size_t index) const {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_array[index];
  }

  inline const T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    if(m_size == 0) selectError(__TFUNCTION__);
    return m_array[randSizet(rnd) % m_size];
  }

  inline T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    if(m_size == 0) selectError(__TFUNCTION__);
    return m_array[randSizet(rnd) % m_size];
  }

  CompactArray<T> getRandomSample(size_t k, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    if(k > m_size) {
      throwInvalidArgumentException(__TFUNCTION__, _T("k(=%s) > size(=%s)")
                                   ,format1000(k).cstr()
                                   ,format1000(m_size).cstr());
    }
    CompactArray<T> result(k);
    for(size_t i = 0; i < k; i++) {
      result.add(m_array[i]);
    }
    if(k > 0) {
      for(size_t i = k; i < m_size; i++) {
        const size_t j = randSizet(i+1, rnd);
        if(j < k) {
          result[j] = m_array[i];
        }
      }
    }
    return result;
  }

  inline T &first() {
    if(m_size == 0) indexError(__TFUNCTION__, 0);
    return m_array[0];
  }

  inline const T &first() const {
    if(m_size == 0) indexError(__TFUNCTION__, 0);
    return m_array[0];
  }

  inline T &last() {
    if(m_size == 0) indexError(__TFUNCTION__, m_size);
    return m_array[m_size-1];
  }

  inline const T &last() const {
    if(m_size == 0) indexError(__TFUNCTION__, m_size);
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
      setCapacity(3*m_capacity+5);
    }
    m_array[m_size++] = e;
    m_updateCount++;
  }

  void add(size_t index, const T &e, size_t count = 1) {
    if(index > m_size) indexError(__TFUNCTION__, index);
    if(count == 0) return;
    const size_t newSize = m_size + count;
    if(newSize > m_capacity) {
      setCapacity(3*(newSize) + 5);
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
    if(index > m_size) indexError(__TFUNCTION__, index);
    if(count == 0) return;
    const size_t newSize = m_size + count;
    if(newSize > m_capacity) {
      setCapacity(3*(newSize) + 5);
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
    if(j > m_size) indexError(format(_T("%s(%s,%s)"), __TFUNCTION__,format1000(index).cstr(), format1000(count).cstr()).cstr(),j);
    if(j < m_size) {
      memmove(m_array+index, m_array+j, (m_size-j) * sizeof(T));
    }
    m_size -= count;
    if(m_size < m_capacity/4) {
      setCapacity(m_size);
    }
    m_updateCount++;
  }

  void removeLast() {
    if(m_size == 0) indexError(__TFUNCTION__, m_size);
    remove(m_size-1);
  }

  CompactArray<T> &swap(size_t i1, size_t i2) {
    if(i1 >= m_size) indexError(__TFUNCTION__, i1);
    if(i2 >= m_size) indexError(__TFUNCTION__, i2);
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

  CompactArray<T> &reverse() {
    if(size() == 0) return *this;
    for(T *p1 = &first(), *p2 = &last(); p1 < p2; p1++, p2--) {
      const T tmp = *p1; *p1 = *p2; *p2 = tmp;
    }
    m_updateCount++;
    return *this;
  }

  bool generateAllPermuations(PermutationHandler &handler) {
    return permuter(size(), handler);
  }

  // if capacity < 0, it's left unchanged
  CompactArray<T> &clear(intptr_t capacity=0) {
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

  intptr_t binaryInsert(const T &key, Comparator<T> &cmp) {
    intptr_t l = 0, r = size();
    while(l < r) {
      const intptr_t m = (l+r)/2;
      if(cmp.compare(m_array[m], key) < 0) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    add(r, key);
    return r;
  }

  intptr_t binaryInsert(const T &key, int (*cmp)(const T &e1, const T &e2)) {
    return binaryInsert(key, FunctionComparator<T>(cmp));
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

  ULONG hashCode() const {
    ULONG sum = 0;
    size_t count = size();
    for(const T *p = m_array; count--;) {
      sum = sum * 31 + (p++)->hashCode();
    }
    return sum;
  }

  inline bool isEmpty() const {
    return m_size == 0;
  };

  inline size_t size() const {
    return m_size;
  }

  inline const T *getBuffer() const { // can return NULL
    return m_array;
  }

  void save(ByteOutputStream &s) const { // modify this => take care of CompactFileArray
    const UINT64 size64 = m_size;
    s.putBytes((BYTE*)&size64, sizeof(size64));
    if(m_size) {
      s.putBytes((BYTE*)m_array, sizeof(T)*m_size);
    }
  }
  void load(ByteInputStream &s) {
    clear();
    UINT64 size64;
    s.getBytesForced((BYTE*)&size64, sizeof(size64));
    CHECKUINT64ISVALIDSIZET(size64)
    setCapacity((size_t)size64);
    if(size64) {
      s.getBytesForced((BYTE*)m_array, sizeof(T)*(size_t)size64);
    }
    m_size = (size_t)size64;
  }

  String toString(const TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    if(m_size) {
      const T *p = m_array;
      result += (p++)->toString();
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += (p++)->toString();
      }
    }
    result += _T(")");
    return result;
  }

  String toStringPointerType(const TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    if(m_size) {
      const T *p = m_array;
      result += (*(p++))->toString();
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += (*(p++))->toString();
      }
    }
    result += _T(")");
    return result;
  }

  String toStringBasicType(const TCHAR *delimiter = _T(",")) const {
    String result = _T("(");
    if(m_size) {
      const T *p = m_array;
      result += ::toString(*(p++));
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += ::toString(*(p++));
      }
    }
    result += _T(")");
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
    AbstractIterator *clone()       override {
      return new CompactArrayIterator(*this);
    }
    inline bool hasNext()     const override {
      return m_next < m_a.size();
    }
    void *next()                    override {
      if(m_next >= m_a.size()) {
        noNextElementError(_compactArrayIteratorClassName);
      }
      checkUpdateCount();
      m_current = m_next++;
      return &m_a[m_current];
    }
    void remove()                   override {
      if(m_current < 0) {
        noCurrentElementError(_compactArrayIteratorClassName);
      }
      checkUpdateCount();
      m_a.remove(m_current,1);
      m_current     = -1;
      m_updateCount = m_a.getUpdateCount();
    }
  };
  inline Iterator<T> getIterator() {
    return Iterator<T>(new CompactArrayIterator(this));
  }
  inline T *begin() {
    return isEmpty() ? NULL : &first();
  }
  inline T *end() {
    return isEmpty() ? NULL : (&first() + size());
  }
  inline const T *begin() const {
    return isEmpty() ? NULL : &first();
  }
  inline const T *end() const {
    return isEmpty() ? NULL : (&first() + size());
  }
};

template<typename S, typename T, class D=StreamDelimiter> S &operator<<(S &out, const CompactArray<T> &a) {
  const D      delimiter;
  const UINT   elemSize = sizeof(T);
  const size_t n        = a.size();
  out << elemSize << delimiter << n << delimiter;
  if(n) {
    for(T e : a) {
      out << e << delimiter;
    }
  }
  return out;
}

template<typename S, typename T> S &operator>>(S &in, CompactArray<T> &a) {
  UINT elemSize;
  in >> elemSize;
  if(elemSize != sizeof(T)) {
    throwException(_T("Invalid element size:%u bytes. Expected %zu bytes"), elemSize, sizeof(T));
  }
  a.clear();
  UINT64 size64;
  in >> size64;
  CHECKUINT64ISVALIDSIZET(size64)
  a.setCapacity((size_t)size64);
  for(size_t i = (size_t)size64; i--;) {
    T e;
    in >> e;
    a.add(e);
  }
  return in;
}

typedef CompactArray<TCHAR*> CompactStrArray;
typedef CompactArray<char>   CompactCharArray;
typedef CompactArray<short>  CompactShortArray;
typedef CompactArray<USHORT> CompactUshortArray;
typedef CompactArray<int>    CompactIntArray;
typedef CompactArray<UINT>   CompactUintArray;
typedef CompactArray<long>   CompactLongArray;
typedef CompactArray<ULONG>  CompactUlongArray;
typedef CompactArray<INT64>  CompactInt64Array;
typedef CompactArray<UINT64> CompactUint64Array;
typedef CompactArray<float>  CompactFloatArray;
typedef CompactArray<double> CompactDoubleArray;
typedef CompactArray<size_t> CompactSizetArray;

template <typename T> class CompactFileArray {
private:
  mutable ByteInputFile  m_f;
  const UINT64           m_dataStartOffset;
  size_t                 m_size;

  void indexError(const TCHAR *method, size_t index) const {
    throwIndexOutOfRangeException(method, index, m_size);
  }

public:
  CompactFileArray(const String &fileName, UINT64 startOffset)
    : m_dataStartOffset(startOffset + sizeof(UINT64))
    , m_f(fileName)
  {
    m_f.seek(startOffset);
    UINT64 size64;
    m_f.getBytesForced((BYTE*)&size64, sizeof(size64));
    CHECKUINT64ISVALIDSIZET(size64);
    m_size = (size_t)size64;
  }
  CompactFileArray(const CompactFileArray &src) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  CompactFileArray &operator=(const CompactFileArray &src) {
    throwUnsupportedOperationException(__TFUNCTION__);
    return *this;
  }

  inline size_t size() const {
    return m_size;
  }

  T operator[](size_t index) const {
    if(index >= m_size) indexError(__TFUNCTION__, index);

    m_f.seek(m_dataStartOffset + index * sizeof(T));
    T result;
    m_f.getBytesForced((BYTE*)&result, sizeof(T));
    return result;
  }
};
