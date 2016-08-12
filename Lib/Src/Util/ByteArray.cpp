#include "pch.h"
#include <ByteArray.h>

ByteArray::ByteArray() {
  init();
}

ByteArray::ByteArray(const BYTE *data, size_t size) {
  init();
  if (size > 0) {
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

ByteArray &ByteArray::append(const ByteArray &d) {
  return append(d.m_data, d.size());
}

ByteArray &ByteArray::append(const BYTE *data, size_t size) {
  if(size) {
    const size_t newSize = m_size + size;
    if(newSize > m_capacity) {     // Careful here!!! data and m_data might overlap!!
      const size_t newCapacity = getCapacityCeil(2 * newSize); // > 0
      BYTE *newData = allocateBytes(newCapacity);
      if(m_size > 0) {
        memcpy(newData, m_data, m_size);
      }
      memcpy(newData + m_size, data, size);
      if(m_data != NULL) { // Now we can safely delete m_data.
        deallocateBytes(m_data);
      }
      m_data     = newData;
      m_capacity = newCapacity;
    } else {
      memcpy(m_data+m_size, data, size);
    }
    m_size = newSize;
  }
  return *this;
}

ByteArray &ByteArray::insertConstant(size_t index, BYTE b, size_t count) {
  if(count == 0) {
    return *this;
  }
  if(index > m_size) {
    indexError(_T(__FUNCTION__), index);
  }
  const size_t newSize = m_size + count;
  if(newSize > m_capacity) {
    const size_t newCapacity = getCapacityCeil(2 * newSize);
    BYTE *newData = allocateBytes(newCapacity);
    if(index > 0) {
      memcpy(newData, m_data, index);
    }
    if(index < m_size) {
      memcpy(newData+index+count, m_data+index, m_size-index);
    }
    if(m_data != NULL) {
      deallocateBytes(m_data);
    }
    m_data = newData;
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
  return result.append(d);
}

ByteArray &ByteArray::operator+=(const ByteArray &d) {
  return append(d);
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

ByteArray &ByteArray::remove(size_t index, size_t count) {
  DEFINEMETHODNAME;
  if(count == 0) {
    return *this;
  }
  const size_t j = index+count;
  if(j > m_size) {
    throwException(_T("%s(%s,%s): Invalid index. size=%s")
                  ,method
                  ,format1000(index).cstr(), format1000(count).cstr()
                  ,format1000(m_size).cstr());
  }
  if(j < m_size) {
    memmove(m_data+index, m_data+j, (m_size-j));
  }
  m_size -= count;
  if(m_size < m_capacity/4) {
    setCapacity(m_size);
  }
  return *this;
}

void ByteArray::indexError(const TCHAR *method, size_t i) const {
  throwException(_T("%s:Index %s out of range. size=%s")
                ,method
                ,format1000(i).cstr()
                ,format1000(m_size).cstr());
}

void ByteArray::init() {
  m_data     = NULL;
  m_capacity = 0;
  m_size     = 0;
}

BYTE *ByteArray::allocateBytes(size_t size) {
  return new BYTE[size];
}

void  ByteArray::deallocateBytes(BYTE *buffer) {
  delete[] buffer;
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
