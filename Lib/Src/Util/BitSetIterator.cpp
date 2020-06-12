#include "pch.h"
#include <String.h>
#include <BitSet.h>
#include <Random.h>

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
#if defined(BITSET_ASM_OPTIMIZED)
#if defined(IS64BIT)
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

#if !defined(BITSET_ASM_OPTIMIZED)
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + _BS_ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + _BS_ATOMINDEX(m_end);
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
      if(a & _BS_ATOMBIT(j)) {
        m_hasNext = (m_next = (q-firstAtom) * _BS_BITSINATOM + j) <= m_end;
        return;
      }
    }
  } else {
    size_t j = start % _BS_BITSINATOM;
    const BitSet::Atom a = *q & ~_BS_MASKATOM(j);
    if(a) {
      for(;;j++) {
        if(a & _BS_ATOMBIT(j)) {
          m_hasNext = (m_next = (q-firstAtom) * _BS_BITSINATOM + j) <= m_end;
          return;
        }
      }
    }
    q++;
    goto SearchFirst1Bit;
  }

#else // BITSET_ASM_OPTIMIZED

#if defined(IS32BIT)
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + _BS_ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + _BS_ATOMINDEX(m_end);
  end                           = m_end;
  ULONG i;
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
    mov ecx, start        ; mask edx &= ~_BS_MASKATOM(start % _BS_BITSINATOM)
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

#if !defined(BITSET_ASM_OPTIMIZED)
  size_t i = _BS_ATOMINDEX(m_next), j = m_next % _BS_BITSINATOM;
  if(j) {
    const BitSet::Atom a = m_s.m_p[i] & ~_BS_MASKATOM(j);
    if(a) {
      for(;;j++) {
        if(a & _BS_ATOMBIT(j)) {
          m_hasNext = (m_next = i * _BS_BITSINATOM + j) <= m_end;
          return &m_current;
        }
      }
    }
    i++;
  }
  const BitSet::Atom *p       = m_s.m_p + i;
  const BitSet::Atom *endAtom = m_s.m_p + _BS_ATOMINDEX(m_end);

  while((*p==0) && (p++ < endAtom));
  if(p > endAtom) {
    m_hasNext = false;
    return &m_current;
  }
  const BitSet::Atom a = *p; // a != 0
  for(j = 0;; j++) {
    if(a & _BS_ATOMBIT(j)) {
      m_hasNext = (m_next = (p - m_s.m_p) * _BS_BITSINATOM + j) <= m_end;
      return &m_current;
    }
  }

#else // BITSET_ASM_OPTIMIZED

#if defined(IS32BIT)
  size_t i = _BS_ATOMINDEX(m_next), j = m_next % _BS_BITSINATOM;
  const BitSet::Atom *firstAtom = m_s.m_p;
  const size_t        end       = m_end;
  const BitSet::Atom *endAtom   = firstAtom + _BS_ATOMINDEX(end);
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

#if !defined(BITSET_ASM_OPTIMIZED)

  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + _BS_ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + _BS_ATOMINDEX(m_end);
  const BitSet::Atom *q         = p;

SearchRFirst1Bit:
  while((*q == 0) && (q-- > endAtom));

  if(q < endAtom) {
    m_hasNext = false;
    return;
  }

  if(q < p) {
    const BitSet::Atom a = *q; // a != 0
    for(size_t j = _BS_BITSINATOM - 1;; j--) {
      if(a & _BS_ATOMBIT(j)) {
        m_hasNext = (m_next = (q - firstAtom) * _BS_BITSINATOM + j) >= m_end;
        return;
      }
    }
  } else {
    size_t j = start % _BS_BITSINATOM;
    const BitSet::Atom a = *q & _BS_MASKATOM(j+1);
    if(a) {
      for(;;j--) {
        if(a & _BS_ATOMBIT(j)) {
          m_hasNext = (m_next = (q - firstAtom) * _BS_BITSINATOM + j) >= m_end;
          return;
        }
      }
    }
    q--;
    goto SearchRFirst1Bit;
  }

#else // BITSET_ASM_OPTIMIZED

#if defined(IS32BIT)
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + _BS_ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + _BS_ATOMINDEX(m_end);
  end                           = m_end;
  ULONG i;
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

    mov ecx, start          ; mask edx &= _BS_MASKATOM(start % _BS_BITSINATOM + 1)
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

#if !defined(BITSET_ASM_OPTIMIZED)
  size_t i = _BS_ATOMINDEX(m_next), j = m_next % _BS_BITSINATOM + 1;
  if(j < _BS_BITSINATOM) {
    const BitSet::Atom a = m_s.m_p[i] & _BS_MASKATOM(j);
    if(a) {
      for(;;j--) {
        if(a & _BS_ATOMBIT(j)) {
          m_hasNext = (m_next = i * _BS_BITSINATOM + j) >= m_end;
          return &m_current;
        }
      }
    }
    i--;
  }

  const BitSet::Atom *p         = m_s.m_p + i;
  const BitSet::Atom *endAtom   = m_s.m_p + _BS_ATOMINDEX(m_end);

  while((*p == 0) && (p-- > endAtom));
  if(p < endAtom) {
    m_hasNext = false;
    return &m_current;
  }
  const BitSet::Atom a = *p; // a != 0
  for(j = _BS_BITSINATOM - 1;; j--) {
    if(a & _BS_ATOMBIT(j)) {
      m_hasNext = (m_next = (p - m_s.m_p) * _BS_BITSINATOM + j) >= m_end;
      return &m_current;
    }
  }

#else // BITSET_ASM_OPTIMIZED

#if defined(IS32BIT)
  size_t i = _BS_ATOMINDEX(m_next), j = m_next % _BS_BITSINATOM + 1;
  const BitSet::Atom *firstAtom = m_s.m_p;
  const size_t        end       = m_end;
  const BitSet::Atom *endAtom   = firstAtom + _BS_ATOMINDEX(end);
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
