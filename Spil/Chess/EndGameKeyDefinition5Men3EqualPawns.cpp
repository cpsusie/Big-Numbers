#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

#define MININDEX 0

static unsigned long rangeStartTable[23];

#define KK_POSCOUNT                    MAXINDEX_KK_WITH_PAWN_2MEN
#define KK_1_QUEENSIDE_POSCOUNT        GET_RANGESTART3EQUAL(KK_POSCOUNT, 22)
#define KK_2_QUEENSIDE_POSCOUNT        GET_RANGESTART3EQUAL(KK_POSCOUNT, 23)
#define KK_3_QUEENSIDE_POSCOUNT        KK_1_QUEENSIDE_POSCOUNT

#define START_RANGE_P4_QUEENSIDE       0 
#define START_RANGE_P24_QUEENSIDE      (START_RANGE_P4_QUEENSIDE       + KK_1_QUEENSIDE_POSCOUNT)
#define START_RANGE_P34_QUEENSIDE      (START_RANGE_P24_QUEENSIDE      + KK_2_QUEENSIDE_POSCOUNT)
#define START_RANGE_P234_QUEENSIDE     (START_RANGE_P34_QUEENSIDE      + KK_2_QUEENSIDE_POSCOUNT)

EndGameKeyDefinition5Men3EqualPawns::EndGameKeyDefinition5Men3EqualPawns(PieceKey pk234)
: EndGameKeyDefinitionDupletsAllowed(pk234, pk234)
{
#ifdef TABLEBASE_BUILDER
  setPawnsOwner();
#endif

  static bool initDone = false;
  if(initDone) return;

  initDone = true;

  INIT_RANGETABLE3EQUAL(rangeStartTable, KK_POSCOUNT);

/*
  DUMP_MACRO( KK_POSCOUNT                     );
  DUMP_MACRO( KK_1_QUEENSIDE_POSCOUNT         );
  DUMP_MACRO( KK_2_QUEENSIDE_POSCOUNT         );
  DUMP_MACRO( KK_3_QUEENSIDE_POSCOUNT         );

  DUMP_MACRO( START_RANGE_P4_QUEENSIDE        );
  DUMP_MACRO( START_RANGE_P24_QUEENSIDE       );
  DUMP_MACRO( START_RANGE_P34_QUEENSIDE       );
  DUMP_MACRO( START_RANGE_P234_QUEENSIDE      );

  DUMP_RANGETABLE(rangeStartTable);
*/
}

#ifdef _DEBUG

static UINT encodeNoFlip(EndGameKey key) {
  int pi2 = EndGameKeyDefinition::pawnPosToIndex[key.getPosition2()];
  int pi3 = EndGameKeyDefinition::pawnPosToIndex[key.getPosition3()];
  int pi4 = EndGameKeyDefinition::pawnPosToIndex[key.getPosition4()];

  SORT3(pi2, pi3, pi4); /* pi2 < pi3 < pi4 */
  return ADDPIT(key, ADD3EQUAL(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pi2, pi3, pi4))            // Use SET3EQUALPAWNSNOFLIP to decode
       + START_RANGE_P234_QUEENSIDE
       - MININDEX;
}

static UINT encodeFlipi(EndGameKey key, UINT i, UINT j, UINT k) {
  int pi = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];
  int pj = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(j)];
  int pk = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(k)];
  SORT2(pj, pk);

  if(pi <= pj) { // pi <= pj < pk
    return ADDPIT(key, ADD3EQUALALLOWEQUALLM(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pi, pj, pk)) // Use SET3EQUALPAWNSFLIPi to decode
         + START_RANGE_P34_QUEENSIDE
         - MININDEX;
  } else {       // pj < pi <= pk
    return ADDPIT(key, ADD3EQUALALLOWEQUALHM(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pj, pi, pk)) // Use SET3EQUALPAWNSFLIPj to decode
         + START_RANGE_P24_QUEENSIDE
         - MININDEX;
  }
}

static UINT encodeFlipij(EndGameKey key, int i, int j, int k) {
  int pi = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition(i))];
  int pj = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition(j))];
  int pk = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(k)];
  SORT2(pi, pj);

  // pi < pj < pk
  return ADDPIT(key, ADD3EQUAL(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pi, pj, pk))               // Use SET3EQUALPAWNSFLIPij to decode
       + START_RANGE_P4_QUEENSIDE
       - MININDEX;
}

