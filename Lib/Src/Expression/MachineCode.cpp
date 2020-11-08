#include "pch.h"
#include "MachineCode.h"

namespace Expr {

void MachineCode::finalize(void *esi) {
  m_entryPoint = (ExpressionEntryPoint)getData();
  m_esi        = esi;
  flushInstructionCache();
}

}; // namespace Expr
