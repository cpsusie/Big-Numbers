#pragma once

extern const char _wkB1BkP2I[64];
extern const char _wkB1BkI2P[58];
extern const char _wkC1BkP2I[64];
extern const char _wkC1BkI2P[58];
extern const char _wkD1BkP2I[64];
extern const char _wkD1BkI2P[58];
extern const char _wkC2BkP2I[64];
extern const char _wkC2BkI2P[55];
extern const char _wkD2BkP2I[64];
extern const char _wkD2BkI2P[55];
extern const char _wkD3BkP2I[64];
extern const char _wkD3BkI2P[55];

#define MAXINDEX_WK_BELOWDIAGB1D1 (3*58)
#define MAXINDEX_WK_BELOWDIAGC2D3 (3*55)
#define MAXINDEX_WK_ONDIAGB2D4    (3*25)
#define MAXINDEX_WK_ONDIAGA1      (1*27)
#define MAXINDEX_KK_ONDIAGB2D4    (3*5 )
#define MAXINDEX_KK_ONDIAGA1      (1*6 )

#define START_RANGE_WK_BELOWDIAGB1D1         0
#define END_RANGE_WK_BELOWDIAGB1D1          (START_RANGE_WK_BELOWDIAGB1D1 + MAXINDEX_WK_BELOWDIAGB1D1)
#define START_RANGE_WK_BELOWDIAGC2D3         END_RANGE_WK_BELOWDIAGB1D1
#define END_RANGE_WK_BELOWDIAGC2D3          (START_RANGE_WK_BELOWDIAGC2D3 + MAXINDEX_WK_BELOWDIAGC2D3)
#define START_RANGE_WK_ONDIAGB2D4            END_RANGE_WK_BELOWDIAGC2D3
#define END_RANGE_WK_ONDIAGB2D4             (START_RANGE_WK_ONDIAGB2D4    + MAXINDEX_WK_ONDIAGB2D4   )
#define START_RANGE_WK_ONDIAGA1              END_RANGE_WK_ONDIAGB2D4
#define END_RANGE_WK_ONDIAGA1               (START_RANGE_WK_ONDIAGA1      + MAXINDEX_WK_ONDIAGA1     )
#define START_RANGE_KK_ONDIAGB2D4            END_RANGE_WK_ONDIAGA1
#define END_RANGE_KK_ONDIAGB2D4             (START_RANGE_KK_ONDIAGB2D4    + MAXINDEX_KK_ONDIAGB2D4   )
#define START_RANGE_KK_ONDIAGA1              END_RANGE_KK_ONDIAGB2D4
#define END_RANGE_KK_ONDIAGA1               (START_RANGE_KK_ONDIAGA1      + MAXINDEX_KK_ONDIAGA1     )

#define MIN_KINGS_INDEX                      0
#define KK_OFFDIAG_COUNT                     END_RANGE_WK_ONDIAGA1
#define KK_ONDIAG_COUNT                     (MAXINDEX_KK_ONDIAGB2D4       + MAXINDEX_KK_ONDIAGA1)

#define KK_WITH_PAWN_COUNT                   3612

#define START_RANGE_KK_ONDIAG                START_RANGE_KK_ONDIAGB2D4

#define KK_OFFDIAG_2MEN(       key)          EndGameKeyDefinition::encodeKKSym8(         key)
#define SETKK_OFFDIAG(         key, index)   EndGameKeyDefinition::decodeKKSym8(         key, index)

#define KK_ONDIAG_2MEN(        key)         (EndGameKeyDefinition::encodeKKSym8(         key)         - START_RANGE_KK_ONDIAG)
#define SETKK_ONDIAG(          key, index)   EndGameKeyDefinition::decodeKKSym8(         key, (index) + START_RANGE_KK_ONDIAG)

#define KK_WITH_PAWN_2MEN(     key)          EndGameKeyDefinition::encodeKKWithPawn(     key)
#define SETKK_WITH_PAWN(       key, index)   EndGameKeyDefinition::decodeKKWithPawn(     key, index)

#define LEFTWKK_WITH_PAWN_2MEN(key)          EndGameKeyDefinition::encodeLeftWKKWithPawn(key)
#define SETLEFTWKK_WITH_PAWN(  key, index)   EndGameKeyDefinition::decodeLeftWKKWithPawn(key, index)

#define ADD4BIT(    addr  , index)          (((addr) << 4) | (index))
#define ADD5BIT(    addr  , index)          (((addr) << 5) | (index))

#define SUM1TO(n)    ((n)*((n)+1)/2)             /* sum(1, 2, 3, .. , n)                                  */
#define SUMSUM1TO(n) ((((((n)+3)*(n))+2)*(n))/6) /* sum(SUM1TO(1), SUM1TO(2), SUM1TO(3), ... , SUM1TO(n)) */

#define GET_RANGESTART2EQUAL(f, index) ((f) * SUM1TO(index))
#define GET_RANGESTART3EQUAL(f, index) ((f) * SUMSUM1TO(index))

#define SORT2(i,j)                  \
{ if(i > j) {                       \
    swap(i, j);                     \
  }                                 \
}

#define SORT3(i,j,k)                \
{ SORT2(i, j);                      \
  if(j > k) {                       \
    swap(j, k);                     \
    SORT2(i, j);                    \
  }                                 \
}

#define SORT2_UPDATECHANGED(i,j)    \
{ if(i > j) {                       \
    orderChanged = true;            \
    swap(i, j);                     \
  } else {                          \
    orderChanged = false;           \
  }                                 \
}

#define SORT3_UPDATECHANGED(i,j,k)  \
{ SORT2_UPDATECHANGED(i, j);        \
  if(j > k) {                       \
    swap(j, k);                     \
    orderChanged = true;            \
    SORT2(i, j);                    \
  }                                 \
}

#ifdef _DEBUG

UINT add2Equal(             UINT addr, UINT maxAddr, UINT lp, UINT hp);
UINT add2EqualAllowEqualLH( UINT addr, UINT maxAddr, UINT lp, UINT hp);

