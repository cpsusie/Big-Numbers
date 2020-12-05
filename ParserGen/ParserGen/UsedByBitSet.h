#pragma once

#include <BitSet.h>
#include "BitSetParameters.h"

class UsedByBitSet : public BitSet {
private:
  const ElementType m_type;
public:
  UsedByBitSet(const BitSetParameters &param) : BitSet(param.getCapacity()), m_type(param.getType()) {
  }
  inline ElementType getElementType() const {
    return m_type;
  }
  String toString(const AbstractSymbolNameContainer *nameContainer = nullptr) const;
};
