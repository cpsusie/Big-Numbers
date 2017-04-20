#include "pch.h"
#include <String.h>
#include <BitSet.h>
#include <Random.h>

#ifdef IS64BIT
#ifdef BITSET_ASM_OPTIMIZED
extern "C" {
  bool bitSetIsEmpty(const void *p, size_t atomCount);
};
#endif // BITSET_ASM_OPTIMIZED
#endif // IS64BIT

String BitSet::indexOutOfRangeString(size_t index) const {
  return format(_T("Index %s out of range. Capacity=%s"), format1000(index).cstr(), format1000(m_capacity).cstr());
}

void BitSet::throwIndexOutOfRange(const TCHAR *method, size_t index, const TCHAR *format, ...) const {
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
  m_p = new Atom[atomCount];
  memset(m_p,0,atomCount * sizeof(Atom));
}

BitSet::BitSet(const BitSet &set) {
  m_capacity = set.m_capacity;
  const size_t atomCount = _BS_ATOMCOUNT(m_capacity);
  m_p = new Atom[atomCount];
  memcpy(m_p,set.m_p,atomCount * sizeof(Atom)); 
}

BitSet &BitSet::operator=(const BitSet &rhs) {
  if(this == &rhs) {
    return *this;
  }
  const size_t atomCount = _BS_ATOMCOUNT(rhs.m_capacity);
  if(rhs.m_capacity != m_capacity) {
    delete[] m_p;
    m_capacity = rhs.m_capacity;
    m_p = new Atom[atomCount];
  }
  memcpy(m_p,rhs.m_p,atomCount * sizeof(Atom));

  return *this;
}

