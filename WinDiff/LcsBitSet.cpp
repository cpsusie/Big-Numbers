#include "stdafx.h"
#include <BitSet.h>
#include "LcsBitSet.h"

LcsBitSet::LcsBitSet(const LineArray &a, const LineArray &b, LcsComparator &cmp)
: Lcs(cmp, nullptr)
{
  m_X = m_Y = nullptr;
  init(a, b);
}

LcsBitSet::~LcsBitSet() {
  clear();
}

void LcsBitSet::clear() {
  Lcs::clear();
  SAFEDELETEARRAY(m_X);
  SAFEDELETEARRAY(m_Y);
}

void LcsBitSet::init(const LineArray &a, const LineArray &b) {
  if(b.size() < a.size()) {
    init1(b,a);
    m_seqReversed = true;
  } else {
    init1(a,b);
    m_seqReversed = false;
  }
}

void LcsBitSet::init1(const LineArray &a, const LineArray &b) { // assume b.size >= a.size
  CompactArray<LcsElement> A, B;

  m_m = a.size();
  m_n = b.size();

  A.add(LcsElement(-1,EMPTYSTRING)); // add a dummy-LcsElement to make the real elements from [1..n]
  B.add(LcsElement(-1,EMPTYSTRING)); // do

  for(size_t i = 0; i < m_m; i++) {
    A.add(LcsElement((int)i+1, a[i]));
  }
  for(size_t i = 0; i < m_n; i++) {
    B.add(LcsElement((int)i+1, b[i]));
  }
  IndexComparator cmp(m_cmp);

  A.sort(1, A.size()-1, cmp);
  B.sort(1, B.size()-1, cmp);

  m_X = new UINT[m_m+1]; TRACE_NEW(m_X);
  m_Y = new UINT[m_n+1]; TRACE_NEW(m_Y);
  LcsElement *lastA = nullptr;
  LcsElement *lastB = nullptr;

#define samesymbol(e) ((lastA && e.m_s == lastA->m_s) || (lastB && e.m_s == lastB->m_s))

  m_symbolCount = 0;
  for(size_t ia = 1, ib = 1; ia < A.size() || ib < B.size(); ) {
    if(ia >= A.size()) {  // no more A
      if(!samesymbol(B[ib])) {
        m_symbolCount++;
      }
      m_Y[B[ib].m_index] = m_symbolCount;
      lastB = &B[ib];
      ib++;
    } else if(ib >= B.size()) { // no more B
      if(!samesymbol(A[ia])) {
        m_symbolCount++;
      }
      m_X[A[ia].m_index] = m_symbolCount;
      lastA = &A[ia];
      ia++;
    } else {
      int c = m_cmp.compare(A[ia].m_s,B[ib].m_s);
      if(c < 0) {
        if(!samesymbol(A[ia])) {
          m_symbolCount++;
        }
        m_X[A[ia].m_index] = m_symbolCount;
        lastA = &A[ia];
        ia++;
      } else if(c > 0) {
        if(!samesymbol(B[ib])) {
          m_symbolCount++;
        }
        m_Y[B[ib].m_index] = m_symbolCount;
        lastB = &B[ib];
        ib++;
      } else { // c == 0
        if(!samesymbol(A[ia])) {
          m_symbolCount++;
        }
        m_X[A[ia].m_index] = m_symbolCount;
        m_Y[B[ib].m_index] = m_symbolCount;
        lastA = &A[ia];
        lastB = &B[ib];
        ia++; ib++;
      }
    }
  }
  allocateTreshAndLinkArrays(m_m+1);
}

class BitSetWithPlus : public BitSet {
public:
  BitSetWithPlus(UINT size);
  BitSetWithPlus operator+(const BitSetWithPlus &rhs) const;
  BitSetWithPlus(BitSet &src);
};

BitSetWithPlus::BitSetWithPlus(UINT size) : BitSet(size) {
}

BitSetWithPlus::BitSetWithPlus(BitSet &src) : BitSet(src) {
}

#pragma message("Assume _BS_BITSINATOM == 32")

