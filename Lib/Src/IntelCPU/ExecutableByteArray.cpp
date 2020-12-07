#include "pch.h"
#include <IntelCPU/ExecutableByteArray.h>

ExecutableByteArray::ExecutableByteArray(const BYTE *data, UINT size) {
  init();
  if(size > 0) {
    setData(data, size);
  }
}

ExecutableByteArray::ExecutableByteArray(const ByteArray &src) {
  init();
  if(!src.isEmpty()) {
    setData(src.getData(), src.size());
  }
}

ExecutableByteArray::ExecutableByteArray(UINT capacity) {
  init();
  setCapacity(capacity);
}

ExecutableByteArray::~ExecutableByteArray() {
  clear();
}

size_t ExecutableByteArray::getCapacityCeil(size_t capacity) const {
  if(capacity == 0) {
    return 0;
  }
  const size_t pageSize = getSystemPageSize();
  const size_t n        = capacity % pageSize;
  if(n) {
    return capacity + (pageSize - n);
  }
  return capacity;
}

BYTE *ExecutableByteArray::allocateBytes(size_t size) {
  void *result = VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
  if(result == nullptr) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("VirtualAlloc"));
  }
  return (BYTE*)result;
}

void  ExecutableByteArray::deallocateBytes(BYTE *buffer) {
  if(!VirtualFree(buffer, 0, MEM_RELEASE)) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("VirtualFree"));
  }
}

size_t ExecutableByteArray::getSystemPageSize() { // static
  static size_t pageSize = 0;
  if(pageSize == 0) {
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    pageSize = systemInfo.dwPageSize;
  }
  return pageSize;
}

void ExecutableByteArray::flushInstructionCache() {
  if(!FlushInstructionCache(GetCurrentProcess(), getData(), getCapacity())) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("FlushInstructionCache"));
  }
}
