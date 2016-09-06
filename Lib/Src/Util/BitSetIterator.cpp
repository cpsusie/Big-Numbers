#include "pch.h"
#include <String.h>
#include <BitSet.h>
#include <Random.h>

#define BITSINATOM      (sizeof(BitSet::Atom)*8)
#define ATOMINDEX(i)    ((i)/BITSINATOM)
#define MASKATOM(i)     BitSet::mask[i]
#define ATOMBIT(i)      ((BitSet::Atom)1<<((i)%BITSINATOM))

Iterator<size_t> BitSet::getIterator(size_t start, size_t end) {
  return Iterator<size_t>(new BitSetIterator(*this, start, end));
}

Iterator<size_t> BitSet::getReverseIterator(size_t start, size_t end) {
  return Iterator<size_t>(new BitSetReverseIterator(*this, start, end));
}

// -----------------------------------------------------------------------

DEFINECLASSNAME(BitSetIterator);

AbstractIterator *BitSetIterator::clone() {
  return new BitSetIterator(*this);
}
#ifdef BITSET_ASM_OPTIMIZED
#ifdef IS64BIT
extern "C" {
  intptr_t bitSetIteratorFirst(       const BitSet::Atom *first, size_t start, size_t end);
  intptr_t bitSetIteratorNext(        const BitSet::Atom *first, size_t next , size_t end);
  intptr_t bitSetReverseIteratorFirst(const BitSet::Atom *first, size_t start, size_t end);
  intptr_t bitSetReverseIteratorNext( const BitSet::Atom *first, size_t next , size_t end);
};
#endif
#endif

void BitSetIterator::first(size_t start, size_t end) {
  m_end                         = min(m_s.getCapacity()-1, end);
  setCurrentUndefined();
  if(start > m_end) {
    m_hasNext = false;
    return;
  }

#ifndef BITSET_ASM_OPTIMIZED
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(m_end);
  const BitSet::Atom *q         = p;

SearchFirst1Bit:
  while((*q==0) && (q++ < endAtom));

  if(q > endAtom) {
    m_hasNext = false;
    return;
  }
  if(q > p) {
    const BitSet::Atom a = *q;
    for(size_t j = 0;; j++) {
      if(a & ATOMBIT(j)) {
        m_hasNext = (m_next = (q-firstAtom) * BITSINATOM + j) <= m_end;
        return;
      }
    }
  } else {
    size_t j = start % BITSINATOM;
    const BitSet::Atom a = *q & ~MASKATOM(j);
    if(a) {
      for(;;j++) {
        if(a & ATOMBIT(j)) {
          m_hasNext = (m_next = (q-firstAtom) * BITSINATOM + j) <= m_end;
          return;
        }
      }
    }
    q++;
    goto SearchFirst1Bit;
  }

#else // BITSET_ASM_OPTIMIZED

#ifdef IS32BIT
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(m_end);
  end                           = m_end;
  unsigned long i;
  int hasNext;

  __asm {
    pushf
    mov edi, p
    cld
SearchNonZeroAtom:
    mov ecx, endAtom
    sub ecx, edi
    add ecx, 4
    sar ecx, 2
    xor eax, eax
    repe scasd
    je NoNext
    sub edi, 4
    cmp edi, endAtom
    ja NoNext
    mov edx, dword ptr[edi]
    cmp edi, p
    ja  ScanBitsForward
    mov ecx, start        ; mask edx &= ~MASKATOM(start % BITSINATOM)
    and ecx, 31
    mov eax, 1
    shl eax, cl
    dec eax
    not eax
    and edx, eax          ; edx = highend bits of first nonzero DWORD
    jne ScanBitsForward
    add edi, 4
    jmp SearchNonZeroAtom
ScanBitsForward:
    bsf eax, edx
    sub edi, firstAtom
    shl edi, 3
    add edi, eax
    cmp edi, end
    jbe Success
NoNext:
    mov hasNext, 0
    jmp End
Success:
    mov i      , edi
    mov hasNext, 1
End:
    popf
  }
  if(m_hasNext = (hasNext?true:false)) {
    m_next = i;
  }
#else // IS64BIT
  m_hasNext = ((intptr_t)(m_next = bitSetIteratorNext(m_s.m_p, start, m_end))) >= 0;
#endif // IS64BIT

#endif // BITSET_ASM_OPTIMIZED
}

