#include "stdafx.h"
#include "EndGameUtil.h"
#include "EndGameKeyCodec.h"

static const char _wkA1BkP2I[64] = {
  -1,-1, 0, 1, 2, 3, 4, 5
 ,-1,-1, 6, 7, 8, 9,10,11
 ,-1,-1,-1,12,13,14,15,16
 ,-1,-1,-1,-1,17,18,19,20
 ,-1,-1,-1,-1,-1,21,22,23
 ,-1,-1,-1,-1,-1,-1,24,25
 ,-1,-1,-1,-1,-1,-1,-1,26
 ,-1,-1,-1,-1,-1,-1,-1,-1
};
static const char _wkA1BkI2P[27] = {
        C1,D1,E1,F1,G1,H1
       ,C2,D2,E2,F2,G2,H2
          ,D3,E3,F3,G3,H3
             ,E4,F4,G4,H4
                ,F5,G5,H5
                   ,G6,H6
                      ,H7
};
static const char _wkB2BkP2I[64] = {
  -1,-1,-1, 0, 1, 2, 3, 4
 ,-1,-1,-1, 5, 6, 7, 8, 9
 ,-1,-1,-1,10,11,12,13,14
 ,-1,-1,-1,-1,15,16,17,18
 ,-1,-1,-1,-1,-1,19,20,21
 ,-1,-1,-1,-1,-1,-1,22,23
 ,-1,-1,-1,-1,-1,-1,-1,24
 ,-1,-1,-1,-1,-1,-1,-1,-1
};
static const char _wkB2BkI2P[25] = {
           D1,E1,F1,G1,H1
          ,D2,E2,F2,G2,H2
          ,D3,E3,F3,G3,H3
             ,E4,F4,G4,H4
                ,F5,G5,H5
                   ,G6,H6
                      ,H7
};
static const char _wkC3BkP2I[64] = {
  -1, 0, 1, 2, 3, 4, 5, 6
 ,-1,-1,-1,-1, 7, 8, 9,10
 ,-1,-1,-1,-1,11,12,13,14
 ,-1,-1,-1,-1,15,16,17,18
 ,-1,-1,-1,-1,-1,19,20,21
 ,-1,-1,-1,-1,-1,-1,22,23
 ,-1,-1,-1,-1,-1,-1,-1,24
 ,-1,-1,-1,-1,-1,-1,-1,-1
};
static const char _wkC3BkI2P[25] = {
     B1,C1,D1,E1,F1,G1,H1
             ,E2,F2,G2,H2
             ,E3,F3,G3,H3
             ,E4,F4,G4,H4
                ,F5,G5,H5
                   ,G6,H6
                      ,H7
};
static const char _wkD4BkP2I[64] = {
  -1, 0, 1, 2, 3, 4, 5, 6
 ,-1,-1, 7, 8, 9,10,11,12
 ,-1,-1,-1,-1,-1,13,14,15
 ,-1,-1,-1,-1,-1,16,17,18
 ,-1,-1,-1,-1,-1,19,20,21
 ,-1,-1,-1,-1,-1,-1,22,23
 ,-1,-1,-1,-1,-1,-1,-1,24
 ,-1,-1,-1,-1,-1,-1,-1,-1
};
static const char _wkD4BkI2P[25] = {
     B1,C1,D1,E1,F1,G1,H1
       ,C2,D2,E2,F2,G2,H2
                ,F3,G3,H3
                ,F4,G4,H4
                ,F5,G5,H5
                   ,G6,H6
                      ,H7
};
static const char _kkA1BkP2I[64] = {
  -1,-1,-1,-1,-1,-1,-1,-1
 ,-1,-1,-1,-1,-1,-1,-1,-1
 ,-1,-1, 0,-1,-1,-1,-1,-1
 ,-1,-1,-1, 1,-1,-1,-1,-1
 ,-1,-1,-1,-1, 2,-1,-1,-1
 ,-1,-1,-1,-1,-1, 3,-1,-1
 ,-1,-1,-1,-1,-1,-1, 4,-1
 ,-1,-1,-1,-1,-1,-1,-1, 5
};
static const char _kkA1BkI2P[6] = {
        C3,D4,E5,F6,G7,H8
};
static const char _kkB2BkP2I[64] = {
   -1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1, 0,-1,-1,-1,-1
  ,-1,-1,-1,-1, 1,-1,-1,-1
  ,-1,-1,-1,-1,-1, 2,-1,-1
  ,-1,-1,-1,-1,-1,-1, 3,-1
  ,-1,-1,-1,-1,-1,-1,-1, 4
};
static const char _kkB2BkI2P[5] = {
   D4,E5,F6,G7,H8
};
static const char _kkC3BkP2I[64] = {
    0,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1, 1,-1,-1,-1
  ,-1,-1,-1,-1,-1, 2,-1,-1
  ,-1,-1,-1,-1,-1,-1, 3,-1
  ,-1,-1,-1,-1,-1,-1,-1, 4
};
static const char _kkC3BkI2P[5] = {
   A1,E5,F6,G7,H8
};
static const char _kkD4BkP2I[64] = {
    0,-1,-1,-1,-1,-1,-1,-1
  ,-1, 1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1, 2,-1,-1
  ,-1,-1,-1,-1,-1,-1, 3,-1
  ,-1,-1,-1,-1,-1,-1,-1, 4
};
static const char _kkD4BkI2P[5] = {
   A1,B2,F6,G7,H8
};

