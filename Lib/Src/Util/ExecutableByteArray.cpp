#include "pch.h"
#include <ByteArray.h>

ExecutableByteArray::ExecutableByteArray(const BYTE *data, unsigned int size) {
  init();
  if (size > 0) {
    setData(data, size);
  }
}

ExecutableByteArray::ExecutableByteArray(const ByteArray &src) {
  init();
  if (!src.isEmpty()) {
    setData(src.getData(), src.size());
  }
}

ExecutableByteArray::ExecutableByteArray(unsigned int capacity) {
  init();
  setCapacity(capacity);
}

ExecutableByteArray::~ExecutableByteArray() {
  clear();
}

size_t ExecutableByteArray::getCapacityCeil(size_t capacity) const {
  if (capacity == 0) {
    return 0;
  }
  const size_t pageSize = getSystemPageSize();
  const size_t n = capacity % pageSize;
  if (n) {
    return capacity + (pageSize - n);
  }
  return capacity;
}

BYTE *ExecutableByteArray::allocateBytes(size_t size) {
  DEFINEMETHODNAME;
  void *result = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if (result == NULL) {
    throwLastErrorOnSysCallException(method);
  }
  return (BYTE*)result;
}

void  ExecutableByteArray::deallocateBytes(BYTE *buffer) {
  DEFINEMETHODNAME;
  if (!VirtualFree(buffer, 0, MEM_RELEASE)) {
    throwLastErrorOnSysCallException(method);
  }
}

size_t ExecutableByteArray::getSystemPageSize() {
  static size_t pageSize = 0;
  if (pageSize == 0) {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    pageSize = systemInfo.dwPageSize;
  }
  return pageSize;
}

void ExecutableByteArray::flushInstructionCache() {
  DEFINEMETHODNAME;
  if (!FlushInstructionCache(GetCurrentProcess(), getData(), getCapacity())) {
    throwLastErrorOnSysCallException(method);
  }
}

