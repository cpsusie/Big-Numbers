#pragma once

#include "ByteFile.h"
#include "Comparator.h"
#include "CollectionBase.h"
#include "Random.h"

template <typename T> class CompactArray : public CollectionBase<T> {
private:
  size_t  m_capacity;
  size_t  m_size;
  size_t  m_updateCount;
  T      *m_array;

  void indexError(const TCHAR *method, size_t index) const {
    throwIndexOutOfRangeException(method, index, m_size);
  }
  void indexError(const TCHAR *method, size_t index, size_t count) const {
    throwIndexOutOfRangeException(method, index, count, m_size);
  }
  static void emptyArrayError(const TCHAR *method) {
    throwEmptyArrayException(method);
  }

  size_t getSortCount(size_t from, size_t count) const {
    const intptr_t length = size() - from;
    return min((intptr_t)count, length);
  }

  void init(size_t capacity, size_t size) {
    m_size        = size;
    m_updateCount = 0;
    m_capacity    = capacity;
    m_array       = m_capacity ? new T[m_capacity] : nullptr; TRACE_NEW(m_array);
  }

  void setCapacityNoCopy(size_t capacity) {
    if(capacity == m_capacity) {
      return;
    }
    SAFEDELETEARRAY(m_array);
    m_array    = capacity ? new T[capacity] : nullptr; TRACE_NEW(m_array);
    m_capacity = capacity;
  }

public:
  CompactArray() {
    init(0, 0);
  }

  explicit CompactArray(size_t capacity) {
    init(capacity, 0);
  }

  CompactArray(const CompactArray &src) {
    init(src.m_size, src.m_size);
    if(m_size) {
      memcpy(m_array, src.m_array, m_size * sizeof(T));
    }
  }

  CompactArray &operator=(const CompactArray &src) {
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

  ~CompactArray() override {
    SAFEDELETEARRAY(m_array);
  }

  inline size_t getCapacity() const {
    return m_capacity;
  }

  // Return *this
  CompactArray &setCapacity(size_t capacity) {
    if(capacity < m_size) {
      capacity = m_size;
    }
    if(capacity == m_capacity) {
      return *this;
    }
    T *newArray = capacity ? new T[capacity] : nullptr; TRACE_NEW(newArray);
    if(m_size) {
      __assume(m_size  );
      if(newArray == nullptr) {
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


  // If capacity < 0, it's left unchanged
  // Return *this
  CompactArray &clear(intptr_t capacity) {
    if(m_size != 0) {
      m_updateCount++;
    }
    m_size = 0;
    if(capacity >= 0) {
      setCapacity(capacity);
    }
    return *this;
  }

  void clear() override {
    clear(0);
  }

  size_t size() const override {
    return m_size;
  }

  inline T &operator[](size_t index) {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_array[index];
  }

  inline const T &operator[](size_t index) const {
    if(index >= m_size) indexError(__TFUNCTION__, index);
    return m_array[index];
  }

  inline T &first() {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_array[0];
  }

  inline const T &first() const {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_array[0];
  }

  inline T &last() {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_array[m_size-1];
  }

  inline const T &last() const {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_array[m_size-1];
  }

  bool operator==(const CompactArray &a) const {
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

  inline bool operator!=(const CompactArray &a) const {
    return !(*this == a);
  }

  // Return min(k) so (*this)[k] == e, if such k exist, or else return -1
  intptr_t getFirstIndex(const T &e) const {
    if(isEmpty()) {
      return -1;
    }
    for(const T *p = begin(), *endp = end(); p < endp;) {
      if(*(p++) == e) {
        return p - m_array - 1;
      }
    }
    return -1;
  }

  inline bool contains(const T &e) const {
    return getFirstIndex(e) >= 0;
  }

  inline const T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_array[randSizet(m_size,rnd)];
  }

  inline T &select(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_array[randSizet(m_size,rnd)];
  }

  CompactArray getRandomSample(size_t k, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) const {
    if(k > m_size) {
      throwInvalidArgumentException(__TFUNCTION__, _T("k(=%s) > size(=%s)")
                                   ,format1000(k).cstr()
                                   ,format1000(m_size).cstr());
    }
    CompactArray result(k);
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

  // Insert the same element e position [index..index+count-1]
  // Return *this
  CompactArray &insert(size_t index, const T &e, size_t count = 1) {
    if(index > m_size) indexError(__TFUNCTION__, index);
    if(count == 0) {
      return *this;
    }
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
    return *this;
  }

  // Insert the elements e[0..count-1] at position [index..index+count-1]
  // Return *this
  CompactArray &insert(size_t index, const T *ep, size_t count) {
    if(index > m_size) indexError(__TFUNCTION__, index);
    if(count == 0) {
      return *this;
    }
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
    return *this;
  }

  bool add(const T &e) override {
    if(m_size == m_capacity) {
      setCapacity(3*m_capacity+5);
    }
    m_array[m_size++] = e;
    m_updateCount++;
    return true;
  }

  void append(const T *ep, size_t count) {
    insert(size(), ep, count);
  }

  bool addAll(const CompactArray &src) {
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
    if(j > m_size) {
      indexError(__TFUNCTION__, index, count);
    }
    if(j < m_size) {
      memmove(m_array+index, m_array+j, (m_size-j) * sizeof(T));
    }
    m_size -= count;
    if(m_size < m_capacity/4) {
      setCapacity(m_size);
    }
    m_updateCount++;
  }

  // Return *this
  CompactArray &removeLast() {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    remove(m_size-1);
    return *this;
  }

  // Return *this
  CompactArray &swap(size_t i1, size_t i2) {
    if(i1 >= m_size) indexError(__TFUNCTION__, i1);
    if(i2 >= m_size) indexError(__TFUNCTION__, i2);
    const T tmp = m_array[i1];
    m_array[i1] = m_array[i2];
    m_array[i2] = tmp;
    m_updateCount++;
    return *this;
  }

  // Return *this
  CompactArray &shuffle(size_t from, size_t count, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    if(from >= size()) {
      return *this;
    }
    if((count = getSortCount(from, count)) > 1) {
      for(size_t i = 0; i < count; i++ ) {
        swap(from + i, from + randSizet(count,rnd));
      }
    }
    m_updateCount++;
    return *this;
  }

  // Return *this
  CompactArray &shuffle() {
    return shuffle(0, size());
  }

  // Return *this
  CompactArray &reverse() {
    if(size() == 0) {
      return *this;
    }
    for(T *p1 = &first(), *p2 = &last(); p1 < p2; p1++, p2--) {
      const T tmp = *p1; *p1 = *p2; *p2 = tmp;
    }
    m_updateCount++;
    return *this;
  }

  class PermutationHandler {
    public:
      virtual bool handlePermutation(const CompactArray &a) = 0;
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
  CompactArray &sort(size_t from, size_t count, Comparator<T> &comparator) {
    if(from >= size()) {
      return *this;
    }
    if((count = getSortCount(from, count)) > 1) {
      quickSort(m_array+from, count, sizeof(T), comparator);
      m_updateCount++;
    }
    return *this;
  }

  // Return *this
  CompactArray &sort(Comparator<T> &comparator) {
    return sort(0, size(), comparator);
  }

  // Return *this
  CompactArray &sort(size_t from, size_t count, int (*compare)(const T &e1, const T &e2)) {
    return sort(from, count, FunctionComparator<T>(compare));
  }

  // Return *this
  CompactArray &sort(int (*compare)(const T &e1, const T &e2)) {
    return sort(0, size(), compare);
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
    insert(r, key);
    return r;
  }

  intptr_t binaryInsert(const T &key, int (*cmp)(const T &e1, const T &e2)) {
    return binaryInsert(key, FunctionComparator<T>(cmp));
  }

  ULONG hashCode() const {
    ULONG sum = 0;
    size_t count = size();
    for(const T *p = m_array; count--;) {
      sum = sum * 31 + (p++)->hashCode();
    }
    return sum;
  }

  inline const T *getBuffer() const { // can return nullptr
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

  String toStringPointerType(const TCHAR *delimiter = _T(","), BracketType bracketType = BT_ROUNDBRACKETS) const {
    String result;
    result.addStartBracket(bracketType);
    if(m_size) {
      const T *p = m_array;
      result += (*(p++))->toString();
      for(const T *end = &last(); p <= end;) {
        result += delimiter;
        result += (*(p++))->toString();
      }
    }
    result.addEndBracket(bracketType);
    return result;
  }

  inline size_t getUpdateCount() const {
    return m_updateCount;
  }

  class CompactArrayIterator : public AbstractIterator {
  private:
    CompactArray    &m_a;
    size_t           m_next;
    intptr_t         m_current;
    size_t           m_updateCount;
    inline void checkUpdateCount(const TCHAR *method) const {
      if(m_updateCount != m_a.getUpdateCount()) {
        concurrentModificationError(method);
      }
    }
  public:
    CompactArrayIterator(const CompactArray *a) : m_a(*(CompactArray*)a) {
      m_next        = 0;
      m_current     = -1;
      m_updateCount = m_a.getUpdateCount();
    }
    AbstractIterator *clone()          final {
      return new CompactArrayIterator(*this);
    }
    inline bool hasNext()        const final {
      return m_next < m_a.size();
    }
    void *next()                       final {
      if(m_next >= m_a.size()) {
        noNextElementError(__TFUNCTION__);
      }
      checkUpdateCount(__TFUNCTION__);
      m_current = m_next++;
      return &m_a[m_current];
    }
    void remove()                      final {
      if(m_current < 0) {
        noCurrentElementError(__TFUNCTION__);
      }
      checkUpdateCount(__TFUNCTION__);
      m_a.remove(m_current,1);
      m_current     = -1;
      m_updateCount = m_a.getUpdateCount();
    }
  };
  ConstIterator<T> getIterator() const override {
    return ConstIterator<T>(new CompactArrayIterator(this));
  }
  Iterator<T>      getIterator()       override {
    return Iterator<T>(new CompactArrayIterator(this));
  }
  bool             hasOrder()    const final {
    return true;
  }
  inline T        *begin() {
    return isEmpty() ? nullptr : &first();
  }
  inline T        *end() {
    return isEmpty() ? nullptr : (&first() + size());
  }
  inline const T  *begin()       const {
    return isEmpty() ? nullptr : &first();
  }
  inline const T  *end()         const {
    return isEmpty() ? nullptr : (&first() + size());
  }
};

typedef CompactArray<TCHAR*> CompactStrArray;
typedef CompactArray<CHAR  > CompactCharArray;
typedef CompactArray<SHORT > CompactShortArray;
typedef CompactArray<USHORT> CompactUShortArray;
typedef CompactArray<INT   > CompactIntArray;
typedef CompactArray<UINT  > CompactUIntArray;
typedef CompactArray<LONG  > CompactLongArray;
typedef CompactArray<ULONG > CompactULongArray;
typedef CompactArray<INT64 > CompactInt64Array;
typedef CompactArray<UINT64> CompactUInt64Array;
typedef CompactArray<FLOAT > CompactFloatArray;
typedef CompactArray<DOUBLE> CompactDoubleArray;
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
