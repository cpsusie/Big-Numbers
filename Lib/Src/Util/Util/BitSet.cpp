#include "pch.h"
#include <String.h>
#include <BitSet.h>
#include <Random.h>

#define BITSINATOM      (sizeof(BitSet::Atom)*8)
#define ATOMCOUNT(size) (((size)-1) / BITSINATOM + 1)
#define ATOMINDEX(i)    ((i)/BITSINATOM)
#define BYTECOUNT(size) (((size)-1) / 8 + 1)
#define BYTEINDEX(i)    ((i)/8)
#define ATOM(p,i)       p[ATOMINDEX(i)]
#define ATOMBIT(i)      ((BitSet::Atom)1<< ((i)%BITSINATOM))

#define ASM_OPTIMIZED

DEFINECLASSNAME(BitSet);

BitSet::Atom BitSet::mask[_BITSET_ATOMSIZE+1]; // Array of atoms with number of 1-bits = index, index = [0;_BITSET_ATOMSIZE]

#pragma warning(disable : 4073)
#pragma init_seg(lib)

class InitBitSetMask {
public:
  InitBitSetMask();
};

InitBitSetMask::InitBitSetMask() {
  BitSet::mask[0] = 0;
  for(int i = 1; i <= _BITSET_ATOMSIZE; i++) {
    BitSet::mask[i] = ATOMBIT(i-1) | BitSet::mask[i-1];
  }

//#define MASK(i) ((i==32)?(-1):((1<<(i))-1))

//  for(i = 0; i <= BITSINATOM; i++)
//    if(mask[i] != MASK(i))
//      throwException("mask[%d] != MASK(%d)",i,i);
/*
  for(i = 0; i <= BITSINATOM; i++) {
    cout << "mask[" << dec << i << "]:" << hex << mask[i] << ((i % 2 == 1) ? "\n" : " ");
  }
  cout.flush();
*/
}

static InitBitSetMask maskInitializer;

#define MASKATOM(i) mask[i]

int BitSet::getAtomCount() const {
  return ATOMCOUNT(m_capacity);
}

unsigned long BitSet::getAtomIndex(unsigned long i) const {
  return ATOMINDEX(i);
}

int BitSet::getAtomCount(unsigned long capacity) { // static
  return ATOMCOUNT(capacity);
}


BitSet::BitSet(unsigned long capacity) {
  if(capacity == 0) {
    throwMethodInvalidArgumentException(s_className, _T("BitSet"), _T("Capacity=%d"),capacity);
  }
  m_capacity = capacity;
  const unsigned long atomCount = ATOMCOUNT(m_capacity);
  m_p = new Atom[atomCount];
  memset(m_p,0,atomCount * sizeof(Atom));
}

BitSet::~BitSet() {
  delete[] m_p;
}

BitSet::BitSet(const BitSet &set) {
  m_capacity = set.m_capacity;
  const unsigned long atomCount = ATOMCOUNT(m_capacity);
  m_p = new Atom[atomCount];
  memcpy(m_p,set.m_p,atomCount * sizeof(Atom)); 
}

BitSet &BitSet::operator=(const BitSet &rhs) {
  if(this == &rhs) {
    return *this;
  }
  const unsigned long atomCount = ATOMCOUNT(rhs.m_capacity);
  if(rhs.m_capacity != m_capacity) {
    delete[] m_p;
    m_capacity = rhs.m_capacity;
    m_p = new Atom[atomCount];
  }
  memcpy(m_p,rhs.m_p,atomCount * sizeof(Atom));

  return *this;
}

