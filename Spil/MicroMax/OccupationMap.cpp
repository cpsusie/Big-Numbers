#include "stdafx.h"
#include <MyAssert.h>

void OccupationMap::clear() {
  m_playerMap[0].clear();
  m_playerMap[1].clear();
}

String OccupationMap::toString() const {
  String result;
  result =  _T("White:") + m_playerMap[0].toString() + _T("\n");
  result += _T("Black:") + m_playerMap[1].toString() + _T("\n");
  return result;
}

String PlayerOccupationMap::toString() const {
  String result;
  TCHAR tmpStr[10];
  TCHAR *delim = NULL;
  for(int i = 0; i < 64; i++) {
    if(contains(i)) {
      if(delim) {
        result += delim;
      } else {
        delim = _T(",");
      }
      result += getFieldName(tmpStr, INDEXTOPOS(i));
    }
  }
  return result;
}

#if defined(__USETABLECONVERSION)

const char PlayerOccupationMap::posToIndex[120] = {
    0, 1, 2, 3, 4, 5, 6, 7,-1,-1,-1,-1,-1,-1,-1,-1
  , 8, 9,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1
  ,16,17,18,19,20,21,22,23,-1,-1,-1,-1,-1,-1,-1,-1
  ,24,25,26,27,28,29,30,31,-1,-1,-1,-1,-1,-1,-1,-1
  ,32,33,34,35,36,37,38,39,-1,-1,-1,-1,-1,-1,-1,-1
  ,40,41,42,43,44,45,46,47,-1,-1,-1,-1,-1,-1,-1,-1
  ,48,49,50,51,52,53,54,55,-1,-1,-1,-1,-1,-1,-1,-1
  ,56,57,58,59,60,61,62,63
};

const char PlayerOccupationMap::indexToPos[64] = {
     0,  1,  2,  3,  4,  5,  6,  7
  , 16, 17, 18, 19, 20, 21, 22, 23
  , 32, 33, 34, 35, 36, 37, 38, 39
  , 48, 49, 50, 51, 52, 53, 54, 55
  , 64, 65, 66, 67, 68, 69, 70, 71
  , 80, 81, 82, 83, 84, 85, 86, 87
  , 96, 97, 98, 99,100,101,102,103
  ,112,113,114,115,116,117,118,119
};

#endif
