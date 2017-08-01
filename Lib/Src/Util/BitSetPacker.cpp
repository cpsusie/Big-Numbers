#include "pch.h"
#include <Tcp.h>
#include <BitSet.h>
#include "Packer.h"

#define _BITSET_ATOMSIZE 32

Packer &operator<<(Packer &p, const BitSet &s) {
  assert(_BITSET_ATOMSIZE == _BS_BITSINATOM);

  const size_t capacity  = s.getCapacity();
  const size_t atomCount = s.getAtomCount();
  const size_t byteCount = atomCount * sizeof(BitSet::Atom);
  p << (UINT64)capacity;

#if _BITSET_ATOMSIZE == 32
  const ULONG hl = 0x12345678;
  if(hl == htonl(hl)) {
    return p.addElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount]; TRACE_NEW(buffer);
    const BitSet::Atom *src = s.m_p;
    BitSet::Atom *dst = buffer;

    for(intptr_t c = atomCount; c--;) {
      *(dst++) = htonl(*(src++));
    }
    p.addElement(Packer::E_VOID, buffer, byteCount);
    SAFEDELETEARRAY(buffer);
  }
#elif _BITSET_ATOMSIZE == 64
  const UINT64 hll = 0x123456789abcdef;
  if(hll == htonll(hll)) {
    return p.addElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount]; TRACE_NEW(buffer);
    const BitSet::Atom *src = s.m_p;
    BitSet::Atom *dst = buffer;

    for(size_t c = atomCount; c--;) {
      *(dst++) = htonll(*(src++));
    }
    p.addElement(Packer::E_VOID, buffer, byteCount);
    SAFEDELETEARRAY(buffer);
  }
#else
#error 32/64 bits expected in BitSet::Atom
#endif // _BITSET_ATOMSIZE == 64
  return p;
}

Packer &operator>>(Packer &p, BitSet &s) {
  size_t capacity;
  UINT64 capacity64;
  p >> capacity64;
  CHECKUINT64ISVALIDSIZET(capacity64)
  capacity = (size_t)capacity64;

  s.setCapacity(capacity);
  const size_t atomCount = s.getAtomCount();
  const size_t byteCount = atomCount * sizeof(BitSet::Atom);

#if _BITSET_ATOMSIZE == 32
  const ULONG hl = 0x12345678;
  if(hl == htonl(hl)) {
    p.getElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount]; TRACE_NEW(buffer);
    p.getElement(Packer::E_VOID, buffer, byteCount);

    const BitSet::Atom *src = buffer;
    BitSet::Atom *dst = s.m_p;

    for(size_t c = atomCount; c--;) {
      *(dst++) = ntohl(*(src++));
    }
    SAFEDELETEARRAY(buffer);
  }
#elif _BITSET_ATOMSIZE == 64
  const UINT64 hll = 0x123456789abcdef;
  if(hll == htonll(hll)) {
    p.getElement(Packer::E_VOID, s.m_p, byteCount);
  } else {
    BitSet::Atom *buffer = new BitSet::Atom[atomCount]; TRAE_NEW(buffer);
    p.getElement(Packer::E_VOID, buffer, byteCount);

    const BitSet::Atom *src = buffer;
    BitSet::Atom *dst = s.m_p;

    for(size_t c = atomCount; c--;) {
      *(dst++) = ntohll(*(src++));
    }
    SAFEDELETEARRAY(buffer);
  }
#else
#error 32/64 bits expected in BitSet::Atom
#endif // _BITSET_ATOMSIZE == 64
  return p;
}
