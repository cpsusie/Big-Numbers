#pragma once

#include "CompactArray.h"

class PackedArray {
private:
  UINT               m_bitsPerItem;
  UINT               m_maxValue;
  CompactArray<UINT> m_data;
  UINT64             m_firstFreeBit;
  size_t             m_updateCount;

  void assertHasOneFreeItem() {
    if(m_data.size() * 32 < m_firstFreeBit + m_bitsPerItem) m_data.add(0);
  }

  static void validateBitsPerItem(const TCHAR *method, UINT bitsPerItem);
  void indexError(const TCHAR *method, UINT64 index) const {
    throwIndexOutOfRangeException(method, index, size());
  }
  void valueError(const TCHAR *method, UINT v) const {
    throwInvalidArgumentException(method, _T("v=%lu, maxValue=%lu"), v, m_maxValue);
  }
  void indexError(const TCHAR *method, UINT64 index, UINT64 count) const {
    throwIndexOutOfRangeException(method, index, count, size());
  }
  void selectError(const TCHAR *method) const {
    throwSelectFromEmptyCollectionException(method);
  }

public:
  explicit PackedArray(BYTE bitsPerItem);
  virtual ~PackedArray() {
    clear();
  }
  UINT get(      UINT64 index) const;
  UINT select(RandomGenerator *rnd = _standardRandomGenerator) const;
  void set(      UINT64 index, UINT v);
  void or(       UINT64 index, UINT v);
  void and(      UINT64 index, UINT v);
  void xor(      UINT64 index, UINT v);
  void add(                    UINT v);
  void add(      UINT64 index, UINT v);
  void addZeroes(UINT64 index, UINT64 count);
  void remove(   UINT64 index, UINT64 count = 1);
  PackedArray &clear();
  inline bool isEmpty() const {
    return m_firstFreeBit == 0;
  }
  inline UINT64 size() const {
    return m_firstFreeBit / m_bitsPerItem;
  }
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  bool operator==(const PackedArray &a) const;
  inline bool operator!=(const PackedArray &a) const {
    return !(*this == a);
  }
  inline UINT getBitsPerItem() const {
    return m_bitsPerItem;
  }
  inline UINT getMaxValue() const {
    return m_maxValue;
  }

  void setCapacity(UINT64 capacity);

  Iterator<UINT> getIterator();
  Iterator<UINT> getReverseIterator();

  String toString(const TCHAR *delimiter = _T(",")) const {
    return ((PackedArray*)this)->getIterator().toString(UIntStringifier(), delimiter);
  }
  void checkInvariant(const TCHAR *method) const;

  inline size_t getUpdateCount() const {
    return m_updateCount;
  }

  friend class PackedArrayIterator;
  friend class PackedArrayReverseIterator;
  friend class PackedFileArray;

#ifdef _DEBUG_PACKEDARRAY
  static bool s_trace;
  void dump(bool fixedPos = false) const;
  void markPointer(const void *p, int offset = 0) const;
  void markElement(int index) const;
  void markBit(int bit) const;
  String toDebugString() const;
#endif // _DEBUG_PACKEDARRAY
};

class PackedFileArray { // Read-only packed array of integers accessed by seeking the file, instead of loading
                        // the whole array into memory. Slow, but save space
                        // Bytes starting at startOffset must be written by PackedArray.save
private:
  UINT                   m_bitsPerItem;
  UINT                   m_maxValue;
  CompactFileArray<UINT> m_data;
  UINT64                 m_firstFreeBit;
  UINT64                 m_size;
  void indexError(const TCHAR *method, UINT64 index) const {
    throwIndexOutOfRangeException(method, index, size());
  }
  void selectError(const TCHAR *method) const {
    throwSelectFromEmptyCollectionException(method);
  }
  void checkInvariant(const TCHAR *method) const;
public:
  PackedFileArray(const String &fileName, UINT64 startOffset);
  UINT get(UINT64 index) const;
  inline UINT select(RandomGenerator *rnd = _standardRandomGenerator) const {
    if(isEmpty()) selectError(__TFUNCTION__);
    return get(rnd->nextInt64(size()));
  }
  inline bool isEmpty() const {
    return size() == 0;
  }
  inline UINT64 size() const {
    return m_size;
  }
  inline UINT getBitsPerItem() const {
    return m_bitsPerItem;
  }
  inline UINT getMaxValue() const {
    return m_maxValue;
  }
};