void BitSet::setCapacity(size_t newCapacity) {
  const size_t newAtomCount = _BS_ATOMCOUNT(newCapacity);
  const size_t oldAtomCount = _BS_ATOMCOUNT(m_capacity );
  if(newAtomCount != oldAtomCount) {
    Atom *p = new Atom[newAtomCount];
    if(newAtomCount > oldAtomCount) {
      memcpy(p, m_p, oldAtomCount * sizeof(Atom));
      memset(p + oldAtomCount, 0, (newAtomCount-oldAtomCount) * sizeof(Atom));
    } else {
      memcpy(p, m_p, newAtomCount * sizeof(Atom));
    }
    delete[] m_p;
    m_p = p;
  }
  m_capacity = newCapacity;
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

size_t BitSet::select() const {
  const size_t i = randSizet(m_capacity);
  for(Iterator<size_t> it = ((BitSet*)this)->getIterator(i); it.hasNext();) {
    return it.next();
  }
  for (Iterator<size_t> it = ((BitSet*)this)->getReverseIterator(i); it.hasNext();) {
    return it.next();
  }
  throwException(_T("Cannot select from empty BitSet"));
  return -1;
}

BitSet &BitSet::invert() {
  Atom *p = m_p;
  for(size_t i = _BS_ATOMCOUNT(m_capacity); i--; p++) {
    *p = ~*p;
  }

  if(m_capacity % _BS_BITSINATOM) {  // cut the last if neccessary
    p--;
    *p &= _BS_MASKATOM(m_capacity%_BS_BITSINATOM);
  }
  return *this;
}

BitSet compl(const BitSet &s) {
  BitSet result(s);
  return result.invert();
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
  const Atom *last = getLastAtom();
  for (const Atom *p = m_p; p <= last;) {
    result += getPopCount(*(p++));
  }
  return result;
}

intptr_t BitSet::getIndex(size_t i) const {
  if(!contains(i)) return -1;
  const Atom *p    = m_p, *last = p + _BS_ATOMINDEX(i);
  size_t result = 0;
  while(p < last) {
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
    to = m_capacity;
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

#ifndef BITSET_ASM_OPTIMIZED
  const Atom *last = getLastAtom();
  while(p <= last) {
    if(*(p++)) return false;
  }
  return true;

#else // BITSET_ASM_OPTIMIZED
  size_t atomCount = getAtomCount();
#ifdef IS32BIT
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
  const size_t ratomCount = _BS_ATOMCOUNT(rhs.m_capacity);

  if(m_capacity < rhs.m_capacity) {
    setCapacity(rhs.m_capacity);
  }
  Atom *p        = m_p;
  const Atom *rp = rhs.m_p;
  for(size_t i = ratomCount; i--;) {
    *(p++) |= *(rp++);
  }
  return *this;
}

BitSet &BitSet::operator-=(const BitSet &rhs) { // this = this - rhs
  const size_t minCapacity  = min(m_capacity,rhs.m_capacity);
  const size_t minAtomCount = _BS_ATOMCOUNT(minCapacity);

  Atom       *p  = m_p;
  const Atom *rp = rhs.m_p;
  for(size_t i = minAtomCount; i--;) {
    *(p++) &= ~*(rp++);
  }
  return *this;
}

BitSet &BitSet::operator*=(const BitSet &rhs) { // this = this and rhs (intersection)
  const size_t minCapacity  = min(m_capacity, rhs.m_capacity);
  const size_t minAtomCount = _BS_ATOMCOUNT(minCapacity);
  Atom *p  = m_p;
  Atom *rp = rhs.m_p;
  for(size_t i = minAtomCount; i--;) {
    *(p++) &= *(rp++);
  }
  for(size_t i = _BS_ATOMCOUNT(m_capacity) - minAtomCount; i--;) {
    *(p++) = 0;
  }
  return *this;
}

BitSet &BitSet::operator^=(const BitSet &rhs) { // this = this xor rhs
  if(rhs.m_capacity > m_capacity) {
    setCapacity(rhs.m_capacity);
  }

  const size_t atomCount = _BS_ATOMCOUNT(rhs.m_capacity);
  Atom *p  = m_p;
  Atom *rp = rhs.m_p;
  for(size_t i = atomCount; i--;) {
    *(p++) ^= *(rp++);
  }
  return *this;
}

BitSet operator+(const BitSet &lts, const BitSet &rhs) { // union
  if(lts.m_capacity >= rhs.m_capacity) {
    BitSet result(lts);
    result += rhs;
    return result;
  } else {
    BitSet result(rhs);
    result += lts;
    return result;
  }
}

BitSet operator*(const BitSet &lts, const BitSet &rhs) { // intersection
  if(lts.m_capacity <= rhs.m_capacity) {
    BitSet result(lts);
    result *= rhs;
    return result;
  } else {
    BitSet result(rhs);
    result *= lts;
    return result;
  }
}

BitSet operator^(const BitSet &lts, const BitSet &rhs) { // xor, ie symmetric difference
  if(lts.m_capacity >= rhs.m_capacity) {
    BitSet result(lts);
    result ^= rhs;
    return result;
  } else {
    BitSet result(rhs);
    result ^= lts;
    return result;
  }
}

bool operator<=(const BitSet &lts, const BitSet &rhs) {
  if(lts.m_capacity <= rhs.m_capacity) {
    const size_t atomCount = _BS_ATOMCOUNT(lts.m_capacity);
    for(size_t i = 0; i < atomCount; i++) {
      if((lts.m_p[i] & rhs.m_p[i]) != lts.m_p[i]) {
        return false;
      }
    }
  } else { // lts.m_capacity > rhs.m_capacity
    size_t atomCount = _BS_ATOMCOUNT(rhs.m_capacity);
    size_t i;
    for(i = 0; i < atomCount; i++) {
      if((lts.m_p[i] & rhs.m_p[i]) != lts.m_p[i]) {
        return false;
      }
    }
    atomCount = _BS_ATOMCOUNT(lts.m_capacity);
    for(;i < atomCount; i++) {
      if(lts.m_p[i]) {
        return false;
      }
    }
  }
  return true;
}

ULONG BitSet::hashCode() const {
  size_t v = m_capacity;
  const Atom *p = m_p;
  for(size_t i = _BS_ATOMCOUNT(m_capacity); i--;) {
    v ^= *(p++);
  }
  return sizetHash(v);
}

int bitSetCmp(const BitSet &i1, const BitSet &i2) {
  const size_t atomcount1 = _BS_ATOMCOUNT(i1.m_capacity);
  const size_t atomcount2 = _BS_ATOMCOUNT(i2.m_capacity);
  if(atomcount1 == atomcount2) {
    return memcmp(i1.m_p,i2.m_p,atomcount1 * sizeof(BitSet::Atom));
  } else {
    const size_t minAtomCount = min(atomcount1,atomcount2);
    const size_t maxAtomCount = max(atomcount1,atomcount2);
    int c = memcmp(i1.m_p,i2.m_p,minAtomCount * sizeof(BitSet::Atom));
    if(c) {
      return c;
    }
    // the rest must be 0
    const BitSet::Atom *p = (minAtomCount == atomcount1) ? i2.m_p+minAtomCount : i1.m_p+minAtomCount;
    for(size_t i = maxAtomCount - minAtomCount; i--;) {
      if(*(p++)) {
        return (minAtomCount == atomcount1) ? -1 : 1;
      }
    }
  }
  return 0;
}

tostream& operator<<(tostream &s, const BitSet &rhs) {
  return s << rhs.toString();
}

String BitSet::toString() const {
  String result = _T("(");
  TCHAR tmp[40];
  Iterator<size_t> it = ((BitSet*)this)->getIterator();
  if(it.hasNext()) {
    result += _i64tot(it.next(), tmp, 10);
    while(it.hasNext()) {
      result += _T(',');
      result += _i64tot(it.next(), tmp, 10);
    }
  }
  result += _T(")");
  return result;
}

static TCHAR *sprintbin(TCHAR *s, BitSet::Atom p) {
  TCHAR *t = s;
  for(int i = _BS_BITSINATOM; i--;) {
    *(t++) = (p & 1) ? '1' : '0';
    p >>= 1;
  }
  *t = _T('\0');
  return s; // strrev(tmp);
}

tostream &BitSet::dump(tostream &s) const {
  const Atom *p = m_p;
  TCHAR tmp[_BS_BITSINATOM+1];
  for(size_t i = _BS_ATOMCOUNT(m_capacity); i--;) {
    s << sprintbin(tmp, *(p++)) << _T(" ");
  }
  s.flush();
  return s;
}

void BitSet::dump(FILE *f) const {
  const Atom *p = m_p;
  TCHAR tmp[_BS_BITSINATOM+1];
  for(size_t i = _BS_ATOMCOUNT(m_capacity); i--;) {
    _ftprintf(f,_T("%s "),sprintbin(tmp, *(p++)));
  }
  _ftprintf(f,_T("\n"));
}

void BitSet::getRangeTable(CompactInt64Array &rangeTable, BYTE shift) const {
  rangeTable.clear();
  const UINT         stepSize     = 1 << shift;
  size_t             currentLimit = stepSize;
  size_t             counter      = 0;
  for(BitSetIterator it((BitSet&)(*this)); it.hasNext();) {
    const size_t &e = *(const size_t*)it.next();
    if(++counter >= currentLimit) {
  
//      printf("hashTable[%3d]:%11s -> %s\n", m_rangeTable.size(), format1000(currentLimit).cstr(), format1000(e).cstr());

      rangeTable.add(e+1);
      currentLimit += stepSize;
    }
  }
}

#define BYTECOUNT(size) (((size)-1) / 8 + 1)
#define BYTEINDEX(i)    ((i)/8)

size_t BitSet::oldSize() const {
  const BYTE *p = (const BYTE*)m_p;
  size_t result = 0;
  for(size_t i = BYTECOUNT(m_capacity); i--;) {
    result += setBitsCount[*(p++)];
  }
  return result;
}

intptr_t BitSet::oldGetIndex(size_t i) const {
  if(!contains(i)) {
    return -1;
  }
  const BYTE *p = (const BYTE*)m_p;
  const bool frac = (i%8) ? true : false;
  long result = 0;
  for(size_t j = BYTEINDEX(i)+(frac?1:0); j--;) {
    result += setBitsCount[*(p++)];
   }
  if(frac) {
    p--;
    result -= setBitsCount[*p & ~((1 << (i%8))-1)];
  }
  return result;
}

size_t BitSet::oldGetCount(size_t from, size_t to) const {
  if(to >= m_capacity) {
    to = m_capacity;
  }
  if(from > to) {
    return 0;
  }
  size_t fromIndex = BYTEINDEX(from);
  size_t toIndex   = BYTEINDEX(to  );
  const BYTE *p = (const BYTE*)m_p;
 
  if(fromIndex < toIndex) {
    size_t result;
    if(from % 8) {
      result = setBitsCount[p[fromIndex] & ~_BS_MASKATOM(from%8)];
      fromIndex++;
    } else {
      result = 0;
    }
 
    if((to+1) % 8) {
      result += setBitsCount[p[toIndex] & _BS_MASKATOM(to%8+1)];
      toIndex--;
    }
 
    intptr_t j = toIndex - fromIndex + 1;
    if(j > 0) {
      for(p += fromIndex; j--;) {
        result += setBitsCount[*(p++)];
      }
    }
    return result;
  } else {
    return setBitsCount[p[fromIndex] & (~_BS_MASKATOM(from%8) & _BS_MASKATOM(to%8+1))];
  }
}
