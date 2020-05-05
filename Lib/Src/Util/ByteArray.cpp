#include "pch.h"
#include <ByteStream.h>
#include <ByteArray.h>

ByteArray::ByteArray() {
  init();
}

ByteArray::ByteArray(const BYTE *data, size_t size) {
  init();
  if(size > 0) {
    setData(data, size);
  }
}

ByteArray::ByteArray(const ByteArray &src) {
  init();
  if(!src.isEmpty()) {
    setData(src.getData(), src.size());
  }
}

ByteArray::ByteArray(size_t capacity) {
  init();
  setCapacity(capacity);
}

ByteArray &ByteArray::operator=(const ByteArray &src) {
  if(&src == this) {
    return *this;
  }
  if(src.m_size != m_size) {
    cleanup();
    setCapacity(src.m_size);
  }
  m_size = src.m_size;
  if(m_size) {
    memcpy(m_data,src.m_data,m_size);
  }
  return *this;
}

ByteArray::~ByteArray() {
  cleanup();
}

ByteArray &ByteArray::clear(intptr_t newCapacity) {
  switch(newCapacity) {
  case 0 :
    cleanup();
    break;
  case -1:
    m_size = 0;
    break;
  default:
    m_size = 0;
    setCapacity(newCapacity);
    break;
  }
  return *this;
}

ByteArray &ByteArray::add(size_t index, const BYTE *data, size_t size) {
  if(size == 0) {
    return *this;
  }
  if(index > m_size) {
    indexError(__TFUNCTION__, index);
  }
  const size_t newSize = m_size + size;
  if((newSize <= m_capacity) && (index == m_size)) {
    memcpy(m_data + m_size, data, size); // simple append
  } else {  // Careful here!!! data and m_data might overlap!!
    const size_t newCapacity = getCapacityCeil(2 * newSize); // > 0
    BYTE        *newData     = allocateBytes(newCapacity), *dst = newData;
    if(index < m_size) {
      if(index) {
        memcpy(dst, m_data, index); dst += index; // copy first part of old content
      }
      memcpy(dst, data, size); dst += size; // copy data
      memcpy(dst, m_data + index, m_size - index); // copy tail of old content
    } else {
      if(m_size > 0) {
        memcpy(dst, m_data, m_size); dst += m_size;
      }
      memcpy(dst, data, size);
    }
    if(m_data != NULL) { // Now we can safely delete m_data.
      deallocateBytes(m_data);
    }
    m_data     = newData;
    m_capacity = newCapacity;
  }
  m_size = newSize;
  return *this;
}

ByteArray &ByteArray::add(size_t index, BYTE b, size_t count) {
  if(count == 0) {
    return *this;
  }
  if(index > m_size) indexError(__TFUNCTION__, index);
  const size_t newSize = m_size + count;
  if(newSize > m_capacity) {
    const size_t newCapacity = getCapacityCeil(2 * newSize);
    BYTE        *newData     = allocateBytes(newCapacity);
    if(index > 0) {
      memcpy(newData, m_data, index);
    }
    if(index < m_size) {
      memcpy(newData+index+count, m_data+index, m_size-index);
    }
    if(m_data != NULL) {
      deallocateBytes(m_data);
    }
    m_data     = newData;
    m_capacity = newCapacity;
  } else if(index < m_size) {
    memmove(m_data+index+count, m_data+index, m_size-index);
  }
  memset(m_data+index, b, count);
  m_size = newSize;
  return *this;
}

ByteArray ByteArray::operator+(const ByteArray &d) const {
  ByteArray result(*this);
  return result.addAll(d);
}

ByteArray &ByteArray::operator+=(const ByteArray &d) {
  return addAll(d);
}

ByteArray &ByteArray::setData(const BYTE *data, size_t size) {
  cleanup();
  if(size > 0) {
    setCapacity(size);
    m_size = size;
    memcpy(m_data, data, size);
  }
  return *this;
}

// return this. Assume index + count <= size
ByteArray &ByteArray::setBytes(size_t index, const BYTE *data, size_t count) {
  if(count > 0) {
    if(index + count > m_size) indexError(__TFUNCTION__, index, count);
    memcpy(m_data+index, data, count);
  }
  return *this;
}

ByteArray &ByteArray::remove(size_t index, size_t count) {
  if(count > 0) {
    const size_t j = index + count;
    if(j > m_size) indexError(__TFUNCTION__, index, count);
    if(j < m_size) {
      memmove(m_data+index, m_data+j, (m_size-j));
    }
    m_size -= count;
    if(m_size < m_capacity/4) {
      setCapacity(m_size);
    }
  }
  return *this;
}

void ByteArray::indexError(const TCHAR *method, size_t index) const {
  throwIndexOutOfRangeException(method, index, size());
}

void ByteArray::indexError(const TCHAR *method, size_t index, size_t count) const {
  throwIndexOutOfRangeException(method, index, count, size());
}

void ByteArray::init() {
  m_data     = NULL;
  m_capacity = 0;
  m_size     = 0;
}

BYTE *ByteArray::allocateBytes(size_t size) {
  BYTE *b = new BYTE[size]; TRACE_NEW(b); return b;
}

void  ByteArray::deallocateBytes(BYTE *buffer) {
  SAFEDELETEARRAY(buffer);
}

void ByteArray::cleanup() {
  if(m_data != NULL) {
    deallocateBytes(m_data);
  }
  init();
}

void ByteArray::setCapacity(size_t capacity) {
  if(capacity < m_size) {
    capacity = m_size;
  }
  capacity = getCapacityCeil(capacity);
  if(capacity == 0) {
    cleanup();
  } else if(capacity != m_capacity) {
    BYTE *newData = allocateBytes(capacity);
    if(m_size > 0) {
      memcpy(newData, m_data, m_size);
      deallocateBytes(m_data);
    }
    m_data     = newData;
    m_capacity = capacity;
  }
}

bool ByteArray::operator==(const ByteArray &a) const {
  if(this == &a) {
    return true;
  }
  if(m_size != a.m_size) {
    return false;
  }
  if(m_size == 0) {
    return true;
  }
  return memcmp(m_data, a.m_data, m_size) == 0;
}

bool ByteArray::operator!=(const ByteArray &a) const {
  return !(*this == a);
}

void ByteArray::save(ByteOutputStream &s) const {
  const size_t size = m_size; // if modified, => take care of ByteFileArray
  s.putBytes((const BYTE*)&size, sizeof(size));
  if(size) {
    s.putBytes(m_data, size);
  }
}

void ByteArray::load(ByteInputStream &s) {
  clear();
  size_t size;
  s.getBytesForced((BYTE*)&size, sizeof(size));
  if(size) {
    setCapacity(size);
    s.getBytesForced(m_data, size);
    m_size = size;
  }
}