static const char *wkBelowDiagPosToIndex[6] = {
   _wkB1BkP2I
  ,_wkC1BkP2I
  ,_wkD1BkP2I
  ,_wkC2BkP2I
  ,_wkD2BkP2I
  ,_wkD3BkP2I
};

static const char *wkBelowDiagIndexToPos[6] = {
   _wkB1BkI2P
  ,_wkC1BkI2P
  ,_wkD1BkI2P
  ,_wkC2BkI2P
  ,_wkD2BkI2P
  ,_wkD3BkI2P
};

static const char *wkOnDiagPosToIndex[4] = {
   _wkA1BkP2I
  ,_wkB2BkP2I
  ,_wkC3BkP2I
  ,_wkD4BkP2I
};

static const char *wkOnDiagIndexToPos[4] = {
   _wkA1BkI2P
  ,_wkB2BkI2P
  ,_wkC3BkI2P
  ,_wkD4BkI2P
};

static const char *kkOnDiagPosToIndex[4] = {
   _kkA1BkP2I
  ,_kkB2BkP2I
  ,_kkC3BkP2I
  ,_kkD4BkP2I
};

static const char *kkOnDiagIndexToPos[4] = {
   _kkA1BkI2P
  ,_kkB2BkI2P
  ,_kkC3BkI2P
  ,_kkD4BkI2P
};

// s_whiteKingPosToIndex[s_whiteKingIndexToPos[index]] == index, index = [0..9]
const char EndGameKeyDefinition::s_whiteKingPosToIndex[64] = {
    6, 0, 1, 2,-1,-1,-1,-1
  ,-1, 7, 3, 4,-1,-1,-1,-1
  ,-1,-1, 8, 5,-1,-1,-1,-1
  ,-1,-1,-1, 9,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
  ,-1,-1,-1,-1,-1,-1,-1,-1
};

const char EndGameKeyDefinition::s_whiteKingIndexToPos[10] = {
      B1,C1,D1                // Sub-diagonal fields in lower left triangle
        ,C2,D2
           ,D3
  ,A1,B2,C3,D4                // Diagonal fields in lower left triangle
};

// s_subDiagPosToIndex[s_subDiagIndexToPos[index]] == index, index = [0..35]
const char EndGameKeyDefinition::s_subDiagPosToIndex[64] = {
   28, 0, 1, 2, 3, 4, 5, 6
  ,-1,29, 7, 8, 9,10,11,12
  ,-1,-1,30,13,14,15,16,17
  ,-1,-1,-1,31,18,19,20,21
  ,-1,-1,-1,-1,32,22,23,24
  ,-1,-1,-1,-1,-1,33,25,26
  ,-1,-1,-1,-1,-1,-1,34,27
  ,-1,-1,-1,-1,-1,-1,-1,35
};

const char EndGameKeyDefinition::s_subDiagIndexToPos[36] = {
      B1,C1,D1,E1,F1,G1,H1    // Sub-diagonal fields [ 0.. 6]
        ,C2,D2,E2,F2,G2,H2    //                     [ 7..12]
           ,D3,E3,F3,G3,H3    //                     [13..17]
              ,E4,F4,G4,H4    //                     [18..21]
                 ,F5,G5,H5    //                     [22..24]
                    ,G6,H6    //                     [25..26]
                       ,H7    //                     [27..27]
  ,A1,B2,C3,D4,E5,F6,G7,H8    // Diagonal fields     [28..35]
};

