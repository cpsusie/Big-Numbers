#pragma once

#include <MyUtil.h>
#include <CompactArray.h>

class PackedArray {
private:
  UINT               m_bitsPerItem;
  UINT               m_maxValue;
  CompactArray<UINT> m_data;
  UINT64             m_firstFreeBit;
  void assertHasOneFreeItem() {
    if(m_data.size() * 32 < m_firstFreeBit + m_bitsPerItem) m_data.add(0);
  }

  static void validateBitsPerItem(UINT bitsPerItem);
#ifdef _DEBUG
  void indexError(UINT64 index, const TCHAR *method=_T("")) const {
    throwInvalidArgumentException(method, _T("Index %I64u out of range. size=%I64u"), index, size());
  }
  void valueError(UINT v, const TCHAR *method=_T("")) const {
    throwInvalidArgumentException(method, _T("v=%lu, maxValue=%lu"), v, m_maxValue);
  }
  static void selectError() {
    throwException(_T("%s:Cannot select from empty array"), __TFUNCTION__);
  }
#endif

public:
  explicit PackedArray(unsigned char bitsPerItem);
  UINT get(      UINT64 index) const;
  UINT select() const;
  void set(      UINT64 index, UINT v);
  void or(       UINT64 index, UINT v);
  void and(      UINT64 index, UINT v);
  void xor(      UINT64 index, UINT v);
  void add(                    UINT v);
  void add(      UINT64 index, UINT v);
  void addZeroes(UINT64 index, UINT64 count);
  void remove(   UINT64 index, UINT64 count = 1);
  PackedArray &clear();
  bool isEmpty() const {
    return m_firstFreeBit == 0;
  }
  UINT64 size() const {
    return m_firstFreeBit / m_bitsPerItem;
  }
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  bool operator==(const PackedArray &a) const;
  bool operator!=(const PackedArray &a) const {
    return !(*this == a);
  }
  UINT getBitsPerItem() const {
    return m_bitsPerItem;
  }
  UINT getMaxValue() const {
    return m_maxValue;
  }

  void setCapacity(UINT64 capacity);

#ifdef _DEBUG
  static bool trace;
  void dump(bool fixedPos = false) const;
  void markPointer(const void *p, int offset = 0) const;
  void markElement(int index) const;
  void markBit(int bit) const;
#endif
  String toString() const;
  void checkInvariant() const;

  friend class PackedFileArray;
};

class PackedFileArray { // Read-only packed array of integers accessed by seeking the file, instead of loading 
                        // the whole array into memory. Slow, but save space
                        // Bytes starting at startOffset must be written by PackedArray.save
private:
  unsigned char          m_bitsPerItem;
  UINT                   m_maxValue;
  CompactFileArray<UINT> m_data;
  UINT64                 m_firstFreeBit;

#ifdef _DEBUG
  void indexError(UINT64 index, const TCHAR *method=_T("")) const {
    throwInvalidArgumentException(method, _T("Index %I64u out of range. size=%I64u"), index, size());
  }
#endif
public:
  PackedFileArray(const String &fileName, UINT64 startOffset);
  UINT get(UINT64 index) const;
  UINT select() const;
  bool isEmpty() const {
    return m_firstFreeBit == 0;
  }
  UINT64 size() const {
    return m_firstFreeBit / m_bitsPerItem;
  }
  UINT getBitsPerItem() const {
    return m_bitsPerItem;
  }
  UINT getMaxValue() const {
    return m_maxValue;
  }
};