void *BitSetIterator::next() { // throw Exception if no more
  if(!m_hasNext) noNextElementError(s_className);
  if((m_current = m_next++) >= m_end) {
    m_hasNext = false;
    return &m_current;
  }

#ifndef BITSET_ASM_OPTIMIZED
  size_t i = ATOMINDEX(m_next), j = m_next % BITSINATOM;
  if(j) {
    const BitSet::Atom a = m_s.m_p[i] & ~MASKATOM(j);
    if(a) {
      for(;;j++) {
        if(a & ATOMBIT(j)) {
          m_hasNext = (m_next = i * BITSINATOM + j) <= m_end;
          return &m_current;
        }
      }
    }
    i++;
  }
  const BitSet::Atom *p       = m_s.m_p + i;
  const BitSet::Atom *endAtom = m_s.m_p + ATOMINDEX(m_end);

  while((*p==0) && (p++ < endAtom));
  if(p > endAtom) {
    m_hasNext = false;
    return &m_current;
  }
  const BitSet::Atom a = *p; // a != 0
  for(j = 0;; j++) {
    if(a & ATOMBIT(j)) {
      m_hasNext = (m_next = (p - m_s.m_p) * BITSINATOM + j) <= m_end;
      return &m_current;
    }
  }

#else // BITSET_ASM_OPTIMIZED

#ifdef IS32BIT
  size_t i = ATOMINDEX(m_next), j = m_next % BITSINATOM;
  const BitSet::Atom *firstAtom = m_s.m_p;
  const size_t        end       = m_end;
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(end);
  int                 hasNext;

  __asm {
    mov ecx, j
    or  ecx, ecx
    je  InitEDIAndSearchNonZeroAtom
    mov edi, firstAtom
    mov eax, i
    shl eax, 2
    add edi, eax
    mov edx, dword ptr[edi]
    mov eax, 1
    shl eax, cl
    dec eax
    not eax
    and edx, eax
    jz SearchNextNonZeroAtom

    bsf eax, edx
    sub edi, firstAtom
    shl edi, 3
    add edi, eax
    cmp edi, end
    ja  NoNextNoPop
    mov i  , edi
    mov hasNext, 1
    jmp End
NoNextNoPop:
    mov hasNext, 0
    jmp End
NoNextPop:
    mov hasNext, 0
    jmp PopEnd
InitEDIAndSearchNonZeroAtom:
    mov edi, firstAtom
    mov eax, i
    shl eax, 2
    add edi, eax
    jmp SearchNonZeroAtom
SearchNextNonZeroAtom:
    add edi, 4
SearchNonZeroAtom:
    pushf
    mov ecx, endAtom
    sub ecx, edi
    add ecx, 4
    sar ecx, 2
    xor eax, eax
    cld
    repe scasd
    jz NoNextPop

    sub edi, 4
    mov edx, dword ptr[edi]
    bsf eax, edx
    sub edi, firstAtom
    shl edi, 3
    add edi, eax
    cmp edi, end
    ja  NoNextPop
    mov i  , edi
    mov hasNext, 1
PopEnd:
    popf
End:
  }
  if(hasNext) {
    m_next = i;
  } else {
    m_hasNext = false;
  }
  return &m_current;

#else // IS64BIT
  m_hasNext = ((intptr_t)(m_next = bitSetIteratorNext(m_s.m_p, m_next, m_end))) >= 0;
  return &m_current;
#endif // IS64BIT

#endif // BITSET_ASM_OPTIMIZED
}

// ---------------------------------------------------------------------

DEFINECLASSNAME(BitSetReverseIterator);

AbstractIterator *BitSetReverseIterator::clone() {
  return new BitSetReverseIterator(*this);
}

void BitSetReverseIterator::first(size_t start, size_t end) { // actually last
  start                         = min(start, m_s.getCapacity()-1);
  m_end                         = end;
  setCurrentUndefined();
  if(start < m_end) {
    m_hasNext = false;
    return;
  }

#ifndef BITSET_ASM_OPTIMIZED

  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(m_end);
  const BitSet::Atom *q         = p;

SearchRFirst1Bit:
  while((*q == 0) && (q-- > endAtom));

  if(q < endAtom) {
    m_hasNext = false;
    return;
  }

  if(q < p) {
    const BitSet::Atom a = *q; // a != 0
    for(size_t j = BITSINATOM - 1;; j--) {
      if(a & ATOMBIT(j)) {
        m_hasNext = (m_next = (q - firstAtom) * BITSINATOM + j) >= m_end;
        return;
      }
    }
  } else {
    size_t j = start % BITSINATOM;
    const BitSet::Atom a = *q & MASKATOM(j+1);
    if(a) {
      for(;;j--) {
        if(a & ATOMBIT(j)) {
          m_hasNext = (m_next = (q - firstAtom) * BITSINATOM + j) >= m_end;
          return;
        }
      }
    }
    q--;
    goto SearchRFirst1Bit;
  }

#else // BITSET_ASM_OPTIMIZED

#ifdef IS32BIT
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(m_end);
  end                           = m_end;
  unsigned long i;
  int hasNext;

  __asm {
    pushf
    mov edi, p
    std
SearchRNonZeroAtom:
    mov ecx, edi
    sub ecx, endAtom
    add ecx, 4
    sar ecx, 2
    xor eax, eax
    repe scasd
    je NoNext
    add edi, 4
    cmp edi, endAtom
    jb NoNext
    mov edx, dword ptr[edi]
    cmp edi, p
    jb ScanBitsBackward
                    
    mov ecx, start          ; mask edx &= MASKATOM(start % BITSINATOM + 1)
    and ecx, 31
    cmp ecx, 31
    je  ScanBitsBackward    ; if start%32+1==32 no masking, all bits should be searched
    inc ecx
    mov eax, 1
    shl eax, cl
    dec eax
    and edx, eax            ; edx = lowend bits of first nonzero DWORD
    jne ScanBitsBackward
    sub edi, 4
    jmp SearchRNonZeroAtom
ScanBitsBackward:
    bsr eax, edx
    sub edi, firstAtom
    shl edi, 3
    add edi, eax
    cmp edi, end
    jae Success
NoNext:
    mov hasNext, 0
    jmp End
Success:
    mov i, edi
    mov hasNext, 1
End:
    popf
  }
  if(m_hasNext = (hasNext?true:false)) {
    m_next = i;
  }
#else // IS64BIT
  m_hasNext = ((intptr_t)(m_next = bitSetReverseIteratorNext(m_s.m_p, start, m_end))) >= 0;
#endif // IS64BIT

#endif // BITSET_ASM_OPTIMIZED
}