// s_offDiagPosToIndex[s_offDiagIndexToPos[index]] == index, index = [0..55]

const char EndGameKeyDefinition::s_offDiagPosToIndex[64] = {
   -1, 0, 1, 2, 3, 4, 5, 6
  ,28,-1, 7, 8, 9,10,11,12
  ,29,35,-1,13,14,15,16,17
  ,30,36,41,-1,18,19,20,21
  ,31,37,42,46,-1,22,23,24
  ,32,38,43,47,50,-1,25,26
  ,33,39,44,48,51,53,-1,27
  ,34,40,45,49,52,54,55,-1
};

// s_offDiagPosToIndex(MIRRORDIAG1(offdiag-position)) = 28 + s_offDiagPosToIndex(offdiag-position)

const char EndGameKeyDefinition::s_offDiagIndexToPos[56] = {
   B1,C1,D1,E1,F1,G1,H1       // Sub-diagonal fields   [ 0.. 6]
     ,C2,D2,E2,F2,G2,H2       //                       [ 7..12]
        ,D3,E3,F3,G3,H3       //                       [13..17]
           ,E4,F4,G4,H4       //                       [18..21]
              ,F5,G5,H5       //                       [22..24]
                 ,G6,H6       //                       [25..26]
                    ,H7       //                       [27..27]
  ,A2,A3,A4,A5,A6,A7,A8       // Super-diagonal fields [28..34]
     ,B3,B4,B5,B6,B7,B8       //                       [35..40]
        ,C4,C5,C6,C7,C8       //                       [41..45]
           ,D5,D6,D7,D8       //                       [46..49]
              ,E6,E7,E8       //                       [50..52]
                 ,F7,F8       //                       [53..54]
                    ,G8       //                       [55..55]
};


#define WK_INDEX_ONDIAG(wki)    ((wki) >= 6)
#define WK_INDEX_OFFDIAG(wki)   ((wki) <  6)

// Assume W.king is in the triangle [A1,D1,D4], and if W.King is on diagonal A1-D4, then B.king is on or below diagonal A1-H8
// Calculates an index in the range [0..461], which can be decoded back into the same positions with decodeKKSym8.
EndGamePosIndex EndGameKeyDefinition::encodeKKSym8(EndGameKey key) { // static
  int wki = s_whiteKingPosToIndex[key.getWhiteKingPosition()];

#if defined(_DEBUG)
  if((wki < 0)
   || POSADJACENT(key.getWhiteKingPosition(), key.getBlackKingPosition())
   || (WK_INDEX_ONDIAG(wki) && IS_ABOVEMAINDIAG1(key.getBlackKingPosition()))) {
    throwException(_T("%s:Cannot encode kings:[%s,%s]")
                  ,__TFUNCTION__
                  ,getFieldName(key.getWhiteKingPosition())
                  ,getFieldName(key.getBlackKingPosition()));
  }
#endif

  if(wki <= 2) {                                    // W.king below maindiag (edge) => B.king anywhere
    return wki * 58 + wkBelowDiagPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_BELOWDIAGB1D1;
  } else if(wki <= 5) {                             // W.king below maindiag (not edge) => B.king anywhere
    return (wki-3) * 55 + wkBelowDiagPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_BELOWDIAGC2D3;
  }
  wki -= 6;
  if(IS_OFFMAINDIAG1(key.getBlackKingPosition())) { // W.king on maindiag => B.king below
    if(wki) {                                       // W.king not in corner
      return (wki-1) * 25 + wkOnDiagPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_ONDIAGB2D4;
    } else {                                        // W.king in corner
      return wkOnDiagPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_WK_ONDIAGA1;
    }
  }
                                                    // W.king, B.king on maindiag .
  if(wki) {                                         // W.king not in corner
    return (wki-1) * 5 + kkOnDiagPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_KK_ONDIAGB2D4;
  } else {                                          // W.king in corner
    return kkOnDiagPosToIndex[wki][key.getBlackKingPosition()] + START_RANGE_KK_ONDIAGA1;
  }
}

