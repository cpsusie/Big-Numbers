#include "pch.h"
#include <Tcp.h>
#include <BitSet.h>

Packer &operator<<(Packer &p, const BitSet &s) {
  const size_t capacity  = s.getCapacity();
  const size_t atomCount = s.getAtomCount();
  const size_t byteCount = atomCount * sizeof(BitSet::Atom);
  p << capacity;

#if _BITSET_ATOMSIZE == 32
  const unsigned long hl = 0x12345678;
  if(hl == htonl(hl)) {
    return p.addElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount];
    const BitSet::Atom *src = s.m_p;
    BitSet::Atom *dst = buffer;

    for(int c = atomCount; c--;) {
      *(dst++) = htonl(*(src++));
    }
    p.addElement(Packer::E_VOID, buffer, byteCount);
    delete[] buffer;
  }
#else
  const unsigned short hs = 0x1234;
  if(hs == htons(hs)) {
    return p.addElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount];
    const BitSet::Atom *src = s.m_p;
    BitSet::Atom *dst = buffer;

    for(int c = atomCount; c--;) {
      *(dst++) = htons(*(src++));
    }
    p.addElement(Packer::E_VOID, buffer, byteCount);
    delete[] buffer;
  }
#endif

  return p;
}

Packer &operator>>(Packer &p, BitSet &s) {
  size_t capacity;
  p >> capacity;
  s.setCapacity(capacity);
  const size_t atomCount = s.getAtomCount();
  const size_t byteCount = atomCount * sizeof(BitSet::Atom);

#if _BITSET_ATOMSIZE == 32
  const unsigned long hl = 0x12345678;
  if(hl == htonl(hl)) {
    p.getElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount];
    p.getElement(Packer::E_VOID, buffer, byteCount);

    const BitSet::Atom *src = buffer;
    BitSet::Atom *dst = s.m_p;

    for(int c = atomCount; c--;) {
      *(dst++) = ntohl(*(src++));
    }
    delete[] buffer;
  }
#else
  const unsigned long hs = 0x1234;
  if(hs == htons(hs)) {
    p.getElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount];
    p.getElement(Packer::E_VOID, buffer, byteCount);

    const BitSet::Atom *src = buffer;
    BitSet::Atom *dst = s.m_p;

    for(int c = atomCount; c--;) {
      *(dst++) = ntohs(*(src++));
    }
    delete[] buffer;
  }
#endif

  return p;
}
