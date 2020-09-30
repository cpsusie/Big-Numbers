#include "stdafx.h"

// #define DUMPLISTALLOCATOR

#if defined(DUMPLISTALLOCATOR)
#include <MyUtil.h>
#endif // DUMPLISTALLOCATOR

#pragma check_stack(off)

class MergeList {
public:
  bool       m_used;
  ByteArray *m_buffer;

  MergeList(size_t capacity) {
    m_used = false;
    m_buffer = new ByteArray(capacity); TRACE_NEW(m_buffer);
  }
  inline void release() {
    m_used = false;
  }
};

class ListAllocator {
private:
  const size_t     m_elementSize;
  Array<MergeList> m_bufferArray;

  ListAllocator(const ListAllocator &src);            // Not defined. Class not cloneable
  ListAllocator &operator=(const ListAllocator &src); // Not defined. Class not cloneable

#if defined(DUMPLISTALLOCATOR)
  void dumpArray();
#endif // DUMPLISTALLOCATOR
public:
  ListAllocator(size_t elementSize) : m_elementSize(elementSize) {
  }
  ~ListAllocator();
  MergeList &fetchList(size_t minSize);
};

ListAllocator::~ListAllocator() {
#if defined(DUMPLISTALLOCATOR)
  dumpArray();
#endif // DUMPLISTALLOCATOR
  for(size_t i = 0; i < m_bufferArray.size(); i++) {
    SAFEDELETE(m_bufferArray[i].m_buffer);
  }
  m_bufferArray.clear();
}

#if defined(DUMPLISTALLOCATOR)
void ListAllocator::dumpArray() {
  FILE *f = FOPEN(_T("c:\\temp\\ListAllocator.txt"), _T("w"));
  for(size_t i = 0; i < m_bufferArray.size(); i++) {
    _ftprintf(f, _T("buffer[%u].capacity:%u\n"), (UINT)i, (UINT)m_bufferArray[i].m_buffer->getCapacity());
  }
  fclose(f);
}
#endif // DUMPLISTALLOCATOR

MergeList &ListAllocator::fetchList(size_t minSize) {
  const size_t neededCapacity = minSize * m_elementSize;
  intptr_t l = 0, r = m_bufferArray.size();
  while(l < r) {
    const intptr_t m = (l + r) / 2;
    MergeList &ml = m_bufferArray[m];
    if(ml.m_buffer->getCapacity() < neededCapacity) {
      l = m + 1;
    } else if(!ml.m_used) {
      ml.m_used = true;
      return ml;
    } else {
      r = m;
    }
  }
  if(r == m_bufferArray.size() || m_bufferArray[r].m_used) {
    m_bufferArray.insert(r, MergeList(neededCapacity));
  }
  MergeList &result = m_bufferArray[r];
  result.m_used = true;
  return result;
}

class MergeSortAnyWidth : private ListAllocator {
private:
  const size_t        m_width;
  AbstractComparator &m_comparator;

  void merge(char *dst, char *l1, size_t n1, char *l2, size_t n2);

public:
  MergeSortAnyWidth(size_t width, AbstractComparator &comparator) : m_width(width), m_comparator(comparator), ListAllocator(width) {
  }
  void sort(void *list, size_t n);
};


#define COMBINEDSORT

#if defined(COMBINEDSORT)

//#define MAKEPLOT

#if defined(MAKEPLOT)
int MergeMinSize = 120;
#else
#define MergeMinSize 300
#endif

#endif

void MergeSortAnyWidth::sort(void *list, size_t n) {
#if defined(COMBINEDSORT)
  if(n < MergeMinSize) {
    quickSort5OptimalPivot5(list, n, m_width, m_comparator);
    return;
  }
#else
  if(n <= 1) {
    return;
  }
#endif
  const size_t n1 = n / 2;
  const size_t n2 = n - n1;

  MergeList &first  = fetchList(n1);
  MergeList &second = fetchList(n2);
  char *l1 = (char*)first.m_buffer->getData();
  char *l2 = (char*)second.m_buffer->getData();
  memcpy(l1 , list, n1*m_width);
  memcpy(l2, (char*)list+n1*m_width, n2*m_width);

  sort(l1, n1);
  sort(l2, n2);

  merge((char*)list, l1, n1, l2, n2);
  first.release();
  second.release();
}