void EndGameKeyDefinition::decodeKKSym8(EndGameKey &key, EndGamePosIndex index) { // static
  EndGamePosIndex wki;
  if(index < START_RANGE_WK_BELOWDIAGC2D3) {
    index -= START_RANGE_WK_BELOWDIAGB1D1;
    key.setWhiteKingPosition(s_whiteKingIndexToPos[wki = index / 58]);
    key.setBlackKingPosition(wkBelowDiagIndexToPos[wki][index % 58]);
  } else if(index < START_RANGE_WK_ONDIAGB2D4) {
    index -= START_RANGE_WK_BELOWDIAGC2D3;
    key.setWhiteKingPosition(s_whiteKingIndexToPos[wki = index / 55 + 3]);
    key.setBlackKingPosition(wkBelowDiagIndexToPos[wki][index % 55]);
  } else if(index < START_RANGE_WK_ONDIAGA1) {
    index -= START_RANGE_WK_ONDIAGB2D4;
    key.setWhiteKingPosition(s_whiteKingIndexToPos[(wki = index / 25 + 1) + 6]);
    key.setBlackKingPosition(wkOnDiagIndexToPos[wki][index % 25]);
  } else if(index < START_RANGE_KK_ONDIAGB2D4) {
    index -= START_RANGE_WK_ONDIAGA1;
    key.setWhiteKingPosition(s_whiteKingIndexToPos[(wki = index / 27) + 6]);
    key.setBlackKingPosition(wkOnDiagIndexToPos[wki][index % 27]);
  } else if(index < START_RANGE_KK_ONDIAGA1) {
    index -= START_RANGE_KK_ONDIAGB2D4;
    key.setWhiteKingPosition(s_whiteKingIndexToPos[(wki = index / 5 + 1) + 6]);
    key.setBlackKingPosition(kkOnDiagIndexToPos[wki][index % 5]);
  } else {
    index -= START_RANGE_KK_ONDIAGA1;
    key.setWhiteKingPosition(s_whiteKingIndexToPos[(wki = index / 6) + 6]);
    key.setBlackKingPosition(kkOnDiagIndexToPos[wki][index % 6]);
  }
}

//#define _TEST_MODULE

#if defined(_TEST_MODULE)

static EndGamePosIndex checkKey(const EndGameKey &key) {
  const EndGamePosIndex index = EndGameKeyDefinition::encodeKKSym8(key);
  EndGameKey key1;
  EndGameKeyDefinition::decodeKKSym8(key1, index);
  if(key1 != key) {
    _tprintf(_T("(wkPos,bkPos)=(%s,%s) -> %llu -> (%s,%s)\n")
          ,getFieldName(key.getWhiteKingPosition())
          ,getFieldName(key.getBlackKingPosition())
          ,index
          ,getFieldName(key1.getWhiteKingPosition())
          ,getFieldName(key1.getBlackKingPosition()));
    EndGamePosIndex index2 = EndGameKeyDefinition::encodeKKSym8(key);
    EndGameKey key2;
    EndGameKeyDefinition::decodeKKSym8(key2, index2);
  }
  return index;
}

void test2KingsSym8() {
  UINT64          keyCount = 0;
  EndGamePosIndex minIndex = -1;
  EndGamePosIndex maxIndex = 0;
  for(int wki = 0; wki < ARRAYSIZE(EndGameKeyDefinition::s_whiteKingIndexToPos); wki++) {
    const UINT wkPos = EndGameKeyDefinition::s_whiteKingIndexToPos[wki];
    for(int bkPos = 0; bkPos < 64; bkPos++) {
      if(!POSADJACENT(wkPos,bkPos) && (WK_INDEX_OFFDIAG(wki) || !IS_ABOVEMAINDIAG1(bkPos))) {
        EndGameKey key;
        key.setWhiteKingPosition(wkPos);
        key.setBlackKingPosition(bkPos);
        const EndGamePosIndex index = checkKey(key);
        minIndex = min(minIndex, index);
        maxIndex = max(maxIndex, index);
        keyCount++;
      }
    }
  }
  _tprintf(_T("keyCount:%llu\n"), keyCount);
  _tprintf(_T("minIndex:%llu\n"), minIndex);
  _tprintf(_T("maxIndex:%llu\n"), maxIndex);
  DUMP_MACRO(KK_OFFDIAG_COUNT);
  DUMP_MACRO(KK_ONDIAG_COUNT );
  DUMP_MACRO(END_RANGE_KK_ONDIAGA1);
}

#endif