UINT add3Equal(             UINT addr, UINT maxAddr, UINT lp, UINT mp, UINT hp);
UINT add3EqualAllowEqualLM( UINT addr, UINT maxAddr, UINT lp, UINT mp, UINT hp);
UINT add3EqualAllowEqualHM( UINT addr, UINT maxAddr, UINT lp, UINT mp, UINT hp);

void set2Pos2Equal(       EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2OffDiagPosNoFlip(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2OffDiagPosFlipi( EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2OffDiagPosFlipj( EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2OffDiagPosFlipij(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);

void set3Pos3Equal(       EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3OffDiagPosNoFlip(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3OffDiagPosFlipi( EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3OffDiagPosFlipj( EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3OffDiagPosFlipij(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);

#define ADD2EQUAL(             addr, maxAddr, lp,     hp) add2Equal(             addr, maxAddr, lp,     hp)
#define ADD2EQUALALLOWEQUALLH( addr, maxAddr, lp,     hp) add2EqualAllowEqualLH( addr, maxAddr, lp,     hp)

#define ADD3EQUAL(             addr, maxAddr, lp, mp, hp) add3Equal(             addr, maxAddr, lp, mp, hp)
#define ADD3EQUALALLOWEQUALLM( addr, maxAddr, lp, mp, hp) add3EqualAllowEqualLM( addr, maxAddr, lp, mp, hp)
#define ADD3EQUALALLOWEQUALHM( addr, maxAddr, lp, mp, hp) add3EqualAllowEqualHM( addr, maxAddr, lp, mp, hp)


#define SET2POS2EQUAL(       key, addr, maxAddr, lpIndex, hpIndex) set2Pos2Equal(       key, addr, maxAddr, lpIndex, hpIndex)
#define SET2OFFDIAGPOSNOFLIP(key, addr, maxAddr, lpIndex, hpIndex) set2OffDiagPosNoFlip(key, addr, maxAddr, lpIndex, hpIndex)
#define SET2OFFDIAGPOSFLIPi( key, addr, maxAddr, lpIndex, hpIndex) set2OffDiagPosFlipi( key, addr, maxAddr, lpIndex, hpIndex)
#define SET2OFFDIAGPOSFLIPj( key, addr, maxAddr, lpIndex, hpIndex) set2OffDiagPosFlipj( key, addr, maxAddr, lpIndex, hpIndex)
#define SET2OFFDIAGPOSFLIPij(key, addr, maxAddr, lpIndex, hpIndex) set2OffDiagPosFlipij(key, addr, maxAddr, lpIndex, hpIndex)

#define SET3POS3EQUALa(      key, addr, table, size,             maxAddr, lpIndex, mpIndex, hpIndex) \
  set3Pos3Equal(             key, addr, table, size,             maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3POS3EQUAL(       key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  SET3POS3EQUALa(            key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3OFFDIAGPOSNOFLIP(key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3OffDiagPosNoFlip(      key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3OFFDIAGPOSFLIPi( key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3OffDiagPosFlipi(       key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3OFFDIAGPOSFLIPj( key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3OffDiagPosFlipj(       key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3OFFDIAGPOSFLIPij(key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3OffDiagPosFlipij(      key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#else

#define ADD2EQUAL(            addr, maxAddr, lp,     hp) ((addr) * (hp)    + (lp)                      + GET_RANGESTART2EQUAL((maxAddr)/2,(hp)-1))
#define ADD2EQUALALLOWEQUALLH(addr, maxAddr, lp,     hp) ((addr) *((hp)+1) + (lp)                      + GET_RANGESTART2EQUAL((maxAddr)/2, hp   ))
#define ADD3EQUAL(            addr, maxAddr, lp, mp, hp) (ADD2EQUAL(            addr, maxAddr, lp, mp) + GET_RANGESTART3EQUAL((maxAddr)/2,(hp)-2))
#define ADD3EQUALALLOWEQUALLM(addr, maxAddr, lp, mp, hp) (ADD2EQUALALLOWEQUALLH(addr, maxAddr, lp, mp) + GET_RANGESTART3EQUAL((maxAddr)/2,(hp)-1))
#define ADD3EQUALALLOWEQUALHM(addr, maxAddr, lp, mp, hp) (ADD2EQUAL(            addr, maxAddr, lp, mp) + GET_RANGESTART3EQUAL((maxAddr)/2,(hp)-1))

#define SET2POS2EQUAL(key, addr, maxAddr, lpIndex, hpIndex)                           \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                   \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r); r++;                                  \
  key.setPosition##hpIndex(r);                                                        \
  key.setPosition##lpIndex((addr) % r);                                               \
  addr /= r;                                                                          \
}

#define SET2OFFDIAGPOSNOFLIP(key, addr, maxAddr, lpIndex, hpIndex)                    \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                   \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r); r++;                                  \
  key.setPosition##hpIndex(offDiagIndexToPos[r]);                                     \
  key.setPosition##lpIndex(offDiagIndexToPos[(addr) % r]);                            \
  addr /= r;                                                                          \
}

#define SET2OFFDIAGPOSFLIPi(key, addr, maxAddr, lpIndex, hpIndex)                     \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                   \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                       \
  key.setPosition##hpIndex(offDiagIndexToPos[r]); r++;                                \
  key.setPosition##lpIndex(offDiagIndexToPos[(addr) % r + 28]);                       \
  addr /= r;                                                                          \
}

#define SET2OFFDIAGPOSFLIPj(key, addr, maxAddr, lpIndex, hpIndex)                     \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                   \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                       \
  key.setPosition##hpIndex(offDiagIndexToPos[r+28]); r++;                             \
  key.setPosition##lpIndex(offDiagIndexToPos[(addr) % r]);                            \
  addr /= r;                                                                          \
}

#define SET2OFFDIAGPOSFLIPij(key, addr, maxAddr, lpIndex, hpIndex)                    \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                   \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r); r++;                                  \
  key.setPosition##hpIndex(offDiagIndexToPos[r+28]);                                  \
  key.setPosition##lpIndex(offDiagIndexToPos[(addr) % r + 28]);                       \
  addr /= r;                                                                          \
}

// Can specify tablesize if only part of the rangetable should be searched. Use SET3POS3EQUAL, which use ARRAYSIZE(table) for paramete size!
#define SET3POS3EQUALa(key, addr, table, size, maxAddr, lpIndex, mpIndex, hpIndex)    \
{ int r = EndGameKeyDefinition::findRange(table, size, addr);                         \
  key.setPosition##hpIndex(r+2);                                                      \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                       \
  SET2POS2EQUAL(key, addr, maxAddr, lpIndex, mpIndex);                                \
}

#define SET3POS3EQUAL(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)           \
  SET3POS3EQUALa(key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3OFFDIAGPOSNOFLIP(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)    \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);             \
  key.setPosition##hpIndex(offDiagIndexToPos[r+2]);                                   \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                       \
  SET2OFFDIAGPOSNOFLIP(key, addr, maxAddr, lpIndex, mpIndex);                         \
}

#define SET3OFFDIAGPOSFLIPi(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)     \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);             \
  key.setPosition##hpIndex(offDiagIndexToPos[r+1]);                                   \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                       \
  SET2OFFDIAGPOSFLIPi(key, addr, maxAddr, lpIndex, mpIndex);                          \
}

#define SET3OFFDIAGPOSFLIPj(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)     \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);             \
  key.setPosition##hpIndex(offDiagIndexToPos[r+1]);                                   \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                       \
  r = findRange2Equal((maxAddr)/2, addr);                                             \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                       \
  r++;                                                                                \
  key.setPosition##mpIndex(offDiagIndexToPos[r+28]);                                  \
  key.setPosition##lpIndex(offDiagIndexToPos[addr % r]);                              \
  addr /= r;                                                                          \
}

#define SET3OFFDIAGPOSFLIPij(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)    \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);             \
  key.setPosition##hpIndex(offDiagIndexToPos[r+2]);                                   \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                       \
  SET2OFFDIAGPOSFLIPij(key, addr, maxAddr, lpIndex, mpIndex);                         \
}

#endif

#define INIT_RANGETABLE3EQUAL(table, maxAddr)                                         \
{ table[0] = 0;                                                                       \
  for(int i = 1; i < ARRAYSIZE(table); i++) {                                         \
    table[i] = ((maxAddr)/2) * SUM1TO(i) + table[i-1];                                \
  }                                                                                   \
}

#define LENGTH_RANGETABLE3EQUAL(table) SUMSUM1TO(ARRAYSIZE(table))

#define BOOL2MASK(f, x, y   ) ((f(x)?1:0) | (f(y)?2:0))
#define BOOL3MASK(f, x, y, z) (BOOL2MASK(f, x, y) | (f(z)?4:0))

#define KEYBOOL2MASK(key, f, i, j)    BOOL2MASK(f, key.getPosition##i(), key.getPosition##j())
#define KEYBOOL3MASK(key, f, i, j, k) BOOL3MASK(f, key.getPosition##i(), key.getPosition##j(), key.getPosition##k())


// ---------------------------------------- Without pawns or bishop pair ----------------------------------------

#define ADDPLAYER(   player, addr)             (((addr) << 1) | (player))
#define ADDPIT(      key   , addr)             ADDPLAYER(key.getPlayerInTurn(), addr)
#define SETPIT(      key   , addr)             { key.setPlayerInTurn((Player)((addr) & 1)); addr >>= 1; }

#define ADDPOS_INDEX(addr, count, index)       ((addr) * (count) + (index))
#define SETPOS_INDEX(key, addr, count, pIndex) { key.setPosition##pIndex((addr) % (count)); addr /= (count); }

#define P2POSCOUNT                             (64-2)
#define P3POSCOUNT                             (P2POSCOUNT-1)
#define P4POSCOUNT                             (P3POSCOUNT-1)

#define ADDP2_INDEX(      addr, p2Index)       ADDPOS_INDEX(addr, P2POSCOUNT, p2Index)
#define ADDP3_INDEX(      addr, p3Index)       ADDPOS_INDEX(addr, P3POSCOUNT, p3Index)
#define ADDP4_INDEX(      addr, p4Index)       ADDPOS_INDEX(addr, P4POSCOUNT, p4Index)

#define SETP2_INDEX(      key,  addr   )       SETPOS_INDEX(key, addr, P2POSCOUNT, 2)
#define SETP3_INDEX(      key,  addr   )       SETPOS_INDEX(key, addr, P3POSCOUNT, 3)
#define SETP4_INDEX(      key,  addr   )       SETPOS_INDEX(key, addr, P4POSCOUNT, 4)

#define ADDPOS_BELOWDIAG( addr, subDiagIndex)  ADDPOS_INDEX(addr, 28, subDiagIndex)
#define SETPOS_BELOWDIAG( key , addr, pIndex)  { key.setPosition##pIndex(EndGameKeyDefinition::subDiagIndexToPos[(addr) % 28]); addr /= 28; }

#define ADDP2_ONDIAG(     addr, p2DiagIndex)   ADDPOS_INDEX(addr, 6, p2DiagIndex)
#define ADDP3_ONDIAG(     addr, p3DiagIndex)   ADDPOS_INDEX(addr, 5, p3DiagIndex)
#define ADDP4_ONDIAG(     addr, p4DiagIndex)   ADDPOS_INDEX(addr, 4, p4DiagIndex)

#define SETP2_ONDIAG(     key , addr)          SETPOS_INDEX(key, addr,  6, 2)
#define SETP3_ONDIAG(     key , addr)          SETPOS_INDEX(key, addr,  5, 3)
#define SETP4_ONDIAG(     key , addr)          SETPOS_INDEX(key, addr,  4, 4)

#define KK_OFFDIAG_3MEN(         key) ADDP2_INDEX(     KK_OFFDIAG_2MEN(  key), key.getP2OffDiagIndex())
#define KK_ONDIAG_3MEN(          key) ADDPOS_BELOWDIAG(KK_ONDIAG_2MEN(   key), EndGameKeyDefinition::subDiagPosToIndex[key.getPosition2()])
#define KKP2_ONDIAG_3MEN(        key) ADDP2_ONDIAG(    KK_ONDIAG_2MEN(   key), key.getP2DiagIndex())

#define KK_OFFDIAG_4MEN(         key) ADDP3_INDEX(     KK_OFFDIAG_3MEN(  key), key.getP3OffDiagIndex())
#define KK_ONDIAG_4MEN(          key) ADDP3_INDEX(     KK_ONDIAG_3MEN(   key), key.getP3OffDiagIndex())
#define KKP2_ONDIAG_4MEN(        key) ADDPOS_BELOWDIAG(KKP2_ONDIAG_3MEN( key), EndGameKeyDefinition::subDiagPosToIndex[key.getPosition3()])
#define KKP23_ONDIAG_4MEN(       key) ADDP3_ONDIAG(    KKP2_ONDIAG_3MEN( key), key.getP3DiagIndex())

#define KK_OFFDIAG_5MEN(         key) ADDP4_INDEX(     KK_OFFDIAG_4MEN(  key), key.getP4OffDiagIndex())
#define KK_ONDIAG_5MEN(          key) ADDP4_INDEX(     KK_ONDIAG_4MEN(   key), key.getP4OffDiagIndex())
#define KKP2_ONDIAG_5MEN(        key) ADDP4_INDEX(     KKP2_ONDIAG_4MEN( key), key.getP4OffDiagIndex())
#define KKP23_ONDIAG_5MEN(       key) ADDPOS_BELOWDIAG(KKP23_ONDIAG_4MEN(key), EndGameKeyDefinition::subDiagPosToIndex[key.getPosition4()])
#define KKP234_ONDIAG_5MEN(      key) ADDP4_ONDIAG(    KKP23_ONDIAG_4MEN(key), key.getP4DiagIndex())

#define KK_OFFDIAG_2MEN_INDEX(   key) ADDPIT(key, KK_OFFDIAG_2MEN(   key))
#define KK_ONDIAG_2MEN_INDEX(    key) ADDPIT(key, KK_ONDIAG_2MEN(    key))

#define KK_OFFDIAG_3MEN_INDEX(   key) ADDPIT(key, KK_OFFDIAG_3MEN(   key))
#define KK_ONDIAG_3MEN_INDEX(    key) ADDPIT(key, KK_ONDIAG_3MEN(    key))
#define KKP2_ONDIAG_3MEN_INDEX(  key) ADDPIT(key, KKP2_ONDIAG_3MEN(  key))

#define KK_OFFDIAG_4MEN_INDEX(   key) ADDPIT(key, KK_OFFDIAG_4MEN(   key))
#define KK_ONDIAG_4MEN_INDEX(    key) ADDPIT(key, KK_ONDIAG_4MEN(    key))
#define KKP2_ONDIAG_4MEN_INDEX(  key) ADDPIT(key, KKP2_ONDIAG_4MEN(  key))
#define KKP23_ONDIAG_4MEN_INDEX( key) ADDPIT(key, KKP23_ONDIAG_4MEN( key))

#define KK_OFFDIAG_5MEN_INDEX(   key) ADDPIT(key, KK_OFFDIAG_5MEN(   key))
#define KK_ONDIAG_5MEN_INDEX(    key) ADDPIT(key, KK_ONDIAG_5MEN(    key))
#define KKP2_ONDIAG_5MEN_INDEX(  key) ADDPIT(key, KKP2_ONDIAG_5MEN(  key))
#define KKP23_ONDIAG_5MEN_INDEX( key) ADDPIT(key, KKP23_ONDIAG_5MEN( key))
#define KKP234_ONDIAG_5MEN_INDEX(key) ADDPIT(key, KKP234_ONDIAG_5MEN(key))

#define MIN_KK_OFFDIAG_2MEN            0                                           /* Kings:B1, D1 */
#define MAX_KK_OFFDIAG_2MEN            KK_OFFDIAG_COUNT                            /* Kings:D3, H8  == 441 */
#define MIN_KK_ONDIAG_2MEN             0                                           /* Kings:B2, D4 */
#define MAX_KK_ONDIAG_2MEN             KK_ONDIAG_COUNT                             /* Kings:A1, H8 */

#define MIN_KK_OFFDIAG_3MEN            ADDP2_INDEX(     MIN_KK_OFFDIAG_2MEN  ,  0) /* Kings:B1, D1, p2:A1 */
#define MAX_KK_OFFDIAG_3MEN            ADDP2_INDEX(     MAX_KK_OFFDIAG_2MEN  ,  0) /* Kings:D3, H8, p2:G8 */
#define MIN_KK_ONDIAG_3MEN             ADDPOS_BELOWDIAG(MIN_KK_ONDIAG_2MEN   ,  0) /* Kings:B2, D4, p2:B1 */
#define MAX_KK_ONDIAG_3MEN             ADDPOS_BELOWDIAG(MAX_KK_ONDIAG_2MEN   ,  0) /* Kings:A1, H8, p2:H7 */
#define MIN_KKP2_ONDIAG_3MEN           ADDP2_ONDIAG(    MIN_KK_ONDIAG_2MEN   ,  0) /* Kings:B2, D4, p2:A1 */
#define MAX_KKP2_ONDIAG_3MEN           ADDP2_ONDIAG(    MAX_KK_ONDIAG_2MEN   ,  0) /* Kings:A1, H8, p2:G7 */

#define MIN_KK_OFFDIAG_4MEN            ADDP3_INDEX(     MIN_KK_OFFDIAG_3MEN  ,  0) /* Kings:B1, D1, p2:A1, p3:C1 */
#define MAX_KK_OFFDIAG_4MEN            ADDP3_INDEX(     MAX_KK_OFFDIAG_3MEN  ,  0) /* Kings:D3, H8, p2:G8, p3:F8 */
#define MIN_KK_ONDIAG_4MEN             ADDP3_INDEX(     MIN_KK_ONDIAG_3MEN   ,  0) /* Kings:B2, D4, p2:B1, p3:A1 */
#define MAX_KK_ONDIAG_4MEN             ADDP3_INDEX(     MAX_KK_ONDIAG_3MEN   ,  0) /* Kings:A1, H8, p2:H7, p3:G8 */
#define MIN_KKP2_ONDIAG_4MEN           ADDPOS_BELOWDIAG(MIN_KKP2_ONDIAG_3MEN ,  0) /* Kings:B2, D4, p2:A1, p3:B1 */
#define MAX_KKP2_ONDIAG_4MEN           ADDPOS_BELOWDIAG(MAX_KKP2_ONDIAG_3MEN ,  0) /* Kings:A1, H8, p2:G7, p3:H7 */
#define MIN_KKP23_ONDIAG_4MEN          ADDP3_ONDIAG(    MIN_KKP2_ONDIAG_3MEN ,  0) /* Kings:B2, D4, p2:A1, p3:C3 */
#define MAX_KKP23_ONDIAG_4MEN          ADDP3_ONDIAG(    MAX_KKP2_ONDIAG_3MEN ,  0) /* Kings:A1, H8, p2:G7, p3:F6 */

#define MIN_KK_OFFDIAG_5MEN            ADDP4_INDEX(     MIN_KK_OFFDIAG_4MEN  ,  0) /* Kings:B1, D1, p2:A1, p3:C1, p4:E1 */
#define MAX_KK_OFFDIAG_5MEN            ADDP4_INDEX(     MAX_KK_OFFDIAG_4MEN  ,  0) /* Kings:D3, H8, p2:G8, p3:F8, p4:E8 */
#define MIN_KK_ONDIAG_5MEN             ADDP4_INDEX(     MIN_KK_ONDIAG_4MEN   ,  0) /* Kings:B2, D4, p2:B1, p3:A1, p4:C1 */
#define MAX_KK_ONDIAG_5MEN             ADDP4_INDEX(     MAX_KK_ONDIAG_4MEN   ,  0) /* Kings:A1, H8, p2:H7, p3:G8, p4:F8 */
#define MIN_KKP2_ONDIAG_5MEN           ADDP4_INDEX(     MIN_KKP2_ONDIAG_4MEN ,  0) /* Kings:B2, D4, p2:A1, p3:B1, p4:C1 */
#define MAX_KKP2_ONDIAG_5MEN           ADDP4_INDEX(     MAX_KKP2_ONDIAG_4MEN ,  0) /* Kings:A1, H8, p2:G7, p3:H7, p4:G8 */
#define MIN_KKP23_ONDIAG_5MEN          ADDPOS_BELOWDIAG(MIN_KKP23_ONDIAG_4MEN,  0) /* Kings:B2, D4, p2:A1, p3:C3, p4:B1 */
#define MAX_KKP23_ONDIAG_5MEN          ADDPOS_BELOWDIAG(MAX_KKP23_ONDIAG_4MEN,  0) /* Kings:A1, H8, p2:G7, p3:F6, p4:H7 */
#define MIN_KKP234_ONDIAG_5MEN         ADDP4_ONDIAG(    MIN_KKP23_ONDIAG_4MEN,  0) /* Kings:B2, D4, p2:A1, p3:C3, p4:E5 */
#define MAX_KKP234_ONDIAG_5MEN         ADDP4_ONDIAG(    MAX_KKP23_ONDIAG_4MEN,  0) /* Kings:A1, H8, p2:G7, p3:F6, p4:E5 */

#define FIRSTINDEX(name)               ADDPLAYER(WHITEPLAYER, MIN_##name)
#define LASTINDEX(name)                ADDPLAYER(WHITEPLAYER, MAX_##name)

#define MININDEX_KK_OFFDIAG_2MEN       FIRSTINDEX(KK_OFFDIAG_2MEN   )
#define MAXINDEX_KK_OFFDIAG_2MEN       LASTINDEX( KK_OFFDIAG_2MEN   )
#define MININDEX_KK_ONDIAG_2MEN        FIRSTINDEX(KK_ONDIAG_2MEN    )
#define MAXINDEX_KK_ONDIAG_2MEN        LASTINDEX( KK_ONDIAG_2MEN    )

#define MININDEX_KK_OFFDIAG_3MEN       FIRSTINDEX(KK_OFFDIAG_3MEN   )
#define MAXINDEX_KK_OFFDIAG_3MEN       LASTINDEX( KK_OFFDIAG_3MEN   )
#define MININDEX_KK_ONDIAG_3MEN        FIRSTINDEX(KK_ONDIAG_3MEN    )
#define MAXINDEX_KK_ONDIAG_3MEN        LASTINDEX( KK_ONDIAG_3MEN    )
#define MININDEX_KKP2_ONDIAG_3MEN      FIRSTINDEX(KKP2_ONDIAG_3MEN  )
#define MAXINDEX_KKP2_ONDIAG_3MEN      LASTINDEX( KKP2_ONDIAG_3MEN  )

#define MININDEX_KK_OFFDIAG_4MEN       FIRSTINDEX(KK_OFFDIAG_4MEN   )
#define MAXINDEX_KK_OFFDIAG_4MEN       LASTINDEX( KK_OFFDIAG_4MEN   )
#define MININDEX_KK_ONDIAG_4MEN        FIRSTINDEX(KK_ONDIAG_4MEN    )
#define MAXINDEX_KK_ONDIAG_4MEN        LASTINDEX( KK_ONDIAG_4MEN    )
#define MININDEX_KKP2_ONDIAG_4MEN      FIRSTINDEX(KKP2_ONDIAG_4MEN  )
#define MAXINDEX_KKP2_ONDIAG_4MEN      LASTINDEX( KKP2_ONDIAG_4MEN  )
#define MININDEX_KKP23_ONDIAG_4MEN     FIRSTINDEX(KKP23_ONDIAG_4MEN )
#define MAXINDEX_KKP23_ONDIAG_4MEN     LASTINDEX( KKP23_ONDIAG_4MEN )

#define MININDEX_KK_OFFDIAG_5MEN       FIRSTINDEX(KK_OFFDIAG_5MEN   )
#define MAXINDEX_KK_OFFDIAG_5MEN       LASTINDEX( KK_OFFDIAG_5MEN   )
#define MININDEX_KK_ONDIAG_5MEN        FIRSTINDEX(KK_ONDIAG_5MEN    )
#define MAXINDEX_KK_ONDIAG_5MEN        LASTINDEX( KK_ONDIAG_5MEN    )
#define MININDEX_KKP2_ONDIAG_5MEN      FIRSTINDEX(KKP2_ONDIAG_5MEN  )
#define MAXINDEX_KKP2_ONDIAG_5MEN      LASTINDEX( KKP2_ONDIAG_5MEN  )
#define MININDEX_KKP23_ONDIAG_5MEN     FIRSTINDEX(KKP23_ONDIAG_5MEN )
#define MAXINDEX_KKP23_ONDIAG_5MEN     LASTINDEX( KKP23_ONDIAG_5MEN )
#define MININDEX_KKP234_ONDIAG_5MEN    FIRSTINDEX(KKP234_ONDIAG_5MEN)
#define MAXINDEX_KKP234_ONDIAG_5MEN    LASTINDEX( KKP234_ONDIAG_5MEN)

#define START_RANGE_KK_OFFDIAG_2MEN    0
#define OFFSET_KK_OFFDIAG_2MEN         (START_RANGE_KK_OFFDIAG_2MEN    - MININDEX_KK_OFFDIAG_2MEN    )
#define START_RANGE_KK_ONDIAG_2MEN     (OFFSET_KK_OFFDIAG_2MEN         + MAXINDEX_KK_OFFDIAG_2MEN    )
#define OFFSET_KK_ONDIAG_2MEN          (START_RANGE_KK_ONDIAG_2MEN     - MININDEX_KK_ONDIAG_2MEN     )
#define END_RANGE_ONDIAG_2MEN          (START_RANGE_KK_ONDIAG_2MEN     + MAXINDEX_KK_ONDIAG_2MEN     )

#define START_RANGE_KK_OFFDIAG_3MEN    0
#define OFFSET_KK_OFFDIAG_3MEN         (START_RANGE_KK_OFFDIAG_3MEN    - MININDEX_KK_OFFDIAG_3MEN    )
#define START_RANGE_KK_ONDIAG_3MEN     (OFFSET_KK_OFFDIAG_3MEN         + MAXINDEX_KK_OFFDIAG_3MEN    )
#define OFFSET_KK_ONDIAG_3MEN          (START_RANGE_KK_ONDIAG_3MEN     - MININDEX_KK_ONDIAG_3MEN     )
#define START_RANGE_KKP2_ONDIAG_3MEN   (OFFSET_KK_ONDIAG_3MEN          + MAXINDEX_KK_ONDIAG_3MEN     )
#define OFFSET_KKP2_ONDIAG_3MEN        (START_RANGE_KKP2_ONDIAG_3MEN   - MININDEX_KKP2_ONDIAG_3MEN   )
#define END_RANGE_ONDIAG_3MEN          (START_RANGE_KKP2_ONDIAG_3MEN   + MAXINDEX_KKP2_ONDIAG_3MEN   )

#define START_RANGE_KK_OFFDIAG_4MEN    0
#define OFFSET_KK_OFFDIAG_4MEN         (START_RANGE_KK_OFFDIAG_4MEN    - MININDEX_KK_OFFDIAG_4MEN    )
#define START_RANGE_KK_ONDIAG_4MEN     (OFFSET_KK_OFFDIAG_4MEN         + MAXINDEX_KK_OFFDIAG_4MEN    )
#define OFFSET_KK_ONDIAG_4MEN          (START_RANGE_KK_ONDIAG_4MEN     - MININDEX_KK_ONDIAG_4MEN     )
#define START_RANGE_KKP2_ONDIAG_4MEN   (OFFSET_KK_ONDIAG_4MEN          + MAXINDEX_KK_ONDIAG_4MEN     )
#define OFFSET_KKP2_ONDIAG_4MEN        (START_RANGE_KKP2_ONDIAG_4MEN   - MININDEX_KKP2_ONDIAG_4MEN   )
#define START_RANGE_KKP23_ONDIAG_4MEN  (OFFSET_KKP2_ONDIAG_4MEN        + MAXINDEX_KKP2_ONDIAG_4MEN   )
#define OFFSET_KKP23_ONDIAG_4MEN       (START_RANGE_KKP23_ONDIAG_4MEN  - MININDEX_KKP23_ONDIAG_4MEN  )
#define END_RANGE_ONDIAG_4MEN          (START_RANGE_KKP23_ONDIAG_4MEN  + MAXINDEX_KKP23_ONDIAG_4MEN  )

#define START_RANGE_KK_OFFDIAG_5MEN    0
#define OFFSET_KK_OFFDIAG_5MEN         (START_RANGE_KK_OFFDIAG_5MEN    - MININDEX_KK_OFFDIAG_5MEN    )
#define START_RANGE_KK_ONDIAG_5MEN     (OFFSET_KK_OFFDIAG_5MEN         + MAXINDEX_KK_OFFDIAG_5MEN    )
#define OFFSET_KK_ONDIAG_5MEN          (START_RANGE_KK_ONDIAG_5MEN     - MININDEX_KK_ONDIAG_5MEN     )
#define START_RANGE_KKP2_ONDIAG_5MEN   (OFFSET_KK_ONDIAG_5MEN          + MAXINDEX_KK_ONDIAG_5MEN     )
#define OFFSET_KKP2_ONDIAG_5MEN        (START_RANGE_KKP2_ONDIAG_5MEN   - MININDEX_KKP2_ONDIAG_5MEN   )
#define START_RANGE_KKP23_ONDIAG_5MEN  (OFFSET_KKP2_ONDIAG_5MEN        + MAXINDEX_KKP2_ONDIAG_5MEN   )
#define OFFSET_KKP23_ONDIAG_5MEN       (START_RANGE_KKP23_ONDIAG_5MEN  - MININDEX_KKP23_ONDIAG_5MEN  )
#define START_RANGE_KKP234_ONDIAG_5MEN (OFFSET_KKP23_ONDIAG_5MEN       + MAXINDEX_KKP23_ONDIAG_5MEN  )
#define OFFSET_KKP234_ONDIAG_5MEN      (START_RANGE_KKP234_ONDIAG_5MEN - MININDEX_KKP234_ONDIAG_5MEN )
#define END_RANGE_ONDIAG_5MEN          (START_RANGE_KKP234_ONDIAG_5MEN + MAXINDEX_KKP234_ONDIAG_5MEN )

// ---------------------------------------- Pawns ----------------------------------------

#define ONE_PAWN_3MEN(key, pawn1Index)         ADDPOS_INDEX(KK_WITH_PAWN_2MEN(key), PAWN1_POSCOUNT, EndGameKeyDefinition::pawnPosToIndex[key.getPosition##pawn1Index()])
#define ONE_PAWN_4MEN(key)                     ADDP3_INDEX(ONE_PAWN_3MEN(key, 2), key.getP3OffDiagIndex())
#define ONE_PAWN_5MEN(key)                     ADDP4_INDEX(ONE_PAWN_4MEN(key   ), key.getP4OffDiagIndex())

#ifdef _DEBUG
#define LEFTWKK_ONE_PAWN_3MEN(key, pawnIndex)  ADDPOS_INDEX(LEFTWKK_WITH_PAWN_2MEN(key), PAWN1_POSCOUNT, EndGameKeyDefinition::pawnPosToIndex[key.getPosition(pawnIndex)])
#else
#define LEFTWKK_ONE_PAWN_3MEN(key, pawnIndex)  ADDPOS_INDEX(LEFTWKK_WITH_PAWN_2MEN(key), PAWN1_POSCOUNT, EndGameKeyDefinition::pawnPosToIndex[key.getPosition##pawnIndex()])
#endif

#define KK_WITH_PAWN_2MEN_INDEX(     key)      ADDPIT(key, KK_WITH_PAWN_2MEN(key))

#define ONE_PAWN_3MEN_INDEX(         key)      ADDPIT(key, ONE_PAWN_3MEN(key, 2))
#define ONE_PAWN_4MEN_INDEX(         key)      ADDPIT(key, ONE_PAWN_4MEN(key   ))
#define ONE_PAWN_5MEN_INDEX(         key)      ADDPIT(key, ONE_PAWN_5MEN(key   ))

#define LEFTWKK_ONE_PAWN_3MEN_INDEX(key, pawnIndex)  ADDPIT(key, LEFTWKK_ONE_PAWN_3MEN(key, pawnIndex))

#define MIN_KK_WITH_PAWN_2MEN          0
#define MAX_KK_WITH_PAWN_2MEN          KK_WITH_PAWN_COUNT

#define MIN_KK_WITH_PAWN_3MEN          0
#define MAX_KK_WITH_PAWN_3MEN          (KK_WITH_PAWN_COUNT * PAWN1_POSCOUNT)

#define MININDEX_KK_WITH_PAWN_2MEN     FIRSTINDEX(KK_WITH_PAWN_2MEN)
#define MAXINDEX_KK_WITH_PAWN_2MEN     LASTINDEX( KK_WITH_PAWN_2MEN)

#define MININDEX_KK_WITH_PAWN_3MEN     FIRSTINDEX(KK_WITH_PAWN_3MEN)
#define MAXINDEX_KK_WITH_PAWN_3MEN     LASTINDEX( KK_WITH_PAWN_3MEN)

#define MININDEX_KK_WITH_PAWN_4MEN     FIRSTINDEX(KK_WITH_PAWN_4MEN)
#define MAXINDEX_KK_WITH_PAWN_4MEN     LASTINDEX( KK_WITH_PAWN_4MEN)

#define MININDEX_KK_WITH_PAWN_5MEN     FIRSTINDEX(KK_WITH_PAWN_5MEN)
#define MAXINDEX_KK_WITH_PAWN_5MEN     LASTINDEX( KK_WITH_PAWN_5MEN)

#define KK_WITH_PAWN_POSCOUNT_2MEN    (MAXINDEX_KK_WITH_PAWN_2MEN - MININDEX_KK_WITH_PAWN_2MEN)
#define KK_WITH_PAWN_POSCOUNT_3MEN    (MAXINDEX_KK_WITH_PAWN_3MEN - MININDEX_KK_WITH_PAWN_3MEN)

#define TWO_PAWNS_4MEN(key)            ADDPOS_INDEX(ONE_PAWN_3MEN(key, 2), PAWN2_POSCOUNT, EndGameKeyDefinition::pawnPosToIndex[key.getP3Pawn2Index()])
#define TWO_PAWNS_5MEN(key)            ADDP4_INDEX(TWO_PAWNS_4MEN(key)   , key.getP4OffDiagIndex())

#define TWO_PAWNS_4MEN_INDEX(key)      ADDPIT(key, TWO_PAWNS_4MEN(key))
#define TWO_PAWNS_5MEN_INDEX(key)      ADDPIT(key, TWO_PAWNS_5MEN(key))


#define SETPAWN1POS(key, addr, pIndex)  { key.setPosition##pIndex(EndGameKeyDefinition::pawnIndexToPos[(addr) % PAWN1_POSCOUNT]);   \
                                          (addr) /= PAWN1_POSCOUNT;                                                                 \
                                        }

#define SETPAWN2POS(key, addr, pIndex)  { key.setPosition##pIndex(EndGameKeyDefinition::pawnIndexToPos[(addr) % PAWN2_POSCOUNT]);   \
                                          (addr) /= PAWN2_POSCOUNT;                                                                 \
                                        }

#define SETPAWN3POS(key, addr, pIndex)  { key.setPosition##pIndex(EndGameKeyDefinition::pawnIndexToPos[(addr) % PAWN3_POSCOUNT]);   \
                                          (addr) /= PAWN3_POSCOUNT;                                                                 \
                                        }

#define SET2SYMMETRICPAWNS(key, addr, maxAddr, p1Index, p2Index)                                                                    \
                          { key.setPosition##p1Index(EndGameKeyDefinition::pawnIndexToPos[(addr) % ((maxAddr) * PAWN1_POSCOUNT)]);  \
                            key.setPosition##p2Index(MIRRORCOLUMN(key.getPosition##p1Index()));                                     \
                            (addr) /= ((maxAddr)*PAWN1_POSCOUNT);                                                                   \
                          }
#ifdef _DEBUG

void set2EqualPawnsNoFlip(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2EqualPawnsFlipi( EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2EqualPawnsFlipj( EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);
void set2EqualPawnsFlipij(EndGameKey &key, unsigned long &addr, UINT maxAddr, int lpIndex, int hpIndex);

void set3EqualPawnsNoFlip(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3EqualPawnsFlipi( EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3EqualPawnsFlipj( EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);
void set3EqualPawnsFlipij(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex);

#define SET2EQUALPAWNSNOFLIP(key, addr, maxAddr, lpIndex, hpIndex) set2EqualPawnsNoFlip(key, addr, maxAddr, lpIndex, hpIndex)
#define SET2EQUALPAWNSFLIPi( key, addr, maxAddr, lpIndex, hpIndex) set2EqualPawnsFlipi( key, addr, maxAddr, lpIndex, hpIndex)
#define SET2EQUALPAWNSFLIPj( key, addr, maxAddr, lpIndex, hpIndex) set2EqualPawnsFlipj( key, addr, maxAddr, lpIndex, hpIndex)
#define SET2EQUALPAWNSFLIPij(key, addr, maxAddr, lpIndex, hpIndex) set2EqualPawnsFlipij(key, addr, maxAddr, lpIndex, hpIndex)

#define SET3EQUALPAWNSNOFLIP(key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3EqualPawnsNoFlip(      key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3EQUALPAWNSFLIPi( key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3EqualPawnsFlipi(       key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3EQUALPAWNSFLIPj( key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3EqualPawnsFlipj(       key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)

#define SET3EQUALPAWNSFLIPij(key, addr, table,                   maxAddr, lpIndex, mpIndex, hpIndex) \
  set3EqualPawnsFlipij(      key, addr, table, ARRAYSIZE(table), maxAddr, lpIndex, mpIndex, hpIndex)


#else

#define SET2EQUALPAWNSNOFLIP(key, addr, maxAddr, lpIndex, hpIndex)                        \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                       \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r); r++;                                      \
  key.setPosition##hpIndex(EndGameKeyDefinition::pawnIndexToPos[r]);                      \
  key.setPosition##lpIndex(EndGameKeyDefinition::pawnIndexToPos[(addr) % r]);             \
  addr /= r;                                                                              \
}

#define SET2EQUALPAWNSFLIPi(key, addr, maxAddr, lpIndex, hpIndex)                         \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                       \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                           \
  key.setPosition##hpIndex(EndGameKeyDefinition::pawnIndexToPos[r]);                      \
  r++;                                                                                    \
  const int lpPos = EndGameKeyDefinition::pawnIndexToPos[(addr) % r];                     \
  key.setPosition##lpIndex(MIRRORCOLUMN(lpPos));                                          \
  addr /= r;                                                                              \
}

#define SET2EQUALPAWNSFLIPj(key, addr, maxAddr, lpIndex, hpIndex)                         \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                       \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                           \
  r++;                                                                                    \
  key.setPosition##hpIndex(MIRRORCOLUMN(EndGameKeyDefinition::pawnIndexToPos[r]));        \
  key.setPosition##lpIndex(EndGameKeyDefinition::pawnIndexToPos[(addr) % r]);             \
  addr /= r;                                                                              \
}

#define SET2EQUALPAWNSFLIPij(key, addr, maxAddr, lpIndex, hpIndex)                        \
{ int r = EndGameKeyDefinition::findRange2Equal((maxAddr)/2, addr);                       \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                           \
  r++;                                                                                    \
  int pos = EndGameKeyDefinition::pawnIndexToPos[r];                                      \
  key.setPosition##hpIndex(MIRRORCOLUMN(pos));                                            \
  pos = EndGameKeyDefinition::pawnIndexToPos[(addr) % r];                                 \
  key.setPosition##lpIndex(MIRRORCOLUMN(pos));                                            \
  addr /= r;                                                                              \
}

#define SET3EQUALPAWNSNOFLIP(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)        \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);                 \
  key.setPosition##hpIndex(pawnIndexToPos[r+2]);                                          \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                           \
  SET2EQUALPAWNSNOFLIP(key, addr, maxAddr, lpIndex, mpIndex);                             \
}

#define SET3EQUALPAWNSFLIPi(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)         \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);                 \
  key.setPosition##hpIndex(pawnIndexToPos[r+1]);                                          \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                           \
  SET2EQUALPAWNSFLIPi(key, addr, maxAddr, lpIndex, mpIndex);                              \
}

#define SET3EQUALPAWNSFLIPj(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)         \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);                 \
  key.setPosition##hpIndex(pawnIndexToPos[r+1]);                                          \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                           \
  r = findRange2Equal((maxAddr)/2, addr);                                                 \
  addr -= GET_RANGESTART2EQUAL((maxAddr)/2, r);                                           \
  r++;                                                                                    \
  int pos = pawnIndexToPos[r];                                                            \
  key.setPosition##mpIndex(MIRRORCOLUMN(pos));                                            \
  key.setPosition##lpIndex(pawnIndexToPos[addr % r]);                                     \
  addr /= r;                                                                              \
}

#define SET3EQUALPAWNSFLIPij(key, addr, table, maxAddr, lpIndex, mpIndex, hpIndex)        \
{ int r = EndGameKeyDefinition::findRange(table, ARRAYSIZE(table), addr);                 \
  key.setPosition##hpIndex(pawnIndexToPos[r+2]);                                          \
  addr -= GET_RANGESTART3EQUAL((maxAddr)/2, r);                                           \
  SET2EQUALPAWNSFLIPij(key, addr, maxAddr, lpIndex, mpIndex);                             \
}

#endif

