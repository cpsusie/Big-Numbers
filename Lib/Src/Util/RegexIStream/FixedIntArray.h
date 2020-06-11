#pragma once

#include <CompactArray.h>
#include <NumberInterval.h>

#pragma pack(push,4)
class FixedIntArray {
protected:
  const UINT m_size          : 30;
  const UINT m_itemSizeShift : 2;
  inline  FixedIntArray(UINT size, BYTE itemSize) : m_size(size), m_itemSizeShift(getItemSizeShift(itemSize)) {
    assert(m_size == size);
  }
  inline FixedIntArray(const FixedIntArray &src) : m_size(src.m_size), m_itemSizeShift(src.m_itemSizeShift) {
  }
  FixedIntArray &operator=(const FixedIntArray &src); // not implemented
  static BYTE getItemSizeShift(BYTE itemSize);

public:
  virtual FixedIntArray *clone()        const = 0;
  virtual ~FixedIntArray() {
  }
  virtual int    operator[](UINT index) const = 0;
  virtual UINT   countNonZeroes()       const = 0;
  inline  BYTE   getItemSize()          const {
    return 1 << m_itemSizeShift;
  }
  virtual size_t getMemoryUsage()       const = 0; // in bytes
  inline  UINT   size()                 const {
    return m_size;
  }
  inline  bool   isEmpty()              const {
    return m_size == 0;
  }
  inline  size_t getBufferBytes()       const { // total memoryusage of array-items in bytes
    return size() * getItemSize();
  }
  String         toString(UINT maxPerLine) const;
  static FixedIntArray *allocateFixedArray(const CompactIntArray &values);
};
#pragma pack(pop)