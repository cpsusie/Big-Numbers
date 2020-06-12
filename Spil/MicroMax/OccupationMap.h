#pragma once

//#define __USETABLECONVERSION

#if defined(__USETABLECONVERSION)

#define POSTOINDEX(p) PlayerOccupationMap::posToIndex[p]
#define INDEXTOPOS(i) PlayerOccupationMap::indexToPos[i]

#else

#define POSTOINDEX(p) (((p) & 7) | (((p) & 0x70)>>1))
#define INDEXTOPOS(i) (((i) & 7) | (((i) & 0x38)<<1))

#endif

class PlayerOccupationMap {
private:
  friend class PieceIterator;
  UINT64 m_set;
  inline void setBit(BYTE index) {
    m_set |= ((UINT64)1<<index);
  }
  inline void clrBit(BYTE index) {
    m_set &= ~((UINT64)1<<index);
  }
public:
#if defined(__USETABLECONVERSION)
  static const char posToIndex[120], indexToPos[64];
#endif
  inline void add(BYTE pos) {
    setBit(POSTOINDEX(pos));
  }
  inline void remove(BYTE pos) {
    clrBit(POSTOINDEX(pos));
  }
  inline bool contains(BYTE index) const {
    return (m_set & ((UINT64)1<<index)) != 0;
  }
  inline void clear() {
    m_set = 0;
  }
  inline UINT64 &getBits(char index) {
    return m_set;
  }
  String toString() const;
};

class PieceIterator {
private:
  const UINT        *m_set; // dont change layout of this class. see asm-code below
  int                m_start, m_next;
  bool               m_hasNext;
#if defined(_DEBUG)
  const PlayerOccupationMap &m_map; // must be last
#endif

public:
  inline PieceIterator(const PlayerOccupationMap &map, int startPos)
    : m_set((UINT*)&map.m_set)
#if defined(_DEBUG)
    , m_map(map)
#endif
  {
    __asm {
        mov esi, this
        mov ecx, startPos
        mov ebx, ecx
        and ebx, 0x70
        shr ebx, 1
        and ecx, 7
        or  ecx, ebx                    // ecx = POSTOINDEX(startPos)
        shr ebx, 3                      //
        and ebx, 4                      // ebx = (index = (POSTOINDEX(startPos) / 32)) * 4
        and ecx, 31                     // ecx = bit    = (POSTOINDEX(startPos) % 32
                                        //
        mov edi, DWORD PTR [esi]        // edi = m_set
        mov edx, DWORD PTR [edi+ebx]    // edx = m_set[index]
        test cl, cl                     //
        je TestBitZero                  // if(bit != 0) {
        mov eax, 1                      //
        shl eax, cl                     //
        dec eax                         //
        not eax                         //    eax = mask = ~((1<<bit)-1) = highend bits of edx
        and edx, eax                    //    edx &= mask
                                        //
//    TestBitNotZero:                   // most common case
        test edx, edx                   //
        jne SearchBit                   //    if(edx) goto SearchBit
	      xor ebx, 4                      //    index ^= 1
        mov edx, DWORD PTR [edi+ebx]    //    edx = m_set[index]
        test edx, edx                   //
        jne SearchBit                   //
        xor ebx, 4                      //    index ^= 1
        mov edx, DWORD PTR [edi+ebx]    //    edx = m_set[index]
        test edx, edx                   //
        jne SearchBit                   // }
                                        //
      EmptySet:                         //
        mov BYTE PTR [esi+12], 0        // m_hasNext = false
        jmp End                         //
                                        //
      TestBitZero:                      //
        test edx, edx                   //
        jne SearchBit                   // if(edx) goto SearchBit
	      xor ebx, 4                      // index ^= 1
        mov edx, DWORD PTR [edi+ebx]    // edx = m_set[index]
        test edx, edx
        je EmptySet

      SearchBit:                        // Invariant: edx != 0. Find first 1-bit in edx
        bsf eax, edx
        shl ebx, 3
        or eax, ebx                     // eax = 32 * index + "index of first 1-bit of m_set[index]"
        mov DWORD PTR [esi+4], eax      // m_start   = eax
        mov DWORD PTR [esi+8], eax      // m_next    = eax
        mov BYTE  PTR [esi+12], 1       // m_hasNext = true
      End:
    }
  }

  inline bool hasNext() const {
    return m_hasNext;
  }

  inline char next() {
    char result;
    __asm {
        mov esi, this
        mov ecx, DWORD PTR[esi+8]       // ecx = m_next
        mov result, cl                  //
        inc ecx                         //
        and ecx, 63                     // ecx = (m_next+1)%64
        mov ebx, ecx                    //
        shr ebx, 3                      //
        and ebx, 4                      // ebx = (index = (ecx / 32)) * 4
        and ecx, 31                     // ecx = bit    =  ecx % 32
                                        //
        mov edi, DWORD PTR [esi]        // edi = m_set
        mov edx, DWORD PTR [edi+ebx]    // edx = m_set[index]
        test cl, cl                     //
        je TestBitZero                  // if(bit != 0) {
        mov eax, 1                      //
        shl eax, cl                     //
        dec eax                         //
        not eax                         //    eax = mask = ~((1<<bit)-1) = highend bits of edx
        and edx, eax                    //    edx &= mask
                                        //
//    TestBitNotZero:                   // most common case
        test edx, edx                   //
        jne SearchBit                   //    if(edx != 0) goto SearchBit
        xor ebx, 4                      //    index ^= 1
        mov edx, DWORD PTR [edi+ebx]    //    edx = m_set[index]
        test edx, edx                   //
        jne SearchBit                   //
        xor ebx, 4                      //    index ^= 1
        mov edx, DWORD PTR [edi+ebx]    //    edx = m_set[index]
        test edx, edx                   //
        jne SearchBit                   //  }
                                        //
      EndIteration:                     //
        mov BYTE PTR [esi+12], 0        // m_hasNext = false
        jmp End                         //
                                        //
      TestBitZero:                      // if(bit == 0) dont waste time to check this element again as above
        test edx, edx                   //
        jne SearchBit                   // if(edx) goto SearchBit
	      xor ebx, 4                      // index ^= 1
        mov edx, DWORD PTR [edi+ebx]    // edx = m_set[index]
        test edx, edx                   //
        je EndIteration                 //
                                        //
      SearchBit:                        // Invariant: edx != 0. Find first 1-bit in edx
        bsf eax, edx                    //
        shl ebx, 3                      //
        or  eax, ebx                    // eax = 32 * index + "index of first 1-bit of m_set[index]" = next occupied position
        mov ebx, DWORD PTR [esi+4]      // ebx = m_start
        cmp eax, ebx                    //
        je  EndIteration                // if(eax == m_start) goto EndIteration
        mov DWORD PTR [esi+8], eax      // m_next = eax
      End:
    }
    return result;
  }
};

class OccupationMap {
public:
  PlayerOccupationMap m_playerMap[2];
  void clear();
  String toString() const;
};
