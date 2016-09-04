#include "pch.h"
#include <String.h>
#include <BitSet.h>
#include <Random.h>

#define BITSINATOM      (sizeof(BitSet::Atom)*8)
#define ATOMCOUNT(size) (((size)-1) / BITSINATOM + 1)
#define ATOMINDEX(i)    ((i)/BITSINATOM)
#define MASKATOM(i)     BitSet::mask[i]
#define ATOMBIT(i)      ((BitSet::Atom)1<< ((i)%BITSINATOM))

#ifdef IS32BIT
#define ASM_OPTIMIZED
#endif

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

void BitSetIterator::first(size_t start, size_t end) {
  m_end                         = min(m_s.getCapacity()-1, end);
  setCurrentUndefined();
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(m_end);

#ifndef ASM_OPTIMIZED
  const BitSet::Atom *q = p;

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

#else // ASM_OPTIMIZED

  unsigned long i;
  int hasNext;

#if _BITSET_ATOMSIZE == 32
  __asm {
    pushf
    mov ecx, atomCount
    mov edi, p
    xor eax, eax
    cld
    repe scasd
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    sub edi, 4
    inc ecx
    mov edx, DWORD PTR [edi]
    bsf eax, edx
    sub ecx, atomCount
    neg ecx
    shl ecx, 5
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#elif _BITSET_ATOMSIZE == 16
  __asm {
    pushf
    mov ecx, atomCount
    mov edi, p
    xor eax, eax
    cld
    repe scasw
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    sub edi, 2
    inc ecx
    xor edx, edx
    mov dx , WORD PTR [edi]
    bsf ax , dx
    sub ecx, atomCount
    neg ecx
    shl ecx, 4
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#else
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE
  if(m_hasNext = (hasNext?true:false)) {
    m_next = i;
  }
#endif // ASM_OPTIMIZED
}

void *BitSetIterator::next() { // throw Exception if no more
  if(!m_hasNext) noNextElementError(s_className);
  if((m_current = m_next++) >= m_end) {
    m_hasNext = false;
    return &m_current;
  }
  size_t i = ATOMINDEX(m_next), j = m_next % BITSINATOM;
  if(j) {
    const BitSet::Atom a = m_s.m_p[i] & ~MASKATOM(j);
    if(a) {

#ifndef ASM_OPTIMIZED
      for(;;j++) {
        if(a & ATOMBIT(j)) {
          m_hasNext = (m_next = i * BITSINATOM + j) <= m_end;
          break;
        }
      }
#else // ASM_OPTIMIZED
#if _BITSET_ATOMSIZE == 32
      __asm {
        mov edx, q
        bsf eax, edx
        mov edx, i
        shl edx, 5
        add edx, eax
        mov i  , edx
      }
#elif _BITSET_ATOMSIZE == 16
      __asm {
        xor edx, edx
        mov dx , q
        bsf ax , dx
        mov edx, i
        shl edx, 4
        add edx, eax
        mov i  , edx
      }
#else
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE
      m_next = i;
#endif // ASM_OPTIMIZED
      return &m_current;
    }
    i++;
  }

  const BitSet::Atom *p         = m_s.m_p + i;
  const BitSet::Atom *endAtom   = m_s.m_p + ATOMINDEX(m_end);

#ifndef ASM_OPTIMIZED
  while((*p==0) && (p++ < endAtom));

  if(p > endAtom) {
    m_hasNext = false;
    return &m_current;
  }
  const BitSet::Atom a = *p; // a != 0
  for(j = 0;; j++) {
    if(a & ATOMBIT(j)) {
      m_hasNext = (m_next = (p - m_s.m_p) * BITSINATOM + j) <= m_end;
      break;
    }
  }

#else // ASM_OPTIMIZED

  int hasNext;

#if _BITSET_ATOMSIZE == 32
  __asm {
    pushf
    mov ecx, atomCount
    sub ecx, i
    mov edi, p
    xor eax, eax
    cld
    repe scasd
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    sub edi, 4
    inc ecx
    mov edx, DWORD PTR [edi]
    bsf eax, edx
    sub ecx, atomCount
    neg ecx
    shl ecx, 5
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#elif _BITSET_ATOMSIZE == 16
  __asm {
    pushf
    mov ecx, atomCount
    sub ecx, i
    mov edi, p
    xor eax, eax
    cld
    repe scasw
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    sub edi, 2
    inc ecx
    xor edx, edx
    mov dx , WORD PTR [edi]
    bsf ax , dx
    sub ecx, atomCount
    neg ecx
    shl ecx, 4
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#else
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE 
  if(hasNext) {
    m_next = i;
  } else {
    m_hasNext = false;
  }
#endif // ASM_OPTIMIZED

  return &m_current;
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
  const BitSet::Atom *firstAtom = m_s.m_p;
  const BitSet::Atom *p         = firstAtom + ATOMINDEX(start);
  const BitSet::Atom *endAtom   = firstAtom + ATOMINDEX(m_end);

#ifndef ASM_OPTIMIZED
  const BitSet::Atom *q = p;

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

#else // ASM_OPTIMIZED

  unsigned long i;
  int hasNext;

#if _BITSET_ATOMSIZE == 32
  __asm {
    pushf
    mov ecx, atomCount
    mov edi, p
    xor eax, eax
    std
    repe scasd
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    add edi, 4
    mov edx, DWORD PTR [edi]
    bsr eax, edx
    shl ecx, 5
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#elif _BITSET_ATOMSIZE == 16
  __asm {
    pushf
    mov ecx, atomCount
    mov edi, p
    xor eax, eax
    std
    repe scasw
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    add edi, 2
    xor edx, edx
    mov dx , WORD PTR [edi]
    bsr ax , dx
    shl ecx, 4
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#else
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE
  if(m_hasNext = (hasNext?true:false)) {
    m_next = i;
  }
#endif // ASM_OPTIMIZED
}

void *BitSetReverseIterator::next() { // Actually previous. throw Exception if no next element
  if(!hasNext()) noNextElementError(s_className);
  if((m_current = m_next--) <= m_end) {
    m_hasNext = false;
    return &m_current;
  }

  size_t i = ATOMINDEX(m_next), j = m_next % BITSINATOM + 1;
  if(j < BITSINATOM) {
    const BitSet::Atom a = m_s.m_p[i] & MASKATOM(j);
    if(a) {

#ifndef ASM_OPTIMIZED
      for(;;j--) {
        if(a & ATOMBIT(j)) {
          m_hasNext = (m_next = i * BITSINATOM + j) >= m_end;
          break;
        }
      }
#else // ASM_OPTIMIZED
#if _BITSET_ATOMSIZE == 32
      __asm {
        mov edx, q
        bsr eax, edx
        mov edx, i
        shl edx, 5
        add edx, eax
        mov i  , edx
      }
#elif _BITSET_ATOMSIZE == 16
      __asm {
        xor edx, edx
        mov dx , q
        bsr ax , dx
        mov edx, i
        shl edx, 4
        add edx, eax
        mov i  , edx
      }
#else
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE 
      m_next = i;
#endif // ASM_OPTIMIZED
      return &m_current;
    }
    i--;
  }

  const BitSet::Atom *p         = m_s.m_p + i;
  const BitSet::Atom *endAtom   = m_s.m_p + ATOMINDEX(m_end);

#ifndef ASM_OPTIMIZED
  while((*p == 0) && (p-- > endAtom));
  if(p < endAtom) {
    m_hasNext = false;
    return &m_current;
  }
  const BitSet::Atom a = *p; // a != 0
  for(j = BITSINATOM - 1;; j--) {
    if(a & ATOMBIT(j)) {
      m_hasNext = (m_next = (p - m_s.m_p) * BITSINATOM + j) >= m_end;
      break;
    }
  }

#else // ASM_OPTIMIZED

  int hasNext;
    ;
#if _BITSET_ATOMSIZE == 32
  __asm {
    pushf
    mov ecx, i
    inc ecx
    mov edi, p
    xor eax, eax
    std
    repe scasd
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    add edi, 4
    mov edx, DWORD PTR [edi]
    bsr eax, edx
    shl ecx, 5
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#elif _BITSET_ATOMSIZE == 16
  __asm {
    pushf
    mov ecx, i
    inc ecx
    mov edi, p
    xor eax, eax
    std
    repe scasw
    jnz searchBit
    mov hasNext, 0
    jmp end
searchBit:
    add edi, 2
    xor edx, edx
    mov dx , WORD PTR [edi]
    bsr ax , dx
    shl ecx, 4
    add ecx, eax
    mov i  , ecx
    mov hasNext, 1
end:
    popf
  }
#else
#error "_BITSET_ATOMSIZE must be 16 or 32"
#endif // _BITSET_ATOMSIZE
  if(hasNext) {
    m_next = i;
  } else {
    m_hasNext = false;
  }
#endif // ASM_OPTIMIZED

  return &m_current;
}
