#pragma once

#include "MyUtil.h"
#include "ByteStream.h"

class ByteArray {
private:
  BYTE         *m_data;
  size_t        m_capacity;
  size_t        m_size;

  void cleanup();
  void indexError(const TCHAR *method, size_t i) const;
protected:
  void init();
  virtual BYTE *allocateBytes( size_t size);
  virtual void  deallocateBytes(BYTE *buffer);
  virtual size_t getCapacityCeil(size_t capacity) const {
    return capacity;
  }
public:
  ByteArray();
  ByteArray(const BYTE *data, size_t size);
  ByteArray(const ByteArray &src);
  explicit ByteArray(size_t capacity);
  virtual ~ByteArray();
  ByteArray &operator=( const ByteArray &src);
  ByteArray  operator+( const ByteArray &d) const;
  ByteArray &operator+=(const ByteArray &d);
  inline ByteArray &operator+=(BYTE byte) {
    return append(byte);
  }

  ByteArray &clear(intptr_t newCapacity = 0);                   // return this, if newCapcity == -1 it will not be changed
  ByteArray &append( const ByteArray &d);
  ByteArray &append( const BYTE *data, size_t size);
  inline ByteArray &append(BYTE byte) {
    return appendConstant(byte, 1);
  }
  inline ByteArray &appendZeroes(size_t count) {
    return appendConstant(0, count);
  }
  inline ByteArray &insertZeroes(size_t index, size_t count) {
    return insertConstant(index, 0, count);
  }
  inline ByteArray &appendConstant(BYTE b, size_t count) {
    return insertConstant(size(), b, count);
  }

  ByteArray &insertConstant(size_t index, BYTE c, size_t count);
  ByteArray &setData(const BYTE *data, size_t size); // return this

  ByteArray &remove(size_t index, size_t count);

  inline size_t size() const {
    return m_size;
  }

  inline bool isEmpty() const {
    return m_size == 0;
  }

  void setCapacity(size_t capacity);
  inline size_t getCapacity() const {
    return m_capacity;
  }

  inline const BYTE *getData() const {
    return m_data;
  }

  inline const BYTE &operator[](size_t i) const {
    if(i >= m_size) indexError(__TFUNCTION__, i);
    return m_data[i];
  }

  inline BYTE &operator[](size_t i) {
    if(i >= m_size) indexError(__TFUNCTION__, i);
    return m_data[i];
  }

  bool operator==(const ByteArray &a) const;
  bool operator!=(const ByteArray &a) const;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  ByteArray &loadFromResource(int resId, const TCHAR *typeName); // return *this. typeName cannot be String, because of MAKEINTRESOURCE
};

class ExecutableByteArray : public ByteArray {
protected:
  BYTE *allocateBytes(size_t size);
  void  deallocateBytes(BYTE *buffer);
  size_t getCapacityCeil(size_t capacity) const;
public:
  inline ExecutableByteArray() {
  }
  ExecutableByteArray(const BYTE *data, UINT size);
  ExecutableByteArray(const ByteArray &src);
  explicit ExecutableByteArray(UINT capacity);
  ~ExecutableByteArray();
  static size_t getSystemPageSize();
  void flushInstructionCache();
};

class ByteFileArray {   // Read-only BYTE array accessed by seeking the file, instead of loading 
                        // the whole array into memory. Slow, but save space
                        // Bytes starting at startOffset must be written by ByteArray.save
private:
  FILE         *m_f;
  size_t        m_size;
  const size_t  m_startOffset;

public:
  ByteFileArray(const String &fileName, size_t startOffset);
  ByteFileArray(const ByteFileArray &src);            // not defined
  ByteFileArray &operator=(const ByteFileArray &src); // not defined
  ~ByteFileArray();

  inline size_t size() const {
    return m_size;
  }
  BYTE operator[](size_t i) const;
};
