#include "pch.h"
#include <BitSet.h>
#include <CountedByteStream.h>

// read format written by BitSet::save
FileBitSet::FileBitSet(const String &fileName, UINT64 startOffset) : m_f(fileName) {
  m_f.seek(startOffset);
  UINT64 capacity64;
  m_f.getBytesForced((BYTE*)&capacity64, sizeof(capacity64));
  CHECKUINT64ISVALIDSIZET(capacity64)

  m_firstAtomOffset = startOffset + sizeof(capacity64);
  m_capacity        = (size_t)capacity64;
}

#define BITSINATOM      (sizeof(BitSet::Atom)*8)
#define ATOMINDEX(i)    ((i)/BITSINATOM)
#define BYTEINDEX(i)    ((i)/8)
#define ATOMBIT(i)      ((BitSet::Atom)1 << ((i)%BITSINATOM))

bool FileBitSet::contains(size_t i) const {
  if(i >= m_capacity) return false;
  m_f.seek(m_firstAtomOffset + ATOMINDEX(i)*sizeof(BitSet::Atom));
  BitSet::Atom atom;
  m_f.getBytesForced((BYTE*)&atom, sizeof(atom));
  return (atom & ATOMBIT(i)) ? true : false;
}