void BitSet::setCapacity(unsigned long newCapacity) {
  const unsigned long newAtomCount = ATOMCOUNT(newCapacity);
  const unsigned long oldAtomCount = ATOMCOUNT(m_capacity );
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

BitSet &BitSet::clear() {
  const unsigned long atomCount = ATOMCOUNT(m_capacity);
  memset(m_p,0,atomCount * sizeof(Atom));
  return *this;
}

BitSet &BitSet::remove(unsigned long i) {
  if(i >= m_capacity) {
    throwMethodInvalidArgumentException(s_className, _T("remove"), _T("Index %lu out of range. Capacity=%lu"), i, m_capacity);
  }
  ATOM(m_p,i) &= ~ATOMBIT(i);
  return *this;
}

BitSet &BitSet::add(unsigned long i) {
  if(i >= m_capacity) {
    throwMethodInvalidArgumentException(s_className, _T("add"), _T("Index %lu out of range. Capacity=%lu"), i, m_capacity);
  }
  ATOM(m_p,i) |= ATOMBIT(i);
  return *this;
}

BitSet &BitSet::remove(unsigned long a, unsigned long b) {
  if(b >= m_capacity) {
    throwMethodInvalidArgumentException(s_className, _T("remove"), _T("(%lu,%lu):Index %lu out of range. Capacity=%lu"), a, b, b, m_capacity);
  }
  if(a > b) {
    return *this;
  }
  unsigned long aIndex = ATOMINDEX(a);
  unsigned long bIndex = ATOMINDEX(b);

  if(aIndex < bIndex) {
    if(a % BITSINATOM) { 
      m_p[aIndex] &= MASKATOM(a%BITSINATOM);
      aIndex++;
    }
    if((b+1) % BITSINATOM) {
      m_p[bIndex] &= ~MASKATOM(b%BITSINATOM + 1);
      bIndex--;
    }
    if(bIndex >= aIndex) {
      memset(m_p+aIndex, 0, (bIndex-aIndex+1)*sizeof(Atom));
    }
  } else { // aIndex == bIndex
    m_p[aIndex] &= MASKATOM(a%BITSINATOM) | ~MASKATOM(b%BITSINATOM + 1);
  }
  return *this;
}

BitSet &BitSet::add(unsigned long a, unsigned long b) {
  if(b >= m_capacity) {
    throwMethodInvalidArgumentException(s_className, _T("add"), _T("(%lu,%lu):Index %lu out of range. Capacity=%lu"), a, b, b, m_capacity);
  }
  if(a > b) {
    return *this;
  }
  unsigned long aIndex = ATOMINDEX(a);
  unsigned long bIndex = ATOMINDEX(b);

  if(aIndex < bIndex) {
    if(a % BITSINATOM) { 
      m_p[aIndex] |= ~MASKATOM(a%BITSINATOM);
      aIndex++;
    }
    if((b+1) % BITSINATOM) {
      m_p[bIndex] |= MASKATOM(b%BITSINATOM + 1);
      bIndex--;
    }

    if(bIndex >= aIndex) {
      memset(m_p+aIndex, ~0, (bIndex-aIndex+1)*sizeof(Atom));
    }
  } else {
    m_p[aIndex] |= ~MASKATOM(a%BITSINATOM) & MASKATOM(b%BITSINATOM + 1);
  }
  return *this;
}

bool BitSet::contains(unsigned long i) const {
  if(i >= m_capacity) {
    return false;
  }
  return (ATOM(m_p,i) & ATOMBIT(i)) ? true : false;
}

unsigned int BitSet::select() const {
  const unsigned int i = selectRandomNonEmptyAtom();
  Atom p = m_p[i];
  unsigned char bits[BITSINATOM];
  Atom mask;
  int count = 0;
  for (int j = 0; p; j++) {
    if(mask = p & ATOMBIT(j)) {
      bits[count++] = j;
      p &= ~mask;
    }
  }
  return i * BITSINATOM + bits[randInt() % count];
}

unsigned int BitSet::selectRandomNonEmptyAtom() const {
  const unsigned int a = ATOMCOUNT(m_capacity);
  if(randInt() % 2) {
    for(unsigned long c = a, i = randInt() % a; c--; i = (i+1) % a) {
      if(m_p[i]) {
        return i;
      }
    }
  } else {
    for(unsigned long c = a, i = randInt() % a; c--;) {
      if(m_p[i]) {
        return i;
      }
      if(i-- == 0) i = a-1;
    }
  }
  throwMethodException(s_className, _T("select"), _T("BitSet is empty"));
  return -1;
}

BitSet &BitSet::invert() {
  Atom *p = m_p;
  for(unsigned long i = ATOMCOUNT(m_capacity); i--; p++) {
    *p = ~*p;
  }

  if(m_capacity % BITSINATOM) {  // cut the last if neccessary
    p--;
    *p &= MASKATOM(m_capacity%BITSINATOM);
  }
  return *this;
}

BitSet compl(const BitSet &s) {
  BitSet result(s);
  return result.invert();
}

/* old version of count. 
   see below for a faster function 
   (count in average 3 times faster for random sets,
                     6 times faster for full sets
                     2 times slower for empty sets.
*/

unsigned long BitSet::oldsize() const {
  const Atom *p = m_p;
  unsigned long result = 0;
  for (unsigned long i = ATOMCOUNT(m_capacity); i--;) {
	  for(Atom a = *(p++); a; a &= (a-1)) {
      result++;
    }
  }
  return result;
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

unsigned long BitSet::size() const {
  const unsigned char *p = (const unsigned char*)m_p;
  unsigned long result = 0;
  for(unsigned long i = BYTECOUNT(m_capacity); i--;) {
    result += setBitsCount[*(p++)];
  }
  return result;
}

int BitSet::getIndex(unsigned long i) const {
  if(!contains(i)) {
    return -1;
  }
  const unsigned char *p = (const unsigned char*)m_p;
  const bool frac = (i%8) ? true : false;
  long result = 0;
  for(unsigned long j = BYTEINDEX(i)+(frac?1:0); j--;) {
    result += setBitsCount[*(p++)];
  }
  if(frac) {
    p--;
    result -= setBitsCount[*p & ~((1 << (i%8))-1)];
  }
  return result;
}

unsigned int BitSet::getCount(unsigned int from, unsigned int to) const {
  if(to >= m_capacity) {
    to = m_capacity;
  }
  if(from > to) {
    return 0;
  }
  unsigned long fromIndex = BYTEINDEX(from);
  unsigned long toIndex   = BYTEINDEX(to  );
  const unsigned char *p = (const unsigned char*)m_p;

  if(fromIndex < toIndex) {
    unsigned long result;
    if(from % 8) {
      result = setBitsCount[p[fromIndex] & ~MASKATOM(from%8)];
      fromIndex++;
    } else {
      result = 0;
    }

    if((to+1) % 8) {
      result += setBitsCount[p[toIndex] & MASKATOM(to%8+1)];
      toIndex--;
    }

    int j = toIndex - fromIndex + 1;
    if(j > 0) {
      for(p += fromIndex; j--;) {
        result += setBitsCount[*(p++)];
      }
    }
    return result;
  } else {
    return setBitsCount[p[fromIndex] & (~MASKATOM(from%8) & MASKATOM(to%8+1))];
  }
}

bool BitSet::isEmpty() const {
  Atom *p = m_p;
  unsigned long i = ATOMCOUNT(m_capacity);
#ifndef ASM_OPTIMIZED
  for(; i--;) {
    if(*(p++)) {
      return false;
    }
  }
  return true;

#else
#if _BITSET_ATOMSIZE == 32
  __asm {
    pushf
    mov ecx, i
    mov edi, p
    xor eax, eax
    cld
    repe scasd
    jnz nonZeroFound
    mov i, 1
    jmp end
nonZeroFound:
    mov i, 0
end:
    popf
  }
#elif _BITSET_ATOMSIZE == 16
  __asm {
    pushf
    mov ecx, i
    mov edi, p
    xor eax, eax
    cld
    repe scasw
    jnz nonZeroFound
    mov i, 1
    jmp end
nonZeroFound:
    mov i, 0
end:
    popf
  }
#else  // ASM_OPTIMIZED
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE

  return i != 0;

#endif // ASM_OPTIMIZED

}

BitSet &BitSet::operator+=(const BitSet &rhs) { // this = this union rhs
  const unsigned long ratomCount = ATOMCOUNT(rhs.m_capacity);

  if(ATOMCOUNT(m_capacity) < ratomCount) {
    setCapacity(rhs.m_capacity);
  }
  Atom *p        = m_p;
  const Atom *rp = rhs.m_p;
  for(unsigned long i = ratomCount; i--;) {
    *(p++) |= *(rp++);
  }
  return *this;
}

BitSet &BitSet::operator-=(const BitSet &rhs) { // this = this - rhs
  const unsigned long minCapacity  = min(m_capacity,rhs.m_capacity);
  const unsigned long minAtomCount = ATOMCOUNT(minCapacity);

  Atom *p        = m_p;
  const Atom *rp = rhs.m_p;
  for(unsigned long i = minAtomCount; i--;) {
    *(p++) &= ~*(rp++);
  }
  return *this;
}

BitSet &BitSet::operator*=(const BitSet &rhs) { // this = this and rhs (intersection)
  const unsigned long minCapacity  = min(m_capacity, rhs.m_capacity);
  const unsigned long minAtomCount = ATOMCOUNT(minCapacity);
  Atom *p  = m_p;
  Atom *rp = rhs.m_p;
  for(unsigned long i = minAtomCount; i--;) {
    *(p++) &= *(rp++);
  }
  for(unsigned long i = ATOMCOUNT(m_capacity) - minAtomCount; i--;) {
    *(p++) = 0;
  }
  return *this;
}

BitSet &BitSet::operator^=(const BitSet &rhs) { // this = this xor rhs
  if(rhs.m_capacity > m_capacity) {
    setCapacity(rhs.m_capacity);
  }

  const unsigned long atomCount = ATOMCOUNT(rhs.m_capacity);
  Atom *p  = m_p;
  Atom *rp = rhs.m_p;
  for(unsigned long i = atomCount; i--;) {
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

BitSet operator-(const BitSet &lts, const BitSet &rhs) { // difference
  BitSet result(lts);
  result -= rhs;
  return result;
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
    const unsigned long atomCount = ATOMCOUNT(lts.m_capacity);
    for(unsigned long i = 0; i < atomCount; i++) {
      if((lts.m_p[i] & rhs.m_p[i]) != lts.m_p[i]) {
        return false;
      }
    }
  } else { // lts.m_capacity > rhs.m_capacity
    unsigned long atomCount = ATOMCOUNT(rhs.m_capacity);
    unsigned long i;
    for(i = 0; i < atomCount; i++) {
      if((lts.m_p[i] & rhs.m_p[i]) != lts.m_p[i]) {
        return false;
      }
    }
    atomCount = ATOMCOUNT(lts.m_capacity);
    for(;i < atomCount; i++) {
      if(lts.m_p[i]) {
        return false;
      }
    }
  }
  return true;
}

bool operator<(const BitSet &lts, const BitSet &rhs) {
  return (lts <= rhs) && (lts != rhs);
}

bool operator>=(const BitSet &lts, const BitSet &rhs) {
  return rhs <= lts;
}

bool operator>(const BitSet &lts, const BitSet &rhs) {
  return rhs < lts;
}

unsigned long BitSet::hashCode() const {
  unsigned long v = m_capacity;
  const Atom *p = m_p;
  for(unsigned long i = ATOMCOUNT(m_capacity); i--;) {
    v ^= *(p++);
  }
  return v;
}

bool operator==(const BitSet &lts, const BitSet &rhs) {
  return bitSetCmp(lts,rhs) == 0;
}

bool operator!=(const BitSet &lts, const BitSet &rhs) {
  return bitSetCmp(lts,rhs) != 0;
}

int bitSetCmp(const BitSet &i1, const BitSet &i2) {
  const unsigned long atomcount1 = ATOMCOUNT(i1.m_capacity);
  const unsigned long atomcount2 = ATOMCOUNT(i2.m_capacity);
  if(atomcount1 == atomcount2) {
    return memcmp(i1.m_p,i2.m_p,atomcount1 * sizeof(BitSet::Atom));
  } else {
    const unsigned long minAtomCount = min(atomcount1,atomcount2);
    const unsigned long maxAtomCount = max(atomcount1,atomcount2);
    int c = memcmp(i1.m_p,i2.m_p,minAtomCount * sizeof(BitSet::Atom));
    if(c) {
      return c;
    }
    // the rest must be 0
    const BitSet::Atom *p = (minAtomCount == atomcount1) ? i2.m_p+minAtomCount : i1.m_p+minAtomCount;
    for(unsigned long i = maxAtomCount - minAtomCount; i--;) {
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
  String result = "(";
  Iterator<unsigned int> it = ((BitSet*)this)->getIterator();
  if(it.hasNext()) {
    result += format(_T("%lu"), it.next());
    while(it.hasNext()) {
      result += ',';
      result += format(_T("%lu"), it.next());
    }
  }
  result += ")";
  return result;
}

static char *sprintbin(char *s, BitSet::Atom p) {
  char *t = s;
  for(int i = BITSINATOM; i--;) {
    *(t++) = (p & 1) ? '1' : '0';
    p >>= 1;
  }
  *t = '\0';
  return s; // strrev(tmp);
}

tostream &BitSet::dump(tostream &s) {
  Atom *p = m_p;
  char tmp[BITSINATOM+1];
  for(unsigned long i = ATOMCOUNT(m_capacity); i--;) {
    s << sprintbin(tmp, *(p++)) << " ";
  }
  s.flush();
  return s;
}

void BitSet::dump(FILE *f) {
  Atom *p = m_p;
  char tmp[BITSINATOM+1];
  for(unsigned long i = ATOMCOUNT(m_capacity); i--;) {
    fprintf(f,"%s ",sprintbin(tmp, *(p++)));
  }
  fprintf(f,"\n");
}

void BitSet::getRangeTable(CompactArray<unsigned int> &rangeTable, unsigned char shift) const {
  rangeTable.clear();
  const unsigned int stepSize     = 1 << shift;
  unsigned int       currentLimit = stepSize;
  unsigned int       counter      = 0;
  for(BitSetIterator it((BitSet&)(*this)); it.hasNext();) {
    const unsigned int &e = *(const unsigned int*)it.next();
    if(++counter >= currentLimit) {
  
//      printf("hashTable[%3d]:%11s -> %s\n", m_rangeTable.size(), format1000(currentLimit).cstr(), format1000(e).cstr());

      rangeTable.add(e+1);
      currentLimit += stepSize;
    }
  }
}
