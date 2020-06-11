#include "pch.h"
#include "FixedIntArray.h"
#include <limits.h>

static IntInterval getRange(const CompactIntArray &values) {
  int from = values[0], to = from;
  for(int v : values) {
    if(v > to) to = v; else if(v < from) from = v;
  }
  return IntInterval(from, to);
}

BYTE FixedIntArray::getItemSizeShift(BYTE itemSize) { // static
  switch(itemSize) {
  case sizeof(BYTE ) : return 0;
  case sizeof(SHORT) : return 1;
  case sizeof(INT  ) : return 2;
  default: throwInvalidArgumentException(__TFUNCTION__, _T("itemSize=%u"), itemSize);
  }
  return 0;
}

String FixedIntArray::toString(UINT maxPerLine) const {
  String result;
  const UINT sz = size();
  for(UINT i = 0, j = 1; i < sz; i++, j++) {
    if((sz > maxPerLine) && (j == 1)) {
      result += format(_T("(%3u) "), (UINT)i);
    }
    result += format(_T("%3d"), (*this)[i]);
    if(i < sz - 1) {
      result += _T(",");
    }
    if((j == maxPerLine) || (i == sz - 1)) {
      result += _T('\n');
      j = 0;
    }
  }
  return result;
}

template<typename T> class FixedIntArrayTemplate : public FixedIntArray {
protected:
  T *m_buffer;
  inline void allocateBuffer() {
    m_buffer = new T[m_size]; TRACE_NEW(m_buffer);
  }
  inline void deallocateBuffer() {
    SAFEDELETE(m_buffer);
  }

  FixedIntArrayTemplate(const FixedIntArrayTemplate<T> &src) : FixedIntArray(src) {
    allocateBuffer();
    memcpy(m_buffer, src.m_buffer, sizeof(T)*size());
  }
  FixedIntArrayTemplate(const CompactIntArray &values) : FixedIntArray((UINT)values.size(), sizeof(T)) {
    allocateBuffer();
  }
public:
  FixedIntArrayTemplate(const CompactIntArray &values, const IntInterval &range) : FixedIntArray((UINT)values.size(), sizeof(T)) {
    allocateBuffer();
    UINT index = 0;
    for(int v : values) {
      m_buffer[index++] = (T)v;
    }
  }
  size_t getMemoryUsage() const { // in bytes
    return sizeof(FixedIntArrayTemplate) + getBufferBytes();
  }
  FixedIntArray *clone() const {
    return new FixedIntArrayTemplate<T>(*this);
  }
  int operator[](UINT index) const {
    assert(index < m_size);
    return m_buffer[index];
  }
  UINT countNonZeroes() const {
    UINT count = 0;
    const UINT sz = size();
    for(UINT i = 0; i < sz; i++) {
      if((*this)[i] != 0) {
        count++;
      }
    }
    return count;
  }
  ~FixedIntArrayTemplate() {
    deallocateBuffer();
  }
};

template<typename T> class BiasedFixedIntArrayTemplate : public FixedIntArrayTemplate<T> {
private:
  int m_minValue;
  BiasedFixedIntArrayTemplate(const BiasedFixedIntArrayTemplate<T> &src) : FixedIntArrayTemplate(src) {
    m_minValue = src.m_minValue;
  }
public:
  BiasedFixedIntArrayTemplate(const CompactIntArray &values, const IntInterval &range) : FixedIntArrayTemplate(values) {
    m_minValue = range.getFrom();
    UINT index = 0;
    for(int v : values) {
      m_buffer[index++] = (T)(v - m_minValue);
    }
  }
  size_t getMemoryUsage() const { // in bytes
    return sizeof(BiasedFixedIntArrayTemplate) + getBufferBytes();
  }
  FixedIntArray *clone() const {
    return new BiasedFixedIntArrayTemplate<T>(*this);
  }
  int operator[](UINT index) const {
    assert(index < m_size);
    return m_minValue + m_buffer[index];
  }
};

FixedIntArray *FixedIntArray::allocateFixedArray(const CompactIntArray &values) { // static
  assert(!values.isEmpty());
  const IntInterval range       = getRange(values);
  const UINT        rangeLength = range.getLength();
  if(rangeLength <= UCHAR_MAX) {
    if(IntInterval(CHAR_MIN,CHAR_MAX).contains(range)) {
      return new FixedIntArrayTemplate<CHAR>(values, range);
    } else {
      return new BiasedFixedIntArrayTemplate<UCHAR>(values, range);
    }
  } else if(rangeLength < USHRT_MAX) {
    if(IntInterval(SHRT_MIN,SHRT_MAX).contains(range)) {
      return new FixedIntArrayTemplate<SHORT>(values, range);
    } else {
      return new BiasedFixedIntArrayTemplate<USHORT>(values, range);
    }
  } else {
    return new FixedIntArrayTemplate<INT>(values, range);
  }
}
