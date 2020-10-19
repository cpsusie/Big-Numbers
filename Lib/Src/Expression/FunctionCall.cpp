#include "pch.h"
#include <IntelCPU/Opcode.h>
#include "FunctionCall.h"

namespace Expr {

using namespace IntelCPU;

void ValueAddressCalculation::setValueCount(size_t valueCount) {
  if(valueCount == 0) {
    m_esiOffset = 0;
    m_esi       = nullptr;
  } else {
    const int maxOffset = 127-127%sizeof(Real);
    m_esiOffset = (char)min(maxOffset, (valueCount / 2) * sizeof(Real));
    m_esi       = (BYTE*)m_valueTable.getBuffer() + m_esiOffset;
  }
}

String FunctionCall::toString() const {
  return format(_T("%-20s (%s)"), m_signature.cstr(), formatHexValue((size_t)m_fp).cstr());
}

}; // namespace Expr
