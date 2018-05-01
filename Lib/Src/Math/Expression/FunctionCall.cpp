#include "pch.h"
#include "FunctionCall.h"

void ValueAddressCalculation::setValueCount(size_t valueCount) {
  if(valueCount == 0) {
    m_esiOffset = 0;
    m_esi       = NULL;
  } else {
    const int maxOffset = 127-127%sizeof(Real);
    m_esiOffset = (char)min(maxOffset, (valueCount / 2) * sizeof(Real));
    m_esi       = (BYTE*)m_valueTable.getBuffer() + m_esiOffset;
  }
}
