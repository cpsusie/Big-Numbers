#include "pch.h"
#include <Math/Expression/MachineCode.h>

void MachineCode::finalize(void *esi) {
  m_entryPoint = (ExpressionEntryPoint)getData();
  m_esi        = esi;
  flushInstructionCache();
}

int MachineCode::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  append((BYTE*)bytes,count);
  return ret;
}