void MergeSortAnyWidth::merge(char *dst, char *l1, size_t n1, char *l2, size_t n2) {
  while(n1 && n2) {
    if(m_comparator.cmp(l1, l2) < 0) {
      memcpy(dst, l1, m_width);
      l1 += m_width;
      n1--;
    } else {
      memcpy(dst, l2, m_width);
      l2 += m_width;
      n2--;
    }
    dst += m_width;
  }
  if(n1) {
    memcpy(dst, l1, n1*m_width);
  } else if(n2) {
    memcpy(dst, l2, n2*m_width);
  }
}

template <class T> class MergeSortClass : private ListAllocator {
private:
  AbstractComparator &m_comparator;

  void merge(T *dst, T *l1, size_t n1, T *l2, size_t n2);

public:
  MergeSortClass<T>(AbstractComparator &comparator) : m_comparator(comparator), ListAllocator(sizeof(T)) {
  }
  void sort(T *list, size_t n);
};

template <class T> void MergeSortClass<T>::sort(T *list, size_t n) {
#if defined(COMBINEDSORT)
  if(n < MergeMinSize) {
    quickSort5OptimalPivot5(list, n, sizeof(T), m_comparator);
    return;
  }
#else
  if(n <= 1) {
    return;
  }
#endif

  const size_t n1 = n / 2;
  const size_t n2 = n - n1;
  MergeList &first  = fetchList(n1);
  MergeList &second = fetchList(n2);
  T *l1 = (T*)first.m_buffer->getData();
  T *l2 = (T*)second.m_buffer->getData();
  memcpy(l1 , list   , n1*sizeof(T));
  memcpy(l2,  list+n1, n2*sizeof(T));

  sort(l1, n1);
  sort(l2, n2);

  merge(list, l1, n1, l2, n2);
  first.release();
  second.release();
}

template <class T> void MergeSortClass<T>::merge(T *dst, T *l1, size_t n1, T *l2, size_t n2) {
  while(n1 && n2) {
    if(m_comparator.cmp(l1, l2) < 0) {
      *(dst++) = *(l1++);
      n1--;
    } else {
      *(dst++) = *(l2++);
      n2--;
    }
  }
  if(n1) {
    memcpy(dst, l1, n1*sizeof(T));
  } else if(n2) {
    memcpy(dst, l2, n2*sizeof(T));
  }
}

void mergeSort(void *base, size_t nelem, size_t width, AbstractComparator &comparator) {
  switch(width) {
  case sizeof(char)  :
    { MergeSortClass<char>(comparator).sort((char*)base, nelem);
      break;
    }
  case sizeof(short) :
    { MergeSortClass<short>(comparator).sort((short*)base, nelem);
      break;
    }
  case sizeof(long)  : // include pointertypes
    { MergeSortClass<long>(comparator).sort((long*)base, nelem);
      break;
    }
  case sizeof(__int64):
    { MergeSortClass<__int64>(comparator).sort((__int64*)base, nelem);
      break;
    }
  default            :
    { MergeSortAnyWidth(width, comparator).sort(base, nelem);
      break;
    }
  }
}

#if defined(COMBINEDSORT)

#if defined(MAKEPLOT)

class IntComparator : public AbstractComparator {
public:
  int cmp(const void *e1, const void *e2) {
    return *(int*)e1 - *(int*)e2;
  }
  AbstractComparator *clone() const {
    return new IntComparator();
  }
};

void makeMinSizeTimePlot() {
  FILE *f = FOPEN(_T("c:\\temp\\MergeMinSizeTimePlot.txt"), _T("w"));
  CompactIntArray copy;
  Random rnd;
  for(int i = 0; i < 1500000; i++) {
    copy.add(rnd.nextInt());
  }
  for(MergeMinSize = 10; MergeMinSize < 600; MergeMinSize += 3) {
    CompactIntArray a = copy;
    const double startTime = getThreadTime();
    mergeSort((void*)a.getBuffer(), a.size(), sizeof(int), IntComparator());
    const double sortTime = getThreadTime() - startTime;
    _ftprintf(f, _T("%3d %le\n"), MergeMinSize, sortTime/1000000);
  }
  fclose(f);
}
#endif

#endif
