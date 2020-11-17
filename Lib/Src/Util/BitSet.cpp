#include "pch.h"
#include <String.h>
#include <CommonHashFunctions.h>
#include <BitSet.h>

#if defined(IS64BIT)
#if defined(BITSET_ASM_OPTIMIZED)
extern "C" {
  bool bitSetIsEmpty(const void *p, size_t atomCount);
};
#endif // BITSET_ASM_OPTIMIZED
#endif // IS64BIT

String BitSet::indexOutOfRangeString(size_t index) const {
  return format(_T("Index %s out of range. Capacity=%s"), format1000(index).cstr(), format1000(m_capacity).cstr());
}

void BitSet::throwIndexOutOfRange(const TCHAR *method, size_t index, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throwInvalidArgumentException(method, _T("%s:%s"), msg.cstr(), indexOutOfRangeString(index).cstr());
}

 // Array of atoms with number of 1-bits = index, index = [0;_BITSET_ATOMSIZE]
BitSet::Atom BitSet::s_mask[_BS_BITSINATOM+1];

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitBitSetMask {
public:
  InitBitSetMask();
};

InitBitSetMask::InitBitSetMask() {
  BitSet::s_mask[0] = 0;
  for(int i = 1; i <= _BS_BITSINATOM; i++) {
    BitSet::s_mask[i] = _BS_ATOMBIT(i-1) | BitSet::s_mask[i-1];
  }

//#define MASK(i) ((i==32)?(-1):((1<<(i))-1))

//  for(i = 0; i <= _BS_BITSINATOM; i++)
//    if(mask[i] != MASK(i))
//      throwException("mask[%d] != MASK(%d)",i,i);
/*
  for(i = 0; i <= _BS_BITSINATOM; i++) {
    cout << "mask[" << dec << i << "]:" << hex << mask[i] << ((i % 2 == 1) ? "\n" : " ");
  }
  cout.flush();
*/
}

static InitBitSetMask maskInitializer;

BitSet::BitSet(size_t capacity) {
  if(capacity == 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Capacity=0"));
  }
  m_capacity = capacity;
  const size_t atomCount = _BS_ATOMCOUNT(m_capacity);
  m_p = new Atom[atomCount]; TRACE_NEW(m_p)
  memset(m_p,0,atomCount * sizeof(Atom));
}

BitSet::BitSet(const BitSet &set) {
  m_capacity = set.m_capacity;
  const size_t atomCount = _BS_ATOMCOUNT(m_capacity);
  m_p = new Atom[atomCount]; TRACE_NEW(m_p)
  memcpy(m_p,set.m_p,atomCount * sizeof(Atom));
}

BitSet &BitSet::operator=(const BitSet &rhs) {
  if(this == &rhs) {
    return *this;
  }
  const size_t atomCount = _BS_ATOMCOUNT(rhs.m_capacity);
  if(rhs.m_capacity != m_capacity) {
    SAFEDELETEARRAY(m_p);
    m_capacity = rhs.m_capacity;
    m_p = new Atom[atomCount]; TRACE_NEW(m_p);
  }
  memcpy(m_p,rhs.m_p,atomCount * sizeof(Atom));
  return *this;
}

BitSet &BitSet::setCapacity(size_t newCapacity) {
  const size_t newAtomCount = _BS_ATOMCOUNT(newCapacity);
  const size_t oldAtomCount = _BS_ATOMCOUNT(m_capacity );
  if(newAtomCount != oldAtomCount) {
    Atom *p = new Atom[newAtomCount]; TRACE_NEW(p);
    if(newAtomCount > oldAtomCount) {
      memcpy(p, m_p, oldAtomCount * sizeof(Atom));
      memset(p + oldAtomCount, 0, (newAtomCount-oldAtomCount) * sizeof(Atom));
    } else {
      memcpy(p, m_p, newAtomCount * sizeof(Atom));
    }
    SAFEDELETEARRAY(m_p);
    m_p = p;
  }
  m_capacity = newCapacity;
  return *this;
}

BitSet &BitSet::remove(size_t a, size_t b) {
  if(b >= m_capacity) {
    throwIndexOutOfRange(__TFUNCTION__, b, _T("(%s,%s)"), format1000(a).cstr(), format1000(b).cstr());
  }
  if(a > b) {
    return *this;
  }
  size_t aIndex = _BS_ATOMINDEX(a);
  size_t bIndex = _BS_ATOMINDEX(b);

  if(aIndex < bIndex) {
    if(a % _BS_BITSINATOM) {
      m_p[aIndex] &= _BS_MASKATOM(a%_BS_BITSINATOM);
      aIndex++;
    }
    if((b+1) % _BS_BITSINATOM) {
      m_p[bIndex] &= ~_BS_MASKATOM(b%_BS_BITSINATOM + 1);
      bIndex--;
    }
    if(bIndex >= aIndex) {
      memset(m_p+aIndex, 0, (bIndex-aIndex+1)*sizeof(Atom));
    }
  } else { // aIndex == bIndex
    m_p[aIndex] &= _BS_MASKATOM(a%_BS_BITSINATOM) | ~_BS_MASKATOM(b%_BS_BITSINATOM + 1);
  }
  return *this;
}

