#pragma once

#include <ByteArray.h>
#include "PragmaLib.h"

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