#define ENCODE_NOFLIP(key)           return encodeNoFlip(key)
#define ENCODE_FLIPi( key,i,j,k)     return encodeFlipi( key, i, j, k)
#define ENCODE_FLIPij(key,i,j,k)     return encodeFlipij(key, i, j, k)

#else // !_DEBUG

#define ENCODE_NOFLIP(key)                                                                                            \
{ int pi2 = EndGameKeyDefinition::pawnPosToIndex[key.getPosition2()];                                                 \
  int pi3 = EndGameKeyDefinition::pawnPosToIndex[key.getPosition3()];                                                 \
  int pi4 = EndGameKeyDefinition::pawnPosToIndex[key.getPosition4()];                                                 \
  SORT3(pi2, pi3, pi4); /* pi2 < pi3 < pi4 */                                                                         \
  return ADDPIT(key, ADD3EQUAL(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pi2, pi3, pi4))                                   \
       + START_RANGE_P234_QUEENSIDE                                                                                   \
       - MININDEX;                                                                                                    \
}


#define ENCODE_FLIPi(key,i,j,k)                                                                                       \
{ int pi = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition##i())];                                  \
  int pj = EndGameKeyDefinition::pawnPosToIndex[key.getPosition##j()];                                                \
  int pk = EndGameKeyDefinition::pawnPosToIndex[key.getPosition##k()];                                                \
  SORT2(pj, pk);                                                                                                      \
  if(pi <= pj) { /* pi <= pj < pk */                                                                                  \
    return ADDPIT(key, ADD3EQUALALLOWEQUALLM(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pi, pj, pk))                        \
         + START_RANGE_P34_QUEENSIDE                                                                                  \
         - MININDEX;                                                                                                  \
  } else {       /* pj < pi <= pk */                                                                                  \
    return ADDPIT(key, ADD3EQUALALLOWEQUALHM(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pj, pi, pk))                        \
         + START_RANGE_P24_QUEENSIDE                                                                                  \
         - MININDEX;                                                                                                  \
  }                                                                                                                   \
}

#define ENCODE_FLIPij(key,i,j,k)                                                                                      \
{ int pi = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition##i())];                                  \
  int pj = EndGameKeyDefinition::pawnPosToIndex[MIRRORCOLUMN(key.getPosition##j())];                                  \
  int pk = EndGameKeyDefinition::pawnPosToIndex[key.getPosition(k)];                                                  \
  SORT2(pi, pj);                                                                                                      \
  /* pi < pj < pk */                                                                                                  \
  return ADDPIT(key, ADD3EQUAL(KK_WITH_PAWN_2MEN(key), KK_POSCOUNT, pi, pj, pk))                                      \
       + START_RANGE_P4_QUEENSIDE                                                                                     \
       - MININDEX;                                                                                                    \
}

#endif // _DEBUG

unsigned long EndGameKeyDefinition5Men3EqualPawns::keyToIndex(const EndGameKey &key) const {
  switch(KEYBOOL3MASK(key, IS_KINGSIDE, 2, 3, 4)) {
  case 0: ENCODE_NOFLIP(key      );                     // 2,3,4 queenside
  case 1: ENCODE_FLIPi( key,2,3,4);                     //   3,4 queenside
  case 2: ENCODE_FLIPi( key,3,2,4);                     // 2,  4 queenside
  case 3: ENCODE_FLIPij(key,2,3,4);                     //     4 queenside
  case 4: ENCODE_FLIPi( key,4,2,3);                     // 2,3   queenside
  case 5: ENCODE_FLIPij(key,2,4,3);                     //   3   queenside
  case 6: ENCODE_FLIPij(key,3,4,2);                     // 2     queenside
  case 7: ;                                             // all kingside => error
  }
  impossibleEncodingError(key);
  return 0;
}

EndGameKey EndGameKeyDefinition5Men3EqualPawns::indexToKey(unsigned long index) const {
  index += MININDEX;
  EndGameKey result;
  if(index < START_RANGE_P24_QUEENSIDE) {
    index -= START_RANGE_P4_QUEENSIDE;
    SETPIT(              result, index  );
    SET3EQUALPAWNSFLIPij(result, index, rangeStartTable, KK_POSCOUNT, 2, 3, 4);
    SETKK_WITH_PAWN(     result, index  );
  } else if(index < START_RANGE_P34_QUEENSIDE) {
    index -= START_RANGE_P24_QUEENSIDE;
    SETPIT(              result, index  );
    SET3EQUALPAWNSFLIPj( result, index, rangeStartTable, KK_POSCOUNT, 2, 3, 4);
    SETKK_WITH_PAWN(     result, index  );
  } else if(index < START_RANGE_P234_QUEENSIDE) {
    index -= START_RANGE_P34_QUEENSIDE;
    SETPIT(              result, index  );
    SET3EQUALPAWNSFLIPi( result, index, rangeStartTable, KK_POSCOUNT, 2, 3, 4);
    SETKK_WITH_PAWN(     result, index  );
  } else {
    index -= START_RANGE_P234_QUEENSIDE;
    SETPIT(              result, index  );
    SET3EQUALPAWNSNOFLIP(result, index, rangeStartTable, KK_POSCOUNT, 2, 3, 4);
    SETKK_WITH_PAWN(     result, index  );
  } 
  return result;
}

SymmetricTransformation EndGameKeyDefinition5Men3EqualPawns::getSymTransformation(const EndGameKey &key) const {
  return get5Men3EqualPawnsSymTransformation(key);
}

#ifdef _DEBUG

void set3EqualPawnsNoFlip(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = EndGameKeyDefinition::findRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::pawnIndexToPos[r+2]);
  addr -= GET_RANGESTART3EQUAL(maxAddr/2, r);
  SET2EQUALPAWNSNOFLIP(key, addr, maxAddr, lpIndex, mpIndex);
}

void set3EqualPawnsFlipi(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = EndGameKeyDefinition::findRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::pawnIndexToPos[r+1]);
  addr -= GET_RANGESTART3EQUAL(maxAddr/2, r);
  SET2EQUALPAWNSFLIPi(key, addr, maxAddr, lpIndex, mpIndex);
}

void set3EqualPawnsFlipj(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  maxAddr /= 2;
  int r = EndGameKeyDefinition::findRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::pawnIndexToPos[r+1]);
  addr -= GET_RANGESTART3EQUAL(maxAddr, r);
  r = EndGameKeyDefinition::findRange2Equal(maxAddr, addr);
  addr -= GET_RANGESTART2EQUAL(maxAddr, r);
  r++;
  const int pos = EndGameKeyDefinition::pawnIndexToPos[r];
  key.setPosition(mpIndex, MIRRORCOLUMN(pos));
  key.setPosition(lpIndex, EndGameKeyDefinition::pawnIndexToPos[addr % r]);
  addr /= r;
}

void set3EqualPawnsFlipij(EndGameKey &key, unsigned long &addr, unsigned long *table, int tableSize, UINT maxAddr, int lpIndex, int mpIndex, int hpIndex) {
  int r = EndGameKeyDefinition::findRange(table, tableSize, addr);
  key.setPosition(hpIndex, EndGameKeyDefinition::pawnIndexToPos[r+2]);
  addr -= GET_RANGESTART3EQUAL(maxAddr/2, r);
  SET2EQUALPAWNSFLIPij(key, addr, maxAddr, lpIndex, mpIndex);
}

#endif

#ifdef TABLEBASE_BUILDER

void EndGameKeyDefinition5Men3EqualPawns::setPawnsOwner() {
  checkIsPawn(2, true);
  checkIsPawn(3, true);
  checkIsPawn(4, true);
  checkSameOwner(2, 3, true);
  checkSameOwner(2, 4, true);
  m_pawnsOwner.setOwner(0, getPieceOwner(2));
}

void EndGameKeyDefinition5Men3EqualPawns::insertInitialPositions(EndGameTablebase &tablebase) const {
  const int wkPos = A1;
  const int bkPos = C1;

  const Player pawnsOwner = m_pawnsOwner.getOwner(0);
  const int pawnsStartRow = GETPAWNSTARTROW(pawnsOwner);
  const int dp            = (pawnsOwner == WHITEPLAYER) ? 8 : -8;
  for(int col1 = 0; col1 < 4; col1++) {
    const int pawn1Pos = MAKE_POSITION(pawnsStartRow, col1);
    for(int col2 = 0; col2 < 8; col2++) {
      int pawn2Pos = MAKE_POSITION(pawnsStartRow, col2);
      if(pawn2Pos == pawn1Pos) {
        pawn2Pos += dp;
      }
      for(int col3 = 0; col3 < 8; col3++) {
        int pawn3Pos = MAKE_POSITION(pawnsStartRow, col3);
        if(pawn3Pos == pawn1Pos) {
          pawn3Pos += dp;
        }
        if(pawn3Pos == pawn2Pos) {
          pawn3Pos += dp;
        }
        EndGameKey key(*this, WHITEPLAYER, wkPos, bkPos, pawn1Pos, pawn2Pos, pawn3Pos);
        if(getSymTransformation(key) != 0 || tablebase.contains(key)) {
          continue;
        }
        tablebase.addInitPosition(WHITEPLAYER, wkPos, bkPos, pawn1Pos, pawn2Pos, pawn3Pos);
      }
    }
  }
}

void EndGameKeyDefinition5Men3EqualPawns::scanPositions(EndGameKeyWithOccupiedPositions &key, int pIndex) const {
  switch(pIndex) {
  case 0:
    { for(int pos = 0; pos < 64; pos++) {
        key.setPosition(pIndex, pos);
        scanPositions(key, 1);
        key.clearField(pos);
      }
    }
    break;
  case 1:
    { const int wkPos = key.getWhiteKingPosition();
      for(int pos = 0; pos < 64; pos++) {
        if(KINGSADJACENT(wkPos, pos) || key.isOccupied(pos)) {
          continue;
        }
        key.setPosition(pIndex, pos);
        scanPositions(key, 2);
        key.clearField(pos);
      }
    }
    break;
  case 2:
    { for(int i = 0; i < PAWN1_POSCOUNT; i++) {
        const int pos2 = pawnIndexToPos[i];
        if(key.isOccupied(pos2)) {
          continue;
        }
        key.setPosition2(pos2);
        scanPositions(key, 3);
        key.clearField(pos2);
      }
    }
    break;
  case 3:
    { const int pi2 = pawnPosToIndex[key.getPosition2()]; // always on queenside
      for(int i = pi2+1; i < PAWN_POSCOUNT; i++) {
        const int pos3 = pawnIndexToPos[i];
        if(key.isOccupied(pos3)) {
          continue;
        }
        if(IS_KINGSIDE(pos3)) {
          if(pawnPosToIndex[MIRRORCOLUMN(pos3)] >= pi2) {
            continue;
          }
        }
        key.setPosition3(pos3);
        scanPositions(key, 4);
        key.clearField(pos3);
      }
    }
    break;
  case 4:
    { const int pos3 = key.getPosition3();
      for(int i = 0; i < PAWN_POSCOUNT; i++) {
        const int pos4 = pawnIndexToPos[i];
        if(key.isOccupied(pos4)) {
          continue;
        }
        switch(BOOL2MASK(IS_QUEENSIDE, pos3, pos4)) {
        case 0: // 2 queenside, 3, 4 kingside => pi2 > max(mirrorCol(pos3), mirrorCol(pos4))
          if(pos4 < pos3) { // 3, 4 kingside. Must have pos4 > pos3
            continue;
          }
          if(pawnPosToIndex[key.getPosition2()] <= max(pawnPosToIndex[MIRRORCOLUMN(pos3)], pawnPosToIndex[MIRRORCOLUMN(pos4)])) {
            continue;
          }
          break; 
        case 1: // 2,3 queenside, 4 kingside => pi3 > pi2. must have mirrorCol(pi4) <= pi3 (=max(pi2, pi3))
          if(pawnPosToIndex[MIRRORCOLUMN(pos4)] > pawnPosToIndex[pos3]) {
            continue;
          }
          break;
        case 2: // 2,4 queenside, 3 kingside. skip
          continue; 
        case 3: // 2, 3, 4 queenside. Must have pos2 < pos3 < pos4
          if(pos4 < pos3) {
            continue;
          }
          break;
        }
        key.setPosition4(pos4);
        checkForBothPlayers(key);
        key.clearField(pos4);
      }
    }
    break;

  default:
    throwInvalidArgumentException(_T("scanPositions"), _T("pIndex=%d"), pIndex);
  }
}

void EndGameKeyDefinition5Men3EqualPawns::selfCheck() const {
  EndGameKeyWithOccupiedPositions key;
  scanPositions(key, 0);
}

String EndGameKeyDefinition5Men3EqualPawns::getCodecName() const {
  return _T("5Men3EqualPawns");
}

#endif