void *BitSetReverseIterator::next() { // Actually previous. throw Exception if no next element
  if(!hasNext()) noNextElementError(s_className);
  if((m_current = m_next--) <= m_end) {
    m_hasNext = false;
    return &m_current;
  }

#ifndef BITSET_ASM_OPTIMIZED
  size_t i = ATOMINDEX(m_next), j = m_next % BITSINATOM + 1;
  if(j < BITSINATOM) {
    const BitSet::Atom a = m_s.m_p[i] & MASKATOM(j);
    if(a) {
      for(;;j--) {
        if(a & ATOMBIT(j)) {
          m_hasNext = (m_next = i * BITSINATOM + j) >= m_end;
          return &m_current;
        }
      }
    }
    i--;
  }

  const BitSet::Atom *p         = m_s.m_p + i;
  const BitSet::Atom *endAtom   = m_s.m_p + ATOMINDEX(m_end);

  while((*p == 0) && (p-- > endAtom));
  if(p < endAtom) {
    m_hasNext = false;
    return &m_current;
  }
  const BitSet::Atom a = *p; // a != 0
  for(j = BITSINATOM - 1;; j--) {
    if(a & ATOMBIT(j)) {
      m_hasNext = (m_next = (p - m_s.m_p) * BITSINATOM + j) >= m_end;
      return &m_current;
    }
  }

#else // BITSET_ASM_OPTIMIZED

#ifdef IS32BIT
  size_t i = ATOMINDEX(m_next), j = m_next % BITSINATOM + 1;
  const BitSet::Atom *firstAtom = m_s.m_p;
  const size_t        end       = m_end;
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(end);
  int                 hasNext;

  __asm {
    mov ecx, j
    cmp ecx, 32
    je InitEDIAndSearchRNonZeroAtom
    mov edi, firstAtom
    mov eax, i
    shl eax, 2
    add edi, eax
    mov edx, dword ptr[edi]
    mov eax, 1
    shl eax, cl
    dec eax
    and edx, eax
    jz SearchRNextNonZeroAtom

    bsr eax, edx
    sub edi, firstAtom
    shl edi, 3
    add edi, eax
    cmp edi, end
    jb NoNextNoPop
    mov i  , edi
    mov hasNext, 1
    jmp End
NoNextNoPop:
    mov hasNext, 0
    jmp End
NoNextPop:
    mov hasNext, 0
    jmp PopEnd
InitEDIAndSearchRNonZeroAtom:
    mov edi, firstAtom
    mov eax, i
    shl eax, 2
    add edi, eax
    jmp SearchRNonZeroAtom
SearchRNextNonZeroAtom:
    sub edi, 4
SearchRNonZeroAtom:
    pushf
    mov ecx, edi
    sub ecx, endAtom
    add ecx, 4
    sar ecx, 2
    xor eax, eax
    std
    repe scasd
    jz NoNextPop

    add edi, 4
    mov edx, dword ptr[edi]
    bsr eax, edx
    sub edi, firstAtom
    shl edi, 3
    add edi, eax
    cmp edi, end
    jb NoNextPop
    mov i  , edi
    mov hasNext, 1
PopEnd:
    popf
End:
  }
  if(hasNext) {
    m_next = i;
  } else {
    m_hasNext = false;
  }
  return &m_current;
#else // IS64BIT
  m_hasNext = ((intptr_t)(m_next = bitSetReverseIteratorNext(m_s.m_p, m_next, m_end))) >= 0;
  return &m_current;
#endif // IS64BIT

#endif // BITSET_ASM_OPTIMIZED
}
