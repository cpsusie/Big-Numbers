#include "pch.h"
#include <String.h>
#include <BitSet.h>
#include <Random.h>

#define BITSINATOM      (sizeof(BitSet::Atom)*8)
#define ATOMCOUNT(size) (((size)-1) / BITSINATOM + 1)
#define ATOMINDEX(i)    ((i)/BITSINATOM)
#define MASKATOM(i)     BitSet::mask[i]

#define ASM_OPTIMIZED

Iterator<unsigned int> BitSet::getIterator() {
  return Iterator<unsigned int>(new BitSetIterator(*this));
}

Iterator<unsigned int> BitSet::getReverseIterator() {
  return Iterator<unsigned int>(new BitSetReverseIterator(*this));
}

// -----------------------------------------------------------------------
AbstractIterator *BitSetIterator::clone() {
  return new BitSetIterator(*this);
}

void BitSetIterator::first() {
  const unsigned long atomCount = ATOMCOUNT(m_s.getCapacity());
  const BitSet::Atom *first = m_s.m_p;
  const BitSet::Atom *p     = first;
  const BitSet::Atom *last  = first + atomCount - 1;

#ifndef ASM_OPTIMIZED
  for(; p <= last; p++) {
    if(*p) {
      break;
    }
  }
  if(p > last) {
    m_hasNext = false;
    return;
  }
  m_hasNext = true;
  for(unsigned int j = 0;; j++) {
    if(*p & ATOMBIT(j)) {
      m_next = (p-first) * BITSINATOM + j;
      break;
    }
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
  if(!m_hasNext) {
    noNextElementError(_T("BitSetIterator"));
  }

  m_current = m_next++;
  const unsigned long atomCount = ATOMCOUNT(m_s.getCapacity());
  unsigned long i = ATOMINDEX(m_next), j = m_next % BITSINATOM;
  if(j) {
    const BitSet::Atom q = m_s.m_p[i] & ~MASKATOM(j);
    if(q) {

#ifndef ASM_OPTIMIZED
      for(;;j++) {
        if(q & ATOMBIT(j)) {
          m_next = i * BITSINATOM + j;
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

  const BitSet::Atom *p = m_s.m_p + i;

#ifndef ASM_OPTIMIZED
  for(; i < atomCount; i++, p++) {
    if(*p) {
      break;
    }
  }
  if(i == atomCount) {
    m_hasNext = false;
    return &m_current;
  }
  for(j = 0;; j++) {
    if(*p & ATOMBIT(j)) {
      m_next = i * BITSINATOM + j;
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

void BitSetIterator::remove() {
  m_s.remove(m_current);
}

// ---------------------------------------------------------------------

AbstractIterator *BitSetReverseIterator::clone() {
  return new BitSetReverseIterator(*this);
}

void BitSetReverseIterator::first() { // actually last
  const unsigned long atomCount = ATOMCOUNT(m_s.getCapacity());
  const BitSet::Atom *start = m_s.m_p;
  const BitSet::Atom *p     = start + atomCount-1;

#ifndef ASM_OPTIMIZED
  for(;p >= start; p--) {
    if(*p) {
      break;
    }
  }
  if(p < start) {
    m_hasNext = false;
    return;
  }
  m_hasNext = true;
  for(int j = BITSINATOM - 1;; j--) {
    if(*p & ATOMBIT(j)) {
      m_next = (p-start) * BITSINATOM + j;
      break;
    }
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
  if(!m_hasNext) {
    noNextElementError(_T("BitSetReverseIterator"));
  }
  if((m_current = m_next) > 0) {
    m_next--;
  } else  {
    m_hasNext = false;
    return &m_current;
  }

  unsigned long i = ATOMINDEX(m_next), j = m_next % BITSINATOM + 1;
  if(j < BITSINATOM) {
    const BitSet::Atom q = m_s.m_p[i] & MASKATOM(j);
    if(q) {

#ifndef ASM_OPTIMIZED
      for(;;j--) {
        if(q & ATOMBIT(j)) {
          m_next = i * BITSINATOM + j;
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

  const BitSet::Atom *start = m_s.m_p;
  const BitSet::Atom *p     = start + i;

#ifndef ASM_OPTIMIZED
  for(;p >= start; p--) {
    if(*p) {
      break;
    }
  }
  if(p < start) {
    m_hasNext = false;
    return &m_current;
  }

  for(j = BITSINATOM - 1;; j--) {
    if(*p & ATOMBIT(j)) {
      m_next = (p-start) * BITSINATOM + j;
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

void BitSetReverseIterator::remove() {
  m_s.remove(m_current);
}