BitSet &BitSet::add(size_t a, size_t b) {
  if(b >= m_capacity) {
    throwIndexOutOfRange(__TFUNCTION__, b, _T("(%s,%s)"), format1000(a).cstr(), format1000(b).cstr());
  }
  if(a > b) {
    return *this;
  }
  size_t aIndex = _BS_ATOMINDEX(a);
  size_t bIndex = _BS_ATOMINDEX(b);

  if(aIndex < bIndex) {
    if(a % _BS_BITSINATOM) {
      m_p[aIndex] |= ~_BS_MASKATOM(a%_BS_BITSINATOM);
      aIndex++;
    }
    if((b+1) % _BS_BITSINATOM) {
      m_p[bIndex] |= _BS_MASKATOM(b%_BS_BITSINATOM + 1);
      bIndex--;
    }

    if(bIndex >= aIndex) {
      memset(m_p+aIndex, ~0, (bIndex-aIndex+1)*sizeof(Atom));
    }
  } else {
    m_p[aIndex] |= ~_BS_MASKATOM(a%_BS_BITSINATOM) & _BS_MASKATOM(b%_BS_BITSINATOM + 1);
  }
  return *this;
}

BitSet &BitSet::invert() {
  Atom *p = m_p;
  for(const Atom *endp = p + _BS_ATOMCOUNT(m_capacity); p < endp; p++) {
    *p = ~*p;
  }

  if(m_capacity % _BS_BITSINATOM) {  // cut the last if neccessary
    p--;
    *p &= _BS_MASKATOM(m_capacity%_BS_BITSINATOM);
  }
  return *this;
}

