#pragma once

#include "Iterator.h"

extern const char *_compactArrayIteratorClassName;

template <typename T> class CompactArray {
private:
  size_t  m_capacity;
  size_t  m_size;
  size_t  m_updateCount;
  T      *m_array;

  void indexError(const char *method, size_t index) const {
    throwIndexOutOfRangeException(method, index, m_size);
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

  void setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) return;
    T *newArray = capacity ? new T[capacity] : NULL; TRACE_NEW(newArray);
    if(m_size) {
      __assume(m_size  );
      if(newArray == NULL) {
        throwException("new failed");
      }
      __assume(newArray);
      memcpy(newArray, m_array, m_size * sizeof(T));
    }
    SAFEDELETEARRAY(m_array);
    m_array    = newArray;
    m_capacity = capacity;
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  inline T &operator[](size_t index) {
    if(index >= m_size) indexError(__FUNCTION__, index);
    return m_array[index];
  }

  inline const T &operator[](size_t index) const {
    if(index >= m_size) indexError(__FUNCTION__, index);
    return m_array[index];
  }

  inline T &first() {
    if(m_size == 0) indexError(__FUNCTION__, 0);
    return m_array[0];
  }

  inline const T &first() const {
    if(m_size == 0) indexError(__FUNCTION__, 0);
    return m_array[0];
  }

  inline T &last() {
    if(m_size == 0) indexError(__FUNCTION__, m_size);
    return m_array[m_size-1];
  }

  inline const T &last() const {
    if(m_size == 0) indexError(__FUNCTION__, m_size);
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
    if(index > m_size) indexError(__FUNCTION__, index);
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
    if(index > m_size) indexError(__FUNCTION__, index);
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
    if(j > m_size) indexError(format("%s(%s,%s)", __FUNCTION__,format1000(index).c_str(), format1000(count).c_str()).c_str(),j);
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
    if(m_size == 0) indexError(__FUNCTION__, m_size);
    remove(m_size-1);
  }

  CompactArray<T> &swap(size_t i1, size_t i2) {
    if(i1 >= m_size) indexError(__FUNCTION__, i1);
    if(i2 >= m_size) indexError(__FUNCTION__, i2);
    const T tmp = m_array[i1];
    m_array[i1] = m_array[i2];
    m_array[i2] = tmp;
    m_updateCount++;
    return *this;
  }


public:

  CompactArray<T> &reverse() {
    if(size() == 0) return *this;
    for (T *p1 = &first(), *p2 = &last(); p1 < p2; p1++, p2--) {
      const T tmp = *p1; *p1 = *p2; *p2 = tmp;
    }
    m_updateCount++;
    return *this;
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

  string toString(const char *delimiter = ",") const {
    string result = "(";
    if(m_size) {
      const T *p = m_array;
      result += (p++)->toString();
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += (p++)->toString();
      }
    }
    result += ")";
    return result;
  }

  string toStringPointerType(const char *delimiter = ",") const {
    string result = "(";
    if(m_size) {
      const T *p = m_array;
      result += (*(p++))->toString();
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += (*(p++))->toString();
      }
    }
    result += ")";
    return result;
  }

  string toStringBasicType(const char *delimiter = ",") const {
    string result = "(";
    if(m_size) {
      const T *p = m_array;
      result += ::toString(*(p++));
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += ::toString(*(p++));
      }
    }
    result += ")";
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
typedef CompactArray<char>   CompaccharArray;
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

