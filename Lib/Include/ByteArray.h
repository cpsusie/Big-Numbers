#pragma once

#include "MyString.h"

class ByteInputStream;
class ByteOutputStream;

class ByteArray {
private:
  BYTE    *m_data;
  size_t   m_capacity;
  size_t   m_size;
  size_t   m_updateCount;

  void cleanup();
  void indexError(const TCHAR *method, size_t index) const;
  void indexError(const TCHAR *method, size_t index, size_t count) const;
  static void emptyArrayError(const TCHAR *method);
protected:
  void init();
  virtual BYTE  *allocateBytes( size_t size);
  virtual void   deallocateBytes(BYTE *buffer);
  virtual size_t getCapacityCeil(size_t capacity) const {
    return capacity;
  }
public:
  ByteArray();
  explicit ByteArray(size_t capacity);
  ByteArray(const BYTE *data, size_t size);
  ByteArray(const ByteArray &src);
  ByteArray &operator=( const ByteArray &src);
  virtual ~ByteArray();

  inline size_t getCapacity() const {
    return m_capacity;
  }
  // Return *this
  ByteArray &setCapacity(size_t capacity);

  // If newCapcity == -1 it will not be changed
  // Return *this
  ByteArray &clear(intptr_t newCapacity = 0);

  inline size_t size() const {
    return m_size;
  }

  inline bool isEmpty() const {
    return m_size == 0;
  }

  inline const BYTE &operator[](size_t i) const {
    if(i >= m_size) indexError(__TFUNCTION__, i);
    return m_data[i];
  }

  inline BYTE &operator[](size_t i) {
    if(i >= m_size) indexError(__TFUNCTION__, i);
    return m_data[i];
  }

  inline BYTE &first() {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_data[0];
  }

  inline const BYTE &first() const {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_data[0];
  }

  inline BYTE &last() {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_data[m_size-1];
  }

  inline const BYTE &last() const {
    if(isEmpty()) emptyArrayError(__TFUNCTION__);
    return m_data[m_size-1];
  }

  bool operator==(const ByteArray &a) const;
  bool operator!=(const ByteArray &a) const;

  // Insert count bytes (value=b) at position [index..index+count-1]
  // Assume index <= size
  // Return *this
  ByteArray &insert(size_t index, BYTE b, size_t count = 1);

  // Insert bp[0..count-1] at position [index..index+count-1], setting new arraysize to oldsize+count
  // Assume index <= size
  // Return *this
  ByteArray &insert(size_t index, const BYTE *bp, size_t count);


  // Return *this
  inline ByteArray   &add(BYTE b, size_t count = 1) {
    return insert(size(), b, count);
  }

  // Append bp[0..count-1] to array, setting new arraysize to oldsize+size
  // Return *this
  inline ByteArray   &append(const BYTE *bp, size_t count) {
    return insert(m_size, bp, count);
  }

  inline ByteArray   &addAll(const ByteArray &a) {
    return insert(size(), a.m_data, a.size());
  }

  // Return *this
  ByteArray          &remove(size_t index, size_t count);

  // Insert count zeroes at position [index..index+ount-1]
  // Assume index <= size
  // Return *this
  inline ByteArray   &insertZeroes(size_t index, size_t count) {
    return insert(index, (BYTE)0, count);
  }
  // Append count zeroes
  // Return *this
  inline ByteArray   &addZeroes(size_t count) {
    return insertZeroes(size(), count);
  }
  // Set content array to data[0..size-1]. old content is discarded
  // Return *this
  ByteArray          &setData(const BYTE *data, size_t size);
  // Overwrite a[index..index+size-1] with data.
  // Assume index + count <= size
  // Return *this
  ByteArray          &setBytes(size_t index, const BYTE *data, size_t count);

  ByteArray  operator+( const ByteArray &d) const;
  ByteArray &operator+=(const ByteArray &d);
  inline ByteArray &operator+=(BYTE byte) {
    return add(byte);
  }

  inline const BYTE  *getData() const {
    return m_data;
  }

  void                save(ByteOutputStream &s) const;
  void                load(ByteInputStream  &s);
  // typeName cannot be String, because of MAKEINTRESOURCE
  // Return *this
  ByteArray          &loadFromResource(int resId, const TCHAR *typeName);

  inline size_t       getUpdateCount() const {
    return m_updateCount;
  }

  Iterator<BYTE>      getIterator();
  ConstIterator<BYTE> getIterator() const;

  inline BYTE        *begin() {
    return isEmpty() ? nullptr : &first();
  }
  inline BYTE        *end() {
    return isEmpty() ? nullptr : (&first() + size());
  }
  inline const BYTE  *begin() const {
    return isEmpty() ? nullptr : &first();
  }
  inline const BYTE  *end() const {
    return isEmpty() ? nullptr : (&first() + size());
  }
};

class ByteFileArray {   // Read-only BYTE array accessed by seeking the file, instead of loading
                        // the whole array into memory. Slow, but save space
                        // Bytes starting at startOffset must be written by ByteArray.save
private:
  FILE         *m_f;
  size_t        m_size;
  const size_t  m_startOffset;

  ByteFileArray(const ByteFileArray &src);            // Not defined. Class not cloneable
  ByteFileArray &operator=(const ByteFileArray &src); // Not defined. Class not cloneable
public:
  ByteFileArray(const String &fileName, size_t startOffset);
  ~ByteFileArray();

  inline size_t size() const {
    return m_size;
  }
  BYTE operator[](size_t i) const;
};