const char BitSet::setBitsCount[256] = { // Number of set bits for each bytevalue
  0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
  4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

size_t BitSet::size() const {
  size_t result = 0;
  for(const Atom *p = m_p, *endp = p + _BS_ATOMCOUNT(m_capacity); p < endp;) {
    result += getPopCount(*(p++));
  }
  return result;
}

intptr_t BitSet::getIndex(size_t i) const {
  if(!contains(i)) return -1;
  const Atom *p    = m_p, *endp = p + _BS_ATOMINDEX(i);
  size_t result = 0;
  while(p < endp) {
    result += getPopCount(*(p++));
  }
  const BYTE frac = i%_BS_BITSINATOM;
  if(frac) {
    result += getPopCount(*p & (((Atom)1 << frac)-1));
  }
  return result;
}

size_t BitSet::getCount(size_t from, size_t to) const {
  if(to >= m_capacity) {
    to = m_capacity - 1;
  }
  if(from > to) return 0;
  const Atom *first = m_p + _BS_ATOMINDEX(from);
  const Atom *last  = m_p + _BS_ATOMINDEX(to  );

  if(first < last) {
    size_t result,f1;
    if(f1=from % _BS_BITSINATOM) {
      result = getPopCount(*(first++) & ~_BS_MASKATOM(f1));
    } else {
      result = 0;
    }

    if((to+1) % _BS_BITSINATOM) {
      result += getPopCount(*(last--) & _BS_MASKATOM(to%_BS_BITSINATOM+1));
    }

    while(first <= last) {
      result += getPopCount(*(first++));
    }
    return result;
  } else {
    return getPopCount(*first & (~_BS_MASKATOM(from%_BS_BITSINATOM) & _BS_MASKATOM(to%_BS_BITSINATOM+1)));
  }
}

bool BitSet::isEmpty() const {
  const Atom *p = m_p;
#if !defined(BITSET_ASM_OPTIMIZED)
  for(const Atom *endp = p + _BS_ATOMCOUNT(m_capacity); p < endp;) {
    if(*(p++)) {
      return false;
    }
  }
  return true;

#else // BITSET_ASM_OPTIMIZED
  size_t atomCount = getAtomCount();
#if defined(IS32BIT)
  __asm {
    pushf
    mov ecx, atomCount
    mov edi, p
    xor eax, eax
    cld
    repe scasd
    jnz nonZeroFound
    mov atomCount, 1
    jmp end
nonZeroFound:
    mov atomCount, 0
end:
    popf
  }
  return atomCount != 0;

#else // IS64BIT
  return bitSetIsEmpty(p, atomCount);
#endif // IS64BIT

#endif // BITSET_ASM_OPTIMIZED

}

BitSet &BitSet::operator+=(const BitSet &rhs) { // this = this union rhs
  if(m_capacity < rhs.m_capacity) {
    setCapacity(rhs.m_capacity);
  }
  Atom *p = m_p;
  for(const Atom *rp = rhs.m_p, *endp = rp + _BS_ATOMCOUNT(rhs.m_capacity); rp < endp;) {
    *(p++) |= *(rp++);
  }
  return *this;
}

BitSet &BitSet::operator-=(const BitSet &rhs) { // this = this - rhs
  Atom       *p  = m_p;
  const Atom *rp = rhs.m_p;
  const size_t atomCount1 = _BS_ATOMCOUNT(m_capacity), atomCount2 = _BS_ATOMCOUNT(rhs.m_capacity);
  if(atomCount1 <= atomCount2) {
    for(const Atom *endp = p + atomCount1; p < endp;) {
      *(p++) &= ~*(rp++);
    }
  } else {
    for(const Atom *endp = rp + atomCount2; rp < endp;) {
      *(p++) &= ~*(rp++);
    }
  }
  return *this;
}

BitSet &BitSet::operator*=(const BitSet &rhs) { // this = this and rhs (intersection)
  Atom       *p  = m_p;
  const Atom *rp = rhs.m_p;
  const size_t atomCount1 = _BS_ATOMCOUNT(m_capacity), atomCount2 = _BS_ATOMCOUNT(rhs.m_capacity);
  if(atomCount1 <= atomCount2) {
    for(const Atom *endp = p + atomCount1; p < endp;) {
      *(p++) &= *(rp++);
    }
  } else {
    for(const Atom *endp = rp + atomCount2; rp < endp;) {
      *(p++) &= *(rp++);
    }
    memset(p, 0, (m_p + atomCount1 - p) * sizeof(Atom));
  }
  return *this;
}

BitSet &BitSet::operator^=(const BitSet &rhs) { // this = this xor rhs
  if(m_capacity < rhs.m_capacity) {
    setCapacity(rhs.m_capacity);
  }

  Atom *p = m_p;
  for(const Atom *rp = rhs.m_p, *endp = rp + _BS_ATOMCOUNT(rhs.m_capacity); rp < endp;) {
    *(p++) ^= *(rp++);
  }
  return *this;
}

bool operator<=(const BitSet &lts, const BitSet &rhs) {
  if(lts.m_capacity <= rhs.m_capacity) {
    for(const BitSet::Atom *lp = lts.m_p, *rp = rhs.m_p, *endp = lp + _BS_ATOMCOUNT(lts.m_capacity); lp < endp; lp++) {
      if((*lp & *(rp++)) != *lp) {
        return false;
      }
    }
  } else { // lts.m_capacity > rhs.m_capacity
    const BitSet::Atom *lp = lts.m_p;
    for(const BitSet::Atom *rp = rhs.m_p, *endp = rp + _BS_ATOMCOUNT(rhs.m_capacity); rp < endp; lp++) {
      if((*lp & *(rp++)) != *lp) {
        return false;
      }
    }
    for(const BitSet::Atom *endp = lts.m_p + _BS_ATOMCOUNT(lts.m_capacity); lp < endp;) {
      if(*(lp++)) {
        return false;
      }
    }
  }
  return true;
}

ULONG BitSet::hashCode() const {
  size_t v = m_capacity;
  for(const Atom *p = m_p, *endp = p + _BS_ATOMCOUNT(m_capacity); p < endp;) {
    v ^= *(p++);
  }
  return sizetHash(v);
}

int bitSetCmp(const BitSet &i1, const BitSet &i2) {
  const size_t atomcount1 = _BS_ATOMCOUNT(i1.m_capacity);
  const size_t atomcount2 = _BS_ATOMCOUNT(i2.m_capacity);
  if(atomcount1 == atomcount2) {
    return memcmp(i1.m_p,i2.m_p, atomcount1 * sizeof(BitSet::Atom));
  } else {
    const int c = memcmp(i1.m_p,i2.m_p, min(atomcount1, atomcount2) * sizeof(BitSet::Atom));
    if(c) {
      return c;
    }
    // the tail of the biggest set must be 0
    const BitSet::Atom *p, *endp;
    if(atomcount1 < atomcount2) {
      p    = i2.m_p + atomcount1;
      endp = i2.m_p + atomcount2;
    } else {
      p    = i1.m_p + atomcount2;
      endp = i1.m_p + atomcount1;
    }
    while(p < endp) {
      if(*(p++)) {
        return (atomcount1 < atomcount2) ? -1 : 1;
      }
    }
  }
  return 0;
}
