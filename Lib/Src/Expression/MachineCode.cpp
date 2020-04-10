#include "pch.h"
#include "MachineCode.h"

namespace Expr {

void MachineCode::finalize(void *esi) {
  m_entryPoint = (ExpressionEntryPoint)getData();
  m_esi        = esi;
  flushInstructionCache();
}

int MachineCode::addBytes(const void *bytes, int count) {
  const int ret = (int)size();
  add((BYTE*)bytes,count);
  return ret;
}

}; // namespace Expr
