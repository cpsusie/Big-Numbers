#include "pch.h"
#include <BitSet.h>

void BitSet::save(ByteOutputStream &s) const {
  UINT64 cap64 = m_capacity;
  s.putBytes((BYTE*)(&cap64), sizeof(cap64));
  s.putBytes((BYTE*)m_p, sizeof(Atom) * getAtomCount(m_capacity));
}

void BitSet::load(ByteInputStream &s) {
  UINT64 capacity64;
  s.getBytesForced((BYTE*)&capacity64, sizeof(capacity64));
  CHECKUINT64ISVALIDSIZET(capacity64)
  setCapacity((size_t)capacity64);
  s.getBytesForced((BYTE*)m_p, sizeof(Atom) * getAtomCount((size_t)capacity64));
}

