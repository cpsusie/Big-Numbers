#include "pch.h"
#include <BigEndianStream.h>

BigEndianOutputStream &BigEndianOutputStream::operator<<(const BitSet &s) {
  const UINT64 capacity = s.getCapacity();
  *this << capacity;
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  m_out.putBytes((BYTE*)s.getFirstAtom(), s.getAtomCount() * sizeof(BitSet::Atom));
#else
  BitSet::Atom buffer[4096], *dst = buffer, *endBuf = buffer+ARRAYSIZE(buffer);
  const BitSet::Atom *first  = s.getFirstAtom();
  const BitSet::Atom *endSet = first + s.getAtomCount();
  for(const BitSet::Atom *src = first; src < endSet;) {
    *(dst++) = *(src++);
    if(dst == endBuf) { // flush buffer
      putLongs(buffer, ARRAYSIZE(buffer));
      dst = buffer;
    }
  }
  if(dst > buffer) {
    putLongs(buffer, dst - buffer);
  }
#endif
  return *this;
}

BigEndianInputStream &BigEndianInputStream::operator>>(BitSet &s) {
  UINT64 capacity;
  *this >> capacity;
  CHECKUINT64ISVALIDSIZET(capacity);
  s.setCapacity((size_t)capacity);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  m_in.getBytesForced((BYTE*)s.getFirstAtom(), s.getAtomCount() * sizeof(BitSet::Atom));
#else
  BitSet::Atom       *first  = (BitSet::Atom*)(s.getFirstAtom());
  const BitSet::Atom *endSet = first + s.getAtomCount();
  for(BitSet::Atom *dst = first; dst < endSet;) {
    BitSet::Atom buffer[4096];
    const UINT   n = min(ARRAYSIZE(buffer), (UINT)(endSet-dst));
    getLongs(buffer, n);
    for(const BitSet::Atom *src = buffer, *endBuf = src+n; src < endBuf;) {
      *(dst++) = *(src++);
    }
  }
#endif
  return *this;
}
