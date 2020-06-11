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
private:
  T   *m_buffer;
  int  m_minValue;
  FixedIntArrayTemplate(const FixedIntArrayTemplate<T> &src) : FixedIntArray(src) {
    m_minValue = src.m_minValue;
    m_buffer = new T[m_size]; TRACE_NEW(m_buffer);
    memcpy(m_buffer, src.m_buffer, sizeof(T)*size());
  }
public:
  FixedIntArrayTemplate(const CompactIntArray &values, const IntInterval &range)
    : FixedIntArray((UINT)values.size(), sizeof(T)) {
    m_minValue = range.getFrom();
    m_buffer = new T[m_size]; TRACE_NEW(m_buffer);
    UINT index = 0;
    for(int v : values) {
      m_buffer[index++] = (T)(v - m_minValue);
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
    return m_minValue + m_buffer[index];
  }
  inline const T *begin() const {
    return m_buffer;
  }
  inline const T *end() const {
    return m_buffer + m_size;
  }
  UINT countNonZeroes() const {
    UINT count = 0;
    for(const T v : *this) if(v) count++;
    return count;
  }
  ~FixedIntArrayTemplate() {
    SAFEDELETE(m_buffer);
  }
};

FixedIntArray *FixedIntArray::allocateFixedArray(const CompactIntArray &values) { // static
  assert(!values.isEmpty());
  const IntInterval range       = getRange(values);
  const UINT        rangeLength = range.getLength();
  if(rangeLength <= UCHAR_MAX) {
    return new FixedIntArrayTemplate<char >(values, range);
  } else if(rangeLength < USHRT_MAX) {
    return new FixedIntArrayTemplate<short>(values, range);
  } else {
    return new FixedIntArrayTemplate<int  >(values, range);
  }
}
