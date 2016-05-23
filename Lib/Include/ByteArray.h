#pragma once

#include "MyUtil.h"
#include "ByteStream.h"

class ByteArray {
private:
  DECLARECLASSNAME;
  BYTE         *m_data;
  unsigned int  m_capacity;
  unsigned int  m_size;

  void cleanup();
  void indexError(unsigned int i) const;
protected:
  void init();
  virtual BYTE *allocateBytes( size_t size);
  virtual void  deallocateBytes(BYTE *buffer);
  virtual size_t getCapacityCeil(size_t capacity) const {
    return capacity;
  }
public:
  ByteArray();
  ByteArray(const BYTE *data, unsigned int size);
  ByteArray(const ByteArray &src);
  explicit ByteArray(unsigned int capacity);
  virtual ~ByteArray();
  ByteArray &operator=( const ByteArray &src);
  ByteArray  operator+( const ByteArray &d) const;
  ByteArray &operator+=(const ByteArray &d);
  inline ByteArray &operator+=(BYTE byte) {
    return append(byte);
  }

  ByteArray &clear(int newCapacity = 0);                   // return this, if newCapcity == -1 it will not be changed
  ByteArray &append( const ByteArray &d);
  ByteArray &append( const BYTE *data, unsigned int size);
  inline ByteArray &append(BYTE byte) {
    return appendConstant(byte, 1);
  }
  inline ByteArray &appendZeroes(unsigned int count) {
    return appendConstant(0, count);
  }
  inline ByteArray &insertZeroes(unsigned int index, unsigned int count) {
    return insertConstant(index, 0, count);
  }
  inline ByteArray &appendConstant(BYTE b, unsigned int count) {
    return insertConstant(size(), b, count);
  }

  ByteArray &insertConstant(unsigned int index, BYTE c, unsigned int count);
  ByteArray &setData(const BYTE *data, unsigned int size); // return this

  ByteArray &remove(unsigned int index, unsigned int count);

  inline unsigned int size() const {
    return m_size;
  }

  inline bool isEmpty() const {
    return m_size == 0;
  }

  void setCapacity(unsigned int capacity);
  inline unsigned int getCapacity() const {
    return m_capacity;
  }

  inline const BYTE *getData() const {
    return m_data;
  }

  inline const BYTE &operator[](unsigned int i) const {
    if(i >= m_size) indexError(i);
    return m_data[i];
  }

  inline BYTE &operator[](unsigned int i) {
    if(i >= m_size) indexError(i);
    return m_data[i];
  }

  bool operator==(const ByteArray &a) const;
  bool operator!=(const ByteArray &a) const;

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
  ByteArray &loadFromResource(int resId, const TCHAR *typeName); // return *this. typeName cannot be String, because of MAKEINTRESOURCE
};

class ExecutableByteArray : public ByteArray {
private:
  DECLARECLASSNAME;
protected:
  BYTE *allocateBytes(size_t size);
  void  deallocateBytes(BYTE *buffer);
  size_t getCapacityCeil(size_t capacity) const;
public:
  inline ExecutableByteArray() {
  }
  ExecutableByteArray(const BYTE *data, unsigned int size);
  ExecutableByteArray(const ByteArray &src);
  explicit ExecutableByteArray(unsigned int capacity);
  ~ExecutableByteArray();
  static size_t getSystemPageSize();
};

class ByteFileArray {   // Read-only BYTE array accessed by seeking the file, instead of loading 
                        // the whole array into memory. Slow, but save space
                        // Bytes starting at startOffset must be written by ByteArray.save
private:
  FILE              *m_f;
  unsigned int       m_size;
  const unsigned int m_startOffset;

public:
  ByteFileArray(const String &fileName, unsigned int startOffset);
  ByteFileArray(const ByteFileArray &src);            // not defined
  ByteFileArray &operator=(const ByteFileArray &src); // not defined
  ~ByteFileArray();

  inline unsigned int size() const {
    return m_size;
  }
  BYTE operator[](unsigned int i) const;
};