BitSetWithPlus BitSetWithPlus::operator+(const BitSetWithPlus &rhs) const {
  size_t s1 = getAtomCount();
  size_t s2 = rhs.getAtomCount();
  BitSetWithPlus result(1);
  const BitSet::Atom *a;
  size_t n;
  if(s1 >= s2) {
    result = *this;
    a = rhs.m_p;
    n = s2;
  } else {
    result = rhs;
    a = m_p;
    n = s1;
  }
  BitSet::Atom *r  = result.m_p;
  UINT carry = 0;
#if defined(IS32BIT)
#define ASM_OPTIMIZED
#endif

#if !defined(ASM_OPTIMIZED)
  for(size_t i = n; i--;) {
    unsigned __int64 tmp = (unsigned __int64)(*r) + *(a++) + carry;
    *(r++) = (BitSet::Atom )(tmp & 0xFFFFFFFF);
    carry  = (UINT)(tmp >> 32);
  }
#else
  __asm {
    mov      ecx, n
    mov      eax, a
    mov      ebx, r
    xor      edx, edx
forloop:
    cmp      ecx, 0
    je       exitloop
    sub      ecx, 1

    xor      edi, edi
    mov      esi, dword ptr [ebx]
    add      esi, dword ptr [eax]
    adc      edi, 0
    add      esi, edx
    mov      dword ptr[ebx], esi
    mov      edx, edi
    add      eax, 4
    add      ebx, 4
    jmp      forloop
exitloop:
    mov      carry, edx
  }
#endif

  if(carry) {
    if(result.getCapacity() % 32 == 0) {
      result.setCapacity(result.getCapacity() + 1);
    }
    result.add((result.getAtomCount()-1) * 32);
  }
  return result;
}

//#define DEBUG_PLUS
#if defined(DEBUG_PLUS)
void test() {
  BitSetWithPlus a(64),b(64);
  a.invert();
  b = a;
  a.dump(stdout);
  b.dump(stdout);
  BitSetWithPlus c = a + b;
c.dump(stdout);
exit(0);
}
#endif

void LcsBitSet::findLcs(ElementPairArray &result) {
#if defined(DEBUG_PLUS)
  test();
#endif
  // step 2 initialize the TRESH Array
  const UINT mmp1 = (UINT)m_m+1;
  for(size_t i = 1; i <= m_m; i++) {
    m_tresh[i] = mmp1;
  }

  Array<BitSetWithPlus> M,Mm;
  M.add(BitSetWithPlus(1));
  Mm.add(BitSetWithPlus(1));
  for(size_t i = 1; i <= m_symbolCount; i++) {
    M.add(BitSetWithPlus(mmp1));
  }
  for(size_t i = 1; i <= m_m; i++) {
    M[m_X[i]].add(i);
  }

  for(size_t i = 1; i <= m_symbolCount; i++) {
    Mm.add(compl(M[i]));
  }
  BitSetWithPlus S(mmp1);
  S.invert();

  for(UINT j = 1; j <= m_n; j++) {
    BitSetWithPlus SS = S.operator+(S & M[m_Y[j]]) | (S & Mm[m_Y[j]]);
    BitSetWithPlus K = (S ^ SS) & S;

    for(Iterator<size_t> it = K.getReverseIterator(); it.hasNext(); ) {
      const UINT i = (UINT)it.next();
      const UINT k = findK(i);
      m_tresh[k] = i;
      m_link[k]  = newLink(i,j,m_link[k-1]);
    }
    S = SS;
  }

  intptr_t k;
  for(k = m_m; k >= 0; k--) {
    if(m_tresh[k] < mmp1) {
      break;
    }
  }
  CompactArray<Link*> pairs;
  for(Link *ptr = m_link[k]; ptr != nullptr; ptr = ptr->m_next) {
    pairs.add(ptr);
  }

  const size_t totalPairCount = pairs.size();

  result.setCapacity(totalPairCount);
  if(m_seqReversed) {
    for(size_t k = totalPairCount; k--;) {
      result.add(ElementPair(pairs[k]->m_j-1,pairs[k]->m_i-1));
    }
  } else {
    for(size_t k = pairs.size(); k--;) {
      result.add(ElementPair(pairs[k]->m_i-1,pairs[k]->m_j-1));
    }
  }
}

UINT LcsBitSet::findK(UINT i) const { // find k:m_tresh[k-1] < i <= m_tresh[k]. ie min(k:m_tresh[k] <= i)
  UINT l = 1;
  UINT r = (UINT)m_m;
  while(l < r) {
    const UINT mid = (l+r)>>1;
    if(m_tresh[mid] < i) {
      l = mid + 1;
    } else {               // m_tresh[mid] >= i
      r = mid;
    }
  }
  return r;
}
