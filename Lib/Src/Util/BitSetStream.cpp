#include "pch.h"
#include <BitSet.h>

void BitSet::save(ByteOutputStream &s) const {
  s.putBytes((BYTE*)(&m_capacity), sizeof(m_capacity));
  s.putBytes((BYTE*)m_p, sizeof(Atom) * getAtomCount(m_capacity));
}

void BitSet::load(ByteInputStream &s) {
  unsigned long capacity;
  s.getBytesForced((BYTE*)&capacity, sizeof(capacity));
  setCapacity(capacity);
  s.getBytesForced((BYTE*)m_p, sizeof(Atom) * getAtomCount(capacity));
}
